#include "Blocks.h"
#include <math.h>

IMPLEMENT_CASTABLE(BBlock);

IMPLEMENT_CASTABLE1(BIntegrator,BDifferentialEQ);
IMPLEMENT_CASTABLE1(BFirstOrder,BDifferentialEQ);
IMPLEMENT_CASTABLE1(BSecondOrder,BDifferentialEQ);
IMPLEMENT_CASTABLE1(BConstant,BBlock);
IMPLEMENT_CASTABLE1(BDifferentiator,BBlock);
IMPLEMENT_CASTABLE1(BGain,BBlock);
IMPLEMENT_CASTABLE1(BAdder,BBlock);
IMPLEMENT_CASTABLE1(BSubstracter,BAdder);
IMPLEMENT_CASTABLE1(BMultiplier,BAdder);
IMPLEMENT_CASTABLE1(BDivider,BAdder);
IMPLEMENT_CASTABLE1(BLimitter,BBlock);
IMPLEMENT_CASTABLE1(BSwitch,BBlock);
IMPLEMENT_CASTABLE1(BStep,BBlock);
IMPLEMENT_CASTABLE1(BBlockGenerator,BBlock);
IMPLEMENT_CASTABLE1(BSinusGenerator,BBlock);
IMPLEMENT_CASTABLE1(BDelay,BBlock);
IMPLEMENT_CASTABLE1(BDifferentialEQ,BBlock);
IMPLEMENT_CASTABLE1(BExpression,BBlock);
//IMPLEMENT_CASTABLE1(BUserResponseLin,BBlock);
//IMPLEMENT_CASTABLE1(BUserResponseQuad,BUserResponseLin);

IMPLEMENT_ABSTRACT_STREAMABLE(BBlock);

IMPLEMENT_ABSTRACT_STREAMABLE1(BDifferentialEQ,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BIntegrator,BDifferentialEQ);
IMPLEMENT_STREAMABLE_FROM_BASE(BFirstOrder,BDifferentialEQ);
IMPLEMENT_STREAMABLE_FROM_BASE(BSecondOrder,BDifferentialEQ);
IMPLEMENT_STREAMABLE_FROM_BASE(BConstant,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BDifferentiator,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BGain,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BAdder,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BSubstracter,BAdder);
IMPLEMENT_STREAMABLE_FROM_BASE(BMultiplier,BAdder);
IMPLEMENT_STREAMABLE_FROM_BASE(BDivider,BAdder);
IMPLEMENT_STREAMABLE_FROM_BASE(BLimitter,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BSwitch,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BStep,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BBlockGenerator,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BSinusGenerator,BBlock);
IMPLEMENT_STREAMABLE_FROM_BASE(BDelay,BBlock);
//IMPLEMENT_STREAMABLE1(BUserResponseLin,BBlock);
//IMPLEMENT_STREAMABLE_FROM_BASE(BUserResponseQuad,BUserResponseLin);
IMPLEMENT_STREAMABLE_FROM_BASE(BExpression,BBlock);

BBlock::BBlock()
:Parent(0)
{
  props["Type"]="Block";
}

BBlock::BBlock(char *Name)
:Parent(0)
{
  props["Type"]="Block";
  props["Name"]=Name;
}

void *BBlock::Streamer::Read(ipstream& in,uint32) const
{
  in >> GetObject()->props;
  return GetObject();
}

void BBlock::Streamer::Write(opstream& out) const
{
  out << GetObject()->props;
}

int BBlock::operator == (const BBlock& oBlock) const
{
  return strcmp(props["Name"],oBlock.props["Name"])==0;
}

Property& BBlock::operator [] (const char *Name)
{
  return props[Name];
}

void BBlock::SetParent(BOverlay* poOverlay)
{
  Parent=poOverlay;
}
/*
void BBlock::Start()
{
  // Bij afgelede klassen moet hier de code staan om
  // een versnelde toegang tot de properties
  // te verkrijgen(via pointers).
}
*/

BOverlay::BOverlay()
{
	TimeStep=0.001;
	Time=0;
}

BOverlay::BOverlay(ipstream& in):Time(0)
{
  int count;
  BBlock *Block;

	in >> TimeStep >> count;

	while(count--)
  {
    in >> Block;
    Block->SetParent(this);
    Blocks.Add(Block);
  }
}

void BOverlay::Flush()
{
  Blocks.Flush();
}

void BOverlay::Delete(char *Name) 
{
  Blocks.Delete(GetBlock(Name));
}

