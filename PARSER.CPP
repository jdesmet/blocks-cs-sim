#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include "parser.h"
#include "stack.h"

#define BASEPOINTER 10

typedef enum {FALSE,TRUE} BOOL;

const char *XParser::GetErrorText () const
{
  switch(Type)
  {
  case FORMAT:    return "Wrong number-format";
  case SYNTAX:    return "Wrong syntaxis";
  case UFUNCTION: return "Undefined function";
  case UVARIABLE: return "Undefined variable";
  case USYMBOL:   return "Undefined symbol in use";
  case ARGUMENTS: return "Wrong number of parameters";
  default:        return "No known error occured";
  }
}

Token::Token(TokenType type):Type(type){};

Token::Token(FuncType func):Type(FNC),Func(func){};

Token::Token(double val):Type(CON),Value(val){};
Token::Token(double *valueref):Type(VAR),ValueRef(valueref){};

Token::Token(const Token &oToken):Type(oToken.Type)
{
  if (Type == FNC)
  {
    Func=oToken.Func;
    if (Func == USR)
    {
      Name = new char [strlen(oToken.Name)+1];
      strcpy(Name,oToken.Name);
    }
  }
  else if (Type == VAR) ValueRef = oToken.ValueRef;
  else if (Type == CON) Value = oToken.Value;
}

Token::Token(const char *str):Type(FNC),Func(USR)
{
  Name = new char [strlen(str)+1];
  strcpy(Name,str);
}

Token::~Token()
{ if (Type==FNC && Func==USR) delete [] Name; }

Token& Token::operator = (const Token &oToken)
{
  if (Type == FNC && Func == USR) delete Name;
  Type = oToken.Type;
  if (Type == FNC)
  {
    Func=oToken.Func;
    if (Func == USR)
    {
      Name = new char [strlen(oToken.Name)+1];
      strcpy(Name,oToken.Name);
    }
  }
  else if (Type == VAR) ValueRef = oToken.ValueRef;
  else if (Type == CON) Value = oToken.Value;
  return *this;
}

int Token::operator == (const Token &oToken)
{
  if (Type != oToken.Type) return 0;
  if (Type == FNC)
  {
    if (Func!=oToken.Func) return 0;
    if (Func == USR) return strcmp(Name,oToken.Name)==0;
    return 1;
  }
  else if (Type == VAR) return ValueRef == oToken.ValueRef;
  else if (Type == CON) return Value == oToken.Value;
  return 0;
}

Parser::Parser():TheEquation(0),Values(0){ }

Parser::Parser(const char *expression):Values(0)
{
  TheEquation=new char [strlen(expression)+1];
  strcpy(TheEquation,expression);
}

Parser::~Parser()
{ if (TheEquation) delete TheEquation; Stop(); }

