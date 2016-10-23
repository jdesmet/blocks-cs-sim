#include <classlib\objstrm.h>
#include "parser.h"
#include "ilist.h"
#include "propties.h"

#define IMPEXPMACRO

class BOverlay;
class BBlock;

// klasse gebruikt voor exception-handling.
// Geeft de naam van het blok dat nog niet gedefinieerd is.
class XBUndefined:xmsg
{
public:
  XBUndefined(char *name):xmsg(name){};
};

class XBDuplicate:xmsg
{
public:
  XBDuplicate(char *name):xmsg(name){};
};


// Abstracte klasse als basis voor elke basis activiteit
// binnen een systeem.
class BBlock : public TStreamableBase
{
private:
protected:
  Properties props;
	BOverlay *Parent;         // Pointer naar container
public:
 // Constructoren en destructoren:
 // -----------------------------
	BBlock(char *Name);  // constructor
	BBlock();   // default constructor

 // Operatoren:
 // ----------
  // Kontroleert op gelijkheid:
  int operator == (const BBlock& oBlock) const;
  // Properties:
  Property& operator [] (const char *Name);

 // Data wijzigen/lezen:
 // -------------------
  void SetParent(BOverlay* poOverlay);
	virtual void Start()=0; // Nulinstellingen & constructie
  virtual void Stop(){};  // Destructie
	virtual void Step()=0;  // Berekeningen voor 1 tijdseenheid

 // Vrienden:
 // --------
  friend BOverlay; // De container heeft private toegang

  DECLARE_ABSTRACT_STREAMABLE(IMPEXPMACRO,BBlock,1);
};

// Container voor alle functionele blokken
class BOverlay
{
private:
	double TimeStep; // Grootte van een tijdseenheid
	double Time;     // Huidige systeemtijd
protected:
  typedef IList<BBlock> BBlocks;
  typedef IListIterator<BBlock> BBlockIterator;

	BBlocks Blocks;  // Lijst van alle aangemelde blokken

public:
 // Constructors & destructors
	BOverlay();
	BOverlay(ipstream& in);

 // Simulatie-members
  void Start(double from = 0.,double step = 0.01); // Begin rekenproces
  void Stop();
  void Step();    // Een stap van 1 tijdseenheid binnen het rekenproces
  int Done(double time); // Controleert op bereiken van een systeemtijd

 // Info-members
  BBlock* GetBlock(char *Name) const; // Retourneert blokref. op naambasis
  // Properties:
  Property& operator [] (const char *Name);
//  double* GetPropValueRefFromInput(char *Name) const;
  // Retourneert Block Ref van een property, input: "Block:Prop"
  BBlock* GetBlockRefFromInput(char *Name) const;
  double GetTime () const { return Time; };      // Retourneert systeemtijd
  double GetDelta () const { return TimeStep; }; // Retourneert tijdseenheid
  double* GetTimeRef () { return &Time; };      // Retourneert systeemtijd
  double* GetDeltaRef () { return &TimeStep; }; // Retourneert tijdseenheid

 // Container-control
  void Flush();
  void Delete(char *Name);
  void Add(BBlock *Block);

 // Streaming
  friend opstream& operator << (opstream& out,const BOverlay& oBOverlay);
  friend ipstream& operator >> (ipstream& in,BOverlay& oBOverlay);

  friend BBlock;                        // De blokken hebben private toegang
};

class BExpression:public BBlock,public Parser
{
private:
  double *Output;
protected:
public:
  BExpression(char *Name,char *expression="0.0");
	virtual void Start();
	virtual void Stop();
	virtual void Step();
  virtual double *GetVarRef(const char *aName);
//  virtual double EvalFunction(const char *aName);
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BExpression,BBlock);
};

/*
class BUserResponseLin:public BBlock
{
private:
protected:
  int nSize,index;
  long double *Response;
  long double prevTime;
public:
	BUserResponseLin(BOverlay *oOverlay,char *Name,long double *resp,int Size);
	~BUserResponseLin();
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE(IMPEXPMACRO,BUserResponseLin,1);
};

class BUserResponseQuad:public BUserResponseLin
{
private:
protected:
public:
	BUserResponseQuad(BOverlay *oOverlay,char *Name,long double *resp,int Size);
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BUserResponseQuad,BUserResponseLin);
};
*/