void BOverlay::Add(BBlock *oBlock)
{
	BBlock *b;

	BBlockIterator BlockIterator(Blocks);
	while(BlockIterator)
	{
		b=BlockIterator++;
		if(strcmp((*b)["Name"],(*oBlock)["Name"])==0)
    {
      delete oBlock;
      throw XBDuplicate((*b)["Name"]);
    }
	}
/*
// Moet geïmplementeerd worden als constant block!! -> later
  if (oBlock->Name == "PI" || oBlock->Name == "e" || oBlock->Name == "TIME")
  {
    string Temp(oBlock->Name);
    delete oBlock;
    throw XBDuplicate(Temp);
  }
*/
  oBlock->SetParent(this);
  Blocks.Add(oBlock);
}

Property& BOverlay::operator [] (const char *Name)
{
  char *ptr;
  char *cpy=new char[strlen(Name)];
  strcpy(cpy,Name);
  for (ptr=cpy;*ptr!=0 && *ptr!=':';ptr++);
  if (ptr==0) throw XBUndefined("");
  *ptr++=0;
  Property *ret=&(*GetBlock(cpy))[ptr];
  delete [] cpy;
  return *ret;
}

BBlock* BOverlay::GetBlockRefFromInput(char *Name) const
{
  char *ptr;
  char *cpy=new char[strlen(Name)];
  strcpy(cpy,Name);
  for (ptr=cpy;*ptr!=0 && *ptr!=':';ptr++);
  if (ptr==0) throw XBUndefined("");
  *ptr=0;
  BBlock *Block=GetBlock(cpy);
  delete [] cpy;
  return Block;
}

BBlock* BOverlay::GetBlock(char *Name) const
{
	BBlock *b;

	BBlockIterator BlockIterator(Blocks);
	while(BlockIterator)
	{
		b=BlockIterator++;
		if(strcmp((*b)["Name"],Name)==0) return (BBlock *)b;
	}
  throw XBUndefined(Name);
}

void BOverlay::Start(double from,double step)
{
  TimeStep=step;
  Time = 0.;
	BBlockIterator BlockIterator(Blocks);
	while(BlockIterator!=0) (BlockIterator++)->Start();
  if (from != 0.) while(!Done(from)) Step();
}

void BOverlay::Step()
{
  Time+=TimeStep;
	BBlockIterator BlockIterator(Blocks);
  while(BlockIterator!=0) (BlockIterator++)->Step();
}

int BOverlay::Done(double time)
{
  return Time>(time-TimeStep/2);
}

opstream& operator << (opstream& out,const BOverlay& oBOverlay)
{
	out << oBOverlay.TimeStep << oBOverlay.Blocks.Count();

	BOverlay::BBlockIterator BlockIterator(oBOverlay.Blocks);
	while(BlockIterator) out << (BBlock *)BlockIterator++;

  return out;
}

ipstream& operator >> (ipstream& in,BOverlay& oBOverlay)
{
  int count;
  BBlock *Block;

	in >> oBOverlay.TimeStep >> count;
  oBOverlay.Blocks.Flush();

	while(count--)
  {
    in >> Block;
    Block->SetParent(&oBOverlay);
    oBOverlay.Blocks.Add(Block);
  }

  return in;
}


BExpression::BExpression(char *Name,char *expression)
:BBlock(Name)
{
  props["Type"]="Expression";
  props["Expression"]=expression;
  props["Output"]=0.0;
}

void BExpression::Stop()
{
  Parser::Stop();
}

void BExpression::Start()
{
  SetExpression(props["Expression"]);
  Parser::Start();
  Output=props["Output"];
  Step();
}

void BExpression::Step()
{
  *Output=Evaluate();
}

double *BExpression::GetVarRef(const char *aName)
{
  if (strcmp(aName,"TIME")==0) return Parent->GetTimeRef();
  if (strcmp(aName,"STEP")==0) return Parent->GetDeltaRef();
  return (*Parent)[aName];
}

