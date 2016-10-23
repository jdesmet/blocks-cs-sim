#ifndef __PROPERTIES__
#define __PROPERTIES__
#include "list.h"
#include "real.h"
#include <classlib\objstrm.h>
#include <iostream.h>

// streaming moet nog aangepast worden om te streamen met
// ASCI data
class XProperty:xmsg
{                          
public:
  enum ErrorType {XPUndefined,XPType,XPDuplicate};
  static const char *ErrorNames[];
public:
  XProperty(const ErrorType Type):xmsg(ErrorNames[Type]){};
  XProperty(const XProperty &oXProperty):xmsg((xmsg)oXProperty){};
};

class Property
{
public:
  char *Name;
  long Mode;
  enum Types {PTundefined,PTshort,PTlong,PTreal,PTstr,
              PTshortArr,PTlongArr,PTrealArr,PTstrArr} Type;
  static const char *TypeNames[];
  union{
    short *rPshort;
    long  *rPlong;
    real  *rPreal;
    char  *Pstr;
    char **rPstr;
  };

//  Property(int handle,istream& in);
  Property(ipstream& in);
  Property(const char *name);
  Property(const char *name,short pshort);
  Property(const char *name,long  plong );
  Property(const char *name,real  preal );
  Property(const char *name,const char  *pstr);

  Property(const char *name,const short *rpshort, size=0);
  Property(const char *name,const long  *rplong , size=0);
  Property(const char *name,const real  *rpreal , size=0);
  Property(const char *name,const char **rpstr  , size=0);
  Property(const Property& prop);

  ~Property();

  Property& operator =  (const Property& prop);

  long GetMode() const;
  Types GetType() const;
  const char *GetName() const;

  void SetMode(long mode);

  int operator == (const char *name) const;
  int operator == (Types type) const;
  int operator == (const Property& prop) const;

  operator short * ();
  operator long  * ();
  operator real  * ();
  operator short   () const;
  operator long    () const;
  operator real    () const;
  operator char  * () const;
  operator char  **() const;

  Property& operator = (int pshort);
  Property& operator = (long plong);
  Property& operator = (real preal);
  Property& operator = (const char *pstr);

  Property& operator = (const int  *rpshort);
  Property& operator = (const long *rplong);
  Property& operator = (const real *rpreal);
  Property& operator = (const char **rpstr);

  friend ostream& operator << (ostream& out,const Property& prop);
//  friend istream& operator >> (istream& in,Property& prop);
  friend opstream& operator << (opstream& out,const Property& prop);
  friend ipstream& operator >> (ipstream& in,Property& prop);
};

class Properties
{
  List<Property> properties;
  int NextHandle;
  long NextMode;
public:
  Properties();
//  Properties(istream& in);
  Properties(ipstream& in);

  void Add(const char *Name);
  int NumberOfProperties();
  long CreateMode();

  Property& operator [] (const char *Name) const;
  Property& operator [] (const char *Name);

  operator List<Property>& () { return properties;}

//  friend ostream& operator << (ostream& out,Properties& props);
//  friend istream& operator >> (istream& in,Properties& props);
  friend opstream& operator << (opstream& out,Properties& props);
  friend ipstream& operator >> (ipstream& in,Properties& props);
};

typedef ListIterator<Property> PropertyIterator;

#endif