Token Parser::NextToken(const Token& prevToken)
{
// Spaties overslaan:
  while (*Position == ' ') Position++;

// Einde-markering:
  if (*Position == 0) return Token(Token::MRK);

// Teken omkeren:
  if (*Position == '-' && prevToken.Type<=Token::CMA)
  {
    Position++;
    return Token(Token::MNS);
  }

// Niets doen:
  if (*Position == '+' && prevToken.Type<=Token::CMA)
  {
    Position++;
    return Token(Token::NOP);
  }

// Reëel getal:
  if ( isdigit(*Position) || *Position== '.' )
  {
    BOOL isDecimal=FALSE;
    int exponent=0;
    double value=0.;

    while ( isdigit(*Position) || *Position == '.')
    {
      if (*Position == '.')
      { if (isDecimal) throw XParser(XParser::FORMAT);
        isDecimal = TRUE;
      }
      else
      {
        value = value*10. + *Position - '0';
        if (isDecimal) exponent--;
      }
      Position++;
    }
    if (value == 0. && *Position == '.') throw XParser(XParser::FORMAT);
    if (*Position == 'E' || *Position == 'e')
    {
      int ExpLength=0,ExpVal=0;
      BOOL positive=TRUE;
      Position++;
      if (*Position == '+') Position++;
      if (*Position == '-') { Position++; positive=FALSE;}
      while ((isdigit(*Position)) && (++ExpLength <= 3))
        ExpVal=ExpVal*10. + *Position++ - '0';
      if (!ExpLength||ExpLength==4) throw XParser(XParser::FORMAT);
      if (!positive) exponent-=ExpVal;
      else exponent +=ExpVal;
    }
    return Token(value*pow10(exponent));
  }

// Functies of variabelen:
  if (isalpha(*Position))
  {
    char *start=Position++;
    while ( isalnum(*Position) ) Position++;
    if (*Position==':') Position++;
    while ( isalnum(*Position) ) Position++;
    char *Name=new char[(int)(Position-start)+1];
    strncpy(Name,start,(int)(Position-start));
    Name[(int)(Position-start)]=0;
    if (*Position == '(')
    {
      Position++;
    // Standaard functies:
      if (!strcmp(Name,"abs"  )) return Token(Token::ABS);
      if (!strcmp(Name,"acos" )) return Token(Token::ACS);
      if (!strcmp(Name,"asin" )) return Token(Token::ASN);
      if (!strcmp(Name,"atan" )) return Token(Token::ATN);
      if (!strcmp(Name,"cosh" )) return Token(Token::CSH);
      if (!strcmp(Name,"cos"  )) return Token(Token::COS);
      if (!strcmp(Name,"exp"  )) return Token(Token::EXP);
      if (!strcmp(Name,"log"  )) return Token(Token::LOG);
      if (!strcmp(Name,"ln"   )) return Token(Token::LN );
      if (!strcmp(Name,"round")) return Token(Token::RND);
      if (!strcmp(Name,"root" )) return Token(Token::RT );
      if (!strcmp(Name,"pow10")) return Token(Token::P10);
      if (!strcmp(Name,"sinh" )) return Token(Token::SNH);
      if (!strcmp(Name,"sin"  )) return Token(Token::SIN);
      if (!strcmp(Name,"sqrt" )) return Token(Token::SQT);
      if (!strcmp(Name,"sqr"  )) return Token(Token::SQR);
      if (!strcmp(Name,"tanh" )) return Token(Token::TNH);
      if (!strcmp(Name,"tan"  )) return Token(Token::TAN);
      if (!strcmp(Name,"trunc")) return Token(Token::TRC);
      if (!strcmp(Name,"sign" )) return Token(Token::SGN);
      if (!strcmp(Name,"min"  )) return Token(Token::MIN);
      if (!strcmp(Name,"max"  )) return Token(Token::MAX);
      if (!strcmp(Name,"random"  )) return Token(Token::RAN);
   // Door gebruiker gedefinieerde functie (door afleiden):
      return Token(Name);
    }
    else
    {
    // Zijn het constanten:
      if (!strcmp(Name,"PI" )) return Token(M_PI);
      if (!strcmp(Name,"e"  )) return Token(M_E );
    // Zijn het variabelen:
      return Token(GetVarRef(Name));
    }
  }

// Operatoren:
  switch(*Position++)
  {
  case '+': return Token(Token::ADD);
  case '-': return Token(Token::SUB);
  case '*': return Token(Token::MUL);
  case '/': return Token(Token::DIV);
  case '(': return Token(Token::OPA);
  case ')': return Token(Token::CPA);
  case '^': return Token(Token::POW);
  case ',': return Token(Token::CMA);
  default : throw XParser(XParser::USYMBOL);
  }
}

#pragma argsused
double Parser::EvalFunction(const char *name)
{
  throw XParser(XParser::UFUNCTION);
}

double Parser::EvalFunction(const Token& aToken)
{
  double Value1,Value2;
  switch(aToken.Func)
  {
    case Token::ABS: return fabs(Values[--sp]);
    case Token::ACS: return acos(Values[--sp]);
    case Token::ASN: return asin(Values[--sp]);
    case Token::ATN: return atan(Values[--sp]);
    case Token::CSH: return cosh(Values[--sp]);
    case Token::COS: return cos(Values[--sp]);
    case Token::EXP: return exp(Values[--sp]);
    case Token::LOG: return log10(Values[--sp]);
    case Token::LN : return log(Values[--sp]);
    case Token::RND: return (double)(int)Values[--sp];
    case Token::RT :
      Value1 = Values[--sp];
      return pow(Values[--sp],1/Value1);
    case Token::P10: return pow10(Values[--sp]);
    case Token::SNH: return sinh(Values[--sp]);
    case Token::SIN: return sin(Values[--sp]);
    case Token::SQT: return sqrt(Values[--sp]);
    case Token::SQR:
      Value1 = Values[--sp];
      return Value1*Value1;
    case Token::TNH: return tanh(Values[--sp]);
    case Token::TAN: return tan(Values[--sp]);
    case Token::TRC: return floor(Values[--sp]);
    case Token::SGN: return Values[--sp]<0?-1.0:1.0;
    case Token::MIN:
      Value1 = Values[--sp];
      Value2 = Values[--sp];
      return Value1<Value2?Value1:Value2;
    case Token::MAX:
      Value1 = Values[--sp];
      Value2 = Values[--sp];
      return Value1>Value2?Value1:Value2;
    case Token::RAN:
      randomize();
      return random((int)Values[--sp]);
    case Token::USR: return EvalFunction(aToken.Name);
  }
  return 0;
}

