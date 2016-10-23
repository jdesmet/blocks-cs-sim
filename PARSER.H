#include "list.h"

class XParser
{
public:
  enum ErrorType {FORMAT,SYNTAX,UFUNCTION,UVARIABLE,USYMBOL,ARGUMENTS};
private:
  ErrorType Type;
public:
  XParser(const ErrorType type):Type(type){};
  XParser(const XParser &oXParser):Type(oXParser.Type){};
  const char *GetErrorText () const;
};

class Token
{
public:
  enum TokenType {MRK,ADD,SUB,MUL,DIV,POW,MNS,NOP,OPA,FNC,CMA,CPA,VAR,CON,ERR};
  enum FuncType  {ABS,ACS,ASN,ATN,CSH,COS,EXP,LOG,LN ,RND,RT ,P10,SNH,SIN,SQT,
                  SQR,TNH,TAN,TRC,SGN,MIN,MAX,RAN,USR};
//private:
  TokenType Type;
//  union
//  {
    double Value; double *ValueRef;
    /*struct { */FuncType Func; char *Name;/* }; */
//  };
//public:
  Token(TokenType type = NOP);
  Token(FuncType func);
  Token(double val);
  Token(double *valueref);
  Token(const Token &oToken);
  Token(const char *str);
  ~Token();
  Token& operator = (const Token &oToken);
  int operator == (const Token &oToken);
//  friend class Parser;
};

class Parser
{
public:
  Parser();
  Parser(const char *aEquation);
  ~Parser();
  void SetExpression(const char *aEquation);
  double Evaluate();
  void Start();
  void Stop();
private:
// Private data
  static const char Decider[10][15];
  char *TheEquation;
  char *Position;
  double *Values;
  int sp;
  List<Token> Tokens;
// Private functions
  double EvalFunction(const Token &aToken);
  Token NextToken(const Token &prevToken);
protected:
// Protected functions
  double PopValue();
  virtual double *GetVarRef(const char *aName);
  virtual double EvalFunction(const char *aName);
};