class BDifferentialEQ:public BBlock
{
private:
protected:
  double *Output,*Input,*Time,*Delta;
  int NEquations;
  double *y,*Y,*k1,*k2,*k3,*k4;
	virtual double f(int Equation,double Time)=0;
public:
	BDifferentialEQ(char *Name,int nEquations);
	virtual void Start();
	virtual void Stop();
	virtual void Step();
  DECLARE_ABSTRACT_STREAMABLE(IMPEXPMACRO,BDifferentialEQ,1);
};

class BIntegrator:public BDifferentialEQ
{
private:
  double *Gain;
protected:
	virtual double f(int Equation,double Time);
public:
	BIntegrator(char *Name);
	virtual void Start();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BIntegrator,BDifferentialEQ);
};

class BFirstOrder:public BDifferentialEQ
{
private:
  double *Gain,*TimeConstant;
protected:
	virtual double f(int Equation,double Time);
public:
	BFirstOrder(char *Name);
	virtual void Start();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BFirstOrder,BDifferentialEQ);
};

class BSecondOrder:public BDifferentialEQ
{
private:
  double *Gain,*Vibration,*Suppression;
protected:
	virtual double f(int Equation,double Time);
public:
	BSecondOrder(char *Name);
	virtual void Start();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BSecondOrder,BDifferentialEQ);
};

class BConstant:public BBlock
{
private:
protected:
public:
	BConstant(char *Name);
	virtual void Step(){};
	virtual void Start();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BConstant,BBlock);
};

class BDifferentiator:public BBlock
{
private:
  double Last;
  double *Input,*Output,*TimeConstant,*Delta;
protected:
public:
	BDifferentiator(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BDifferentiator,BBlock);
};

class BGain:public BBlock
{
private:
  double *Gain,*Input,*Output;
protected:
public:
	BGain(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BGain,BBlock);
};

class BAdder:public BBlock
{
private:
protected:
  double *Input1,*Input2,*Output;
public:
	BAdder(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BAdder,BBlock);
};

class BSubstracter:public BAdder
{
private:
protected:
public:
	BSubstracter(char *Name);
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BSubstracter,BBlock);
};

class BMultiplier:public BAdder
{
private:
protected:
public:
	BMultiplier(char *Name);
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BMultiplier,BBlock);
};

class BDivider:public BAdder
{
private:
protected:
public:
	BDivider(char *Name);
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BDivider,BBlock);
};

class BLimitter:public BBlock
{
private:
  double *Input,*Output,*UpperLimit,*LowerLimit;
protected:
public:
	BLimitter(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BLimitter,BBlock);
};

class BSwitch:public BBlock
{
private:
  double *Input,*Output,*On,*Off;
protected:
public:
	BSwitch(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BSwitch,BBlock);
};

class BStep:public BBlock
{
private:
  double *On,*Off,*Delay,*Output,*Time;
protected:
public:
	BStep(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BStep,BBlock);
};

class BBlockGenerator:public BBlock
{
private:
  double Offset,*On,*Off,*Output,*Period,*DutyCycle,*Time;
protected:
public:
	BBlockGenerator(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BBlockGenerator,BBlock);
};

class BSinusGenerator:public BBlock
{
private:
  double offset,*Amplitude,*Offset,*Output,*Period,*Time;
protected:
public:
	BSinusGenerator(char *Name);
	virtual void Start();
	virtual void Step();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BSinusGenerator,BBlock);
};

class BDelay:public BBlock
{
private:
  double huge * Buffer;
  double *Input,*Output,*Delay;
  unsigned int Index,uintDelay;
protected:
public:
	BDelay(char *Name);
	virtual void Start();
	virtual void Step();
	virtual void Stop();
  DECLARE_STREAMABLE_FROM_BASE(IMPEXPMACRO,BDelay,BBlock);
};


