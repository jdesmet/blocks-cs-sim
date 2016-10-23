#include <classlib\objstrm.h>
#include <iostream.h>
#include "Blocks.h"
#include "initgrph.h"
#include <StdIO.H>
#include <ConIO.H>

BOverlay Root;
double TimeStep=0.01,EndTime=10;
double mini=0,maxi=2;
const int N=0;//=4;
char **Outputs;//[N]={"O:Output","E:Output","AMT:Output","IAMT:Output"};
char naam[255];

void DoLoad()
{
  cout << "\tGeef naam van het te laden systeem: ";
  cin >> naam;

  ifpstream in(naam);

  in >> Root;
  if (N)
  {
    for (i=0;i<N;i++) delete [] Outputs[N];
    delete [] Outputs;
  }
  in >> Root >> N;
  Outputs = new char * [N];
  if (N) for (int i=0;i<N;i++) Outputs[i]=in.readString();
  Root->StartLoop(0,TimeStep);
}

void DoSave()
{
  cout << "\tGeef naam van het te bewaren systeem: ";
  cin >> naam;

  ofpstream out(naam);
  out << Root << N;
  if (N) for (int i=0;i<N;i++) out << Outputs[i];
}

void DoDeleteBlock()
{
  cout << "\tGeef naam van het te wissen blok: ";
  cin >> naam;
  Root.Delete(naam);
}

void DoAddBlocks()
{
	double Value;
  BBlock* block;
  int blok,j;

  clrscr();
  for(;;)
  {
    cout << "[C]onstante, +, -, *, /, [I]ntegrator, [1]e orde, [2]e orde,\n"
            "[S]tap, [B]egrenzer, 3=Blokgolf, 4=Sinusgenerator, Z=Vertraging,\n"
            "R=Relais, [U]itdrukking, [D]ifferentiator, [V]ersterking,\n"
            "[L]ineaire/[K]wadratische interpolatie, [ESC] Exit: ";

    if ((blok=getch())==27)
    {
      cout << endl;
      clrscr();
      return;
    }

    putch (blok);
    cout << endl;
    cout << "Geef naam: ";
    cin >> naam;
    switch(blok)
    {
      case 'C': case 'c': block=new BConstant(naam); break;
      case '+':           block=new BAdder(naam); break;
      case '-':           block=new BSubstracter(naam); break;
      case '*':           block=new BMultiplier(naam); break;
      case '/':           block=new BDivider(naam); break;
      case 'I': case 'i': block=new BIntegrator(naam); break;
      case '1':           block=new BFirstOrder(naam); break;
      case 'S': case 's': block=new BStep(naam); break;
      case 'B': case 'b': block=new BLimiter(naam); break;
      case '3':           block=new BBlockGenerator(naam); break;
      case '4':           block=new BSinusGenerator(naam); break;
      case 'Z': case 'z': block=new BDelay(naam); break;
      case '2':           block=new BSecondOrder(naam); break;
      case 'R': case 'r': block=new BSwitch(naam); break;
      case 'D': case 'd': block=new BDifferentiator(naam); break;
      case 'L': case 'l':
        {
          int ntal;
          cout << "Geef het aantal waarden: ";
          cin >> ntal;
          long double *w=new long double[ntal];
          for (int i=0;i<ntal; i++)
          {
            cout << "[" << i+1 << "] ";
            cin >> w[i];
          }
          block=new BUserResponseLin(naam,w,ntal);
        }
        break;
      case 'K': case 'k':
        {
          int ntal;
          cout << "Geef het aantal waarden: ";
          cin >> ntal;
          long double *w=new long double[ntal];
          for (int i=0;i<ntal; i++)
          {
            cout << "[" << i+1 << "] ";
            cin >> w[i];
          }
          block=new BUserResponseQuad(Root,naam,w,ntal);
        }
        break;
      case 'V': case 'v': block=new BGain(Root,naam); break;
      case 'U': case 'u':
        {
          string str;
          int ninputs;
          cout << "Geef aantal inputs: ";
          cin >> ninputs;
          cout << "Geef Expressie (Inputs = I0-I9): ";
          cin >> str;
          block=new BExpression(Root,naam,str,ninputs);
          break;
        }
      case 0:  getch();
      default:
        block=0;
        cout << "Toets niet geimplementeert!" << endl;
        break;
    }
    if (block)
    {
      for (j=0;j<block->GetNInputs();j++)
      {  cout << "Input " << j+1 << ": ";
         cin >> naam;
         block->SetInput(j,naam);
      }
      for (j=0;j<block->GetNArguments();j++)
      {  cout << "Geef " << block->GetArgumentName(j) << ": ";
         cin >> Value;
         block->SetArgument(j,Value);
      }
    }
  }
}

///////////////////////
void DoGraphics(char **Outputs,int Count)
{
  double *block[N];
  int j,prevx[16],prevy[16];
  double offset;
  char str[32];
  Graphics graph;
  setwritemode(0);
  Root.Start(0,TimeStep);
  offset=Root.GetTime();
  for (j=0;j<N;j++) block[j]=Root[Outputs[j]];
  do
  {
    for (j=0;j<N;j++)
    {
      prevy[j]=((maxi-*block[j])*getmaxy())/(maxi-mini);
      prevx[j]=((Root.GetTime()-offset)*getmaxx())/(EndTime);
    }
    Root.Step();
    for (j=0;j<N;j++)
    {
      setcolor(j+1);
      line(prevx[j],prevy[j],((Root.GetTime()-offset)*getmaxx())/(EndTime),
        ((maxi-*block[j])*getmaxy())/(maxi-mini));
    }
  } while (!Root.Done(EndTime+offset));
  setcolor(15);
  line(0,(maxi*getmaxy())/(maxi-mini),getmaxx(),(maxi*getmaxy())/(maxi-mini));
  rectangle(0,0,getmaxx(),getmaxy());
  for (j=1;j<10;j++)
  {
    line(0,(j*getmaxy())/10,2,(j*getmaxy())/10);
    line(getmaxx()-2,(j*getmaxy())/10,getmaxx(),(j*getmaxy())/10);
    line((j*getmaxx())/10,0,(j*getmaxx())/10,2);
    line((j*getmaxx())/10,getmaxy()-2,(j*getmaxx())/10,getmaxy());
    line((j*getmaxx())/10,(maxi*getmaxy())/(maxi-mini)+2,
     (j*getmaxx())/10,(maxi*getmaxy())/(maxi-mini)-2);
  }
  setfillstyle(1,0);
  bar(0,0,320,4+10*(N+2));
  rectangle(0,0,320,4+10*(N+2));
  sprintf(str,"Tijd: Van %.2lf tot %.2lf s",offset,Root.GetTime());
  outtextxy(2,2,str);
  sprintf(str,"Y-schaal: Van %.2lf tot %.2lf",mini,maxi);
  outtextxy(2,2+10,str);
  for (j=0;j<N;j++)
  {
    setfillstyle(1,j+1);
    bar(1,2+(j+2)*10,20,2+(j+2)*10+9);
    outtextxy(22,2+(j+2)*10,Outputs[j]);
  }
  if (!getch()) getch();
}

void main (void)
{
try{
  ifpstream("Firstord.blk") >> Root;
  DoGraphics();
}
  catch(xmsg& x)
  {
    cout << "Error!!" << endl;
    cout << x.why();
    if (!getch()) getch();
  }
}