/*
BUserResponseLin
::BUserResponseLin(BOverlay *oOverlay,char *Name,long double *resp,int Size)
:BBlock(oOverlay,Name,0,1)
{
  ArgumentName[0] = string("Stap");
  Response = new long double [Size];
  nSize=Size;
  for (int i=0;i<Size;i++) Response[i] = resp[i];
}

BUserResponseLin::~BUserResponseLin()
{
  delete [] Response;
}

void *BUserResponseLin::Streamer::Read(ipstream& in,uint32) const
{
  ReadBaseObject( STATIC_CAST(BBlock *,GetObject()), in );
  in >> GetObject()->nSize;
  GetObject()->Response = new long double [GetObject()->nSize];
  for (int i=0; i<GetObject()->nSize; i++) in >> GetObject()->Response[i];
  return GetObject();
}

void BUserResponseLin::Streamer::Write(opstream& out) const
{
  WriteBaseObject( STATIC_CAST(BBlock *,GetObject()), out );
  out << GetObject()->nSize;
  for (int i=0; i<GetObject()->nSize; i++) out << GetObject()->Response[i];
}

void BUserResponseLin::Start()
{
  prevTime = 0;
  index=0;
  Output = Response[0];
}

void BUserResponseLin::Step()
{
  int i1,i2,i3;
  double t=Parent->GetTime();
  double h=Parent->GetDelta()/2;
  if (t>=(prevTime+Argument[0]-h))
  {
    index++;
    prevTime=t;
  }
  if (index==nSize) index=0;
  i1=index;
  i2=index+1;
  if (i2==nSize) i2=0;
  t-=prevTime;
  t/=Argument[0];
  Output=Response[i1]+(Response[i2]-Response[i1])*t;
}

BUserResponseQuad
::BUserResponseQuad(BOverlay *oOverlay,char *Name,long double *resp,int Size)
:BUserResponseLin(oOverlay,Name,resp,Size)
{
}

void BUserResponseQuad::Step()
{
  int i0,i1,i2;
  double t=Parent->GetTime();
  double h=Parent->GetDelta()/2;
  double y0,y1,y2;
  if (t>=(prevTime+Argument[0]-h))
  {
    index++;
    prevTime=t;
  }
  if (index==nSize) index=0;
  i0=index-1;
  i1=index;
  i2=index+1;
  if (i0<0) i0=nSize-1;
  if (i2>=nSize) i2=0;
  y0=Response[i0];
  y1=Response[i1];
  y2=Response[i2];
  t-=prevTime;
  t/=Argument[0];
  Output=0.5*t*t*(y0-y1-y1+y2)+0.5*t*(y2-y0)+y1;
}
*/

BDifferentialEQ::BDifferentialEQ(char *Name,int nEquations)
:BBlock(Name),NEquations(nEquations)
{
  props["Type"]="Differential Equations";
  props["Input"]="None";
  props["Output"]=0.0;
  props["Start"]=0.0;
}

void *BDifferentialEQ::Streamer::Read(ipstream& in,uint32) const
{
  ReadBaseObject( STATIC_CAST(BBlock *,GetObject()), in );
  in >> GetObject()->NEquations;
  return GetObject();
}

void BDifferentialEQ::Streamer::Write(opstream& out) const
{
  WriteBaseObject( STATIC_CAST(BBlock *,GetObject()), out );
  out << GetObject()->NEquations;
}

void BDifferentialEQ::Start()
{
  y  = new double [NEquations];
  Y  = new double [NEquations];
  k1 = new double [NEquations];
  k2 = new double [NEquations];
  k3 = new double [NEquations];
  k4 = new double [NEquations];
  for (int i=0;i<NEquations;i++) y[i]=0;
  props["Output"]=0.0;
  Output=props["Output"];
  y[0]=props["Start"];
  Input=(*Parent)[props["Input"]];
  Time=Parent->GetTimeRef();
  Delta=Parent->GetDeltaRef();
}

void BDifferentialEQ::Stop()
{
  delete [] y;
  delete [] Y;
  delete [] k1;
  delete [] k2;
  delete [] k3;
  delete [] k4;
}

// Opgepast de formules van Runge Kutta zien er ietwat anders uit omdat
// die aangepast is voor formules waarvan er veranderlijken ifv de tijd
// van een blok verschillend van het huidige blok voorkomen. Voor dit
// geval worden deze veranderlijken namelijk als konstant veronderstelt.
// Of anders: de enige veranderlijke is hier de tijd en alle variabelen
// afhankelijk van de tijd binnen éénzelfde blok!
void BDifferentialEQ::Step()
{
  int i;

  for (i=0;i<NEquations;i++) Y[i]=y[i];
  for (i=0;i<NEquations;i++)
    k1[i]=f(i,*Time) * *Delta;
  for (i=0;i<NEquations;i++) Y[i]=y[i]+k1[i]/2.0;
  for (i=0;i<NEquations;i++)
    k2[i]=f(i,*Time + *Delta/2.0) * *Delta;
  for (i=0;i<NEquations;i++) Y[i]=y[i]+k2[i]/2.0;
  for (i=0;i<NEquations;i++)
    k3[i]=f(i,*Time + *Delta/2.0) * *Delta;
  for (i=0;i<NEquations;i++) Y[i]=y[i]+k3[i];
  for (i=0;i<NEquations;i++)
    k4[i]=f(i,*Time + *Delta) * *Delta;

	for (i=0;i<NEquations;i++) y[i]+=(k1[i]+2*(k2[i]+k3[i])+k4[i])/6.0;

  *Output=y[0];
}