#pragma argsused
double *Parser::GetVarRef(const char * name)
{
 throw XParser(XParser::UVARIABLE);
}

double Parser::PopValue()
{ return Values[--sp]; }

void Parser::SetExpression(const char *aEquation)
{
  if (TheEquation) delete [] TheEquation;
  TheEquation=new char [strlen(aEquation)+1];
  strcpy(TheEquation,aEquation);
}

const char Parser::Decider[10][15]=
/************************************************************/
/*    *      [  +  -  *  /  ^ +-  NG ( f(  ,  )  y  4  ?    */
/************************************************************/
/*  [ */  {{ 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 6, 8, 5},  //
/*  + */   { 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 6, 8, 5},  //
/*  - */   { 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 6, 8, 5},  //
/*  * */   { 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 6, 8, 5},  //
/*  / */   { 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 6, 8, 5},  //
/*  ^ */   { 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 6, 8, 5},  //
/* +- */   { 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 6, 8, 5},  //
/* NG */   { 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 6, 8, 5},  //
/*  ( */   { 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 3, 6, 8, 5},  //
/* f( */   { 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 7, 6, 8, 5}}; //
/************************************************************/

void Parser::Stop()
{ if (Values) delete [] Values; Values=0; }

void Parser::Start()
{
  Values = new double[Tokens.Count()+BASEPOINTER];
  Stack<Token> Temp;
  Token CurrToken,PrevToken;
  Position=TheEquation;
  Temp.Push(PrevToken=Token(Token::MRK));
  CurrToken=NextToken(PrevToken);
  while(1)
    switch(Decider[Temp.Top().Type][CurrToken.Type])
    {
    case 1: // Huidig token naar de Temp-stapel
      Temp.Push(CurrToken);
      CurrToken=NextToken(CurrToken);
      break;
    case 2: // Token bovenaan de Temp-stapel op de RPN-Lijst plaatsen
      PrevToken=Temp.Pop();
      Tokens.Add(PrevToken);
      break;
    case 3: // Token bovenaan de Temp-stapel liquideren
      Temp.Pop();
      CurrToken=NextToken(CurrToken);
      break;
    case 4: // Einde van de pre-evaluatie
      Temp.Pop();
      if (Temp.Count()==0) return;
      throw XParser(XParser::SYNTAX);
    case 5: // Fout opgetreden in de schrijfwijze
      throw XParser(XParser::SYNTAX);
    case 6: // Variabele op de RPN-Lijst plaatsen
      Tokens.Add(CurrToken);
      CurrToken=NextToken(CurrToken);
      break;
    case 7: // Functie op de RPN-Lijst plaatsen
      PrevToken=Temp.Pop();
      Tokens.Add(PrevToken);
      CurrToken=NextToken(PrevToken);
      break;
    case 8: // Token op de RPN-Lijst plaatsen
      Tokens.Add(CurrToken);
      CurrToken=NextToken(CurrToken);
      break;
    case 9: // Volgend token nemen
      CurrToken=NextToken(CurrToken);
      break;
    }
}

double Parser::Evaluate()
{
  sp=BASEPOINTER;
  Token token;
  ListIterator<Token> iter(Tokens);
  while (iter)
  {
    switch(iter.Current().Type)
    {
    case Token::ADD: Values[sp-2]=Values[sp-2]+Values[sp-1]; sp--; break;
    case Token::SUB: Values[sp-2]=Values[sp-2]-Values[sp-1]; sp--; break;
    case Token::MUL: Values[sp-2]=Values[sp-2]*Values[sp-1]; sp--; break;
    case Token::DIV: Values[sp-2]=Values[sp-2]/Values[sp-1]; sp--; break;
    case Token::POW: Values[sp-2]=pow(Values[sp-2],Values[sp-1]); sp--; break;
    case Token::MNS: Values[sp-1]=-Values[sp-1]; break;
    case Token::NOP: break;
    case Token::FNC: Values[sp++]=EvalFunction(iter.Current().Func); break;
    case Token::VAR: Values[sp++]=*iter.Current().ValueRef; break;
    case Token::CON: Values[sp++]=iter.Current().Value; break;
    }
    if (sp<BASEPOINTER) throw XParser(XParser::SYNTAX);
    iter++;
  }
  sp--;
  if (sp!=BASEPOINTER) throw XParser(XParser::SYNTAX);
  return Values[sp];
}