BIntegrator::BIntegrator(char *Name)
:BDifferentialEQ(Name,1)
{
  props["Type"]="Simple integrator";
  props["Gain"]=1.0;
}

void BIntegrator::Start()
{
  BDifferentialEQ::Start();
  Gain=props["Gain"];
}

#pragma argsused
double BIntegrator::f(int Equation,double Time)
{
	return *Gain * *Input;
}

BFirstOrder::BFirstOrder(char *Name)
:BDifferentialEQ(Name,1)
{
  props["Type"]="First order integrator";
  props["Gain"]=1.0;
  props["TimeConstant"]=1.0;
}

void BFirstOrder::Start()
{
  BDifferentialEQ::Start();
  Gain=props["Gain"];
  TimeConstant=props["TimeConstant"];
}

#pragma argsused
double BFirstOrder::f(int Equation,double Time)
{
	return (*Gain * *Input - Y[0]) / *TimeConstant;
}

BSecondOrder::BSecondOrder(char *Name)
:BDifferentialEQ(Name,2)
{
  props["Type"]="Second order integrator";
  props["Gain"]=1.0;
  props["Vibration"]=1.0;
  props["Suppression"]=1.0;
}

void BSecondOrder::Start()
{
  BDifferentialEQ::Start();
  Gain=props["Gain"];
  Vibration=props["Vibration"];
  Suppression=props["Suppression"];
}

#pragma argsused
double BSecondOrder::f(int Equation,double Time)
{
  if(Equation==0) return Y[1];
  return *Vibration * *Vibration * (*Input * *Gain-Y[0])-
         2.0 * *Suppression * *Vibration * Y[1];
}

BConstant::BConstant(char *Name)
:BBlock(Name)
{
  props["Type"]="Constant";
  props["Value"]=1.0;
  props["Output"]=0.0;
}

void BConstant::Start()
{
  props["Output"] = (real) props["Value"];
}

BDifferentiator::BDifferentiator(char *Name):
BBlock(Name)
{
  props["Type"]="Differentiator";
  props["TimeConstant"]=1.0;
  props["Input"]="None";
  props["Output"]=0.0;
}

void BDifferentiator::Start()
{
  Parent->GetBlockRefFromInput(props["Input"])->Start();
  Input= (*Parent)[props["Input"]];
	Last = *Input;
  props["Output"]=0.0;
  TimeConstant=props["TimeConstant"];
  Output=props["Output"];
  Delta=Parent->GetDeltaRef();
}

void BDifferentiator::Step()
{
  *Output = *TimeConstant * (*Input - Last) / *Delta;
  Last = *Input;
}

BGain::BGain(char *Name):
BBlock(Name)
{
  props["Type"]="Gain";
  props["Gain"]=1.0,
  props["Input"]="None";
  props["Output"]=0.0;
}

void BGain::Start()
{
  Parent->GetBlockRefFromInput(props["Input"])->Start();
  Input= (*Parent)[props["Input"]];
  Gain=props["Gain"];
  Output=props["Output"];
	Step();
}

void BGain::Step()
{
  *Output = *Input * *Gain;
}

BAdder::BAdder(char *Name):
BBlock(Name)
{
  props["Type"]="Adder";
  props["Input1"]="None";
  props["Input2"]="None";
  props["Output"]=0.0;
}

void BAdder::Start()
{
  Parent->GetBlockRefFromInput(props["Input1"])->Start();
  Parent->GetBlockRefFromInput(props["Input2"])->Start();
  Input1= (*Parent)[props["Input1"]];
  Input2= (*Parent)[props["Input2"]];
  Output=props["Output"];
	Step();
}

void BAdder::Step()
{
	*Output= *Input1 + *Input2;
}

BMultiplier::BMultiplier(char *Name):
BAdder(Name)
{
  props["Type"]="Multiplier";
}

void BMultiplier::Step()
{
	*Output= *Input1 * *Input2;
}

BDivider::BDivider(char *Name):
BAdder(Name)
{
  props["Type"]="Divider";
}

void BDivider::Step()
{
	*Output= *Input1 / *Input2;
}

BSubstracter::BSubstracter(char *Name):
BAdder(Name)
{
  props["Type"]="Substracter";
}

void BSubstracter::Step()
{
	*Output= *Input1 - *Input2;
}

BLimitter::BLimitter(char *Name)
:BBlock(Name)
{
  props["Type"]="Limitter";
  props["UpperLimit"]=1.0;
  props["LowerLimit"]=0.0;
  props["Input"]="None";
  props["Output"]=0.0;
}

void BLimitter::Start()
{
  Parent->GetBlockRefFromInput(props["Input"])->Start();
  Input = (*Parent)[props["Input"]];
  Output=props["Output"];
  UpperLimit=props["UpperLimit"];
  LowerLimit=props["LowerLimit"];
	Step();
}

void BLimitter::Step()
{
  *Output = *Input;
  if (*Output > *UpperLimit) *Output=*UpperLimit;
  else if (*Output < *LowerLimit) *Output=*LowerLimit;
}

BSwitch::BSwitch(char *Name)
:BBlock(Name)
{
  props["Type"]="Switch";
  props["On"]=1.0;
  props["Off"]=0.0;
  props["Input"]="None";
  props["Output"]=0.0;
}

void BSwitch::Start()
{
  Parent->GetBlockRefFromInput(props["Input"])->Start();
  Input = (*Parent)[props["Input"]];
  Output=props["Output"];
  On=props["On"];
  Off=props["Off"];
	Step();
}

void BSwitch::Step()
{
  *Output = *Input<0?*Off:*On;
}

BStep::BStep(char *Name)
:BBlock(Name)
{
  props["Type"]="Step";
  props["On"]=1.0;
  props["Off"]=0.0;
  props["Delay"]=0.0;
  props["Output"]=0.0;
}

void BStep::Start()
{
  Output=props["Output"];
  On=props["On"];
  Off=props["Off"];
  Delay=props["Delay"];
  *Output=*Off;
  Time=Parent->GetTimeRef();
}

void BStep::Step()
{
  *Output = (*Time >= *Delay)?*On:*Off;
}

BBlockGenerator::BBlockGenerator(char *Name)
:BBlock(Name)
{
  props["Type"]="BlockWave";
  props["On"]=1.0;
  props["Off"]=0.0;
  props["Period"]=0.1;
  props["DutyCycle"]=0.5;
  props["Output"]=0.0;
}

void BBlockGenerator::Start()
{
  Output=props["Output"];
  On=props["On"];
  Off=props["Off"];
  Period=props["Period"];
  DutyCycle=props["DutyCycle"];
  *Output=*On;
  Offset=0;
  Time=Parent->GetTimeRef();
}

void BBlockGenerator::Step()
{
  if ((*Time - Offset) >= *Period) Offset=*Time;
  *Output = ((*Time - Offset) > (*Period * *DutyCycle))?*Off:*On;
}

BSinusGenerator::BSinusGenerator(char *Name)
:BBlock(Name)
{
  props["Type"]="SineWave";
  props["Amplitude"]=1.0;
  props["Period"]=0.1;
  props["Offset"]=0.0;
  props["Output"]=0.0;
}

void BSinusGenerator::Start()
{
  Output=props["Output"];
  Period=props["Period"];
  Offset=props["Offset"];
  Amplitude=props["Amplitude"];
  *Output=0.0;
  offset=0.0;
  Time=Parent->GetTimeRef();
}

void BSinusGenerator::Step()
{
  *Output = *Offset + *Amplitude * sin((2.0*M_PI) / *Period * *Time);
}

BDelay::BDelay(char *Name)
:BBlock(Name)
{
  props["Type"]="Delay";
  props["Delay"]=1.0;
  props["Input"]="None";
  props["Output"]=0.0;
}

void BDelay::Start()
{
  Output=props["Output"];
  Delay=props["Delay"];
  Input = (*Parent)[props["Input"]];
  *Output=0.0;

  uintDelay=(unsigned int)(*Delay / Parent->GetDelta());
  Buffer = new huge double [uintDelay];
  for (unsigned int i=0;i<uintDelay;i++) Buffer[i]=0;
  Index=0;
}

void BDelay::Step()
{
  *Output = Buffer[Index];
  Buffer[Index] = *Input;
  Index++;
  if (Index==uintDelay) Index=0;
}

void BDelay::Stop()
{
  delete [] Buffer;
}

