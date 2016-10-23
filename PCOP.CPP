#include <string.h>
#include "propties.h"

const char *Property::TypeNames[]={"undefined","short","long","real","string"};
const char *XProperty::ErrorNames[]={
 "Niet gedefinieerde property-naam",
 "Verkeerde type-conversie",
 "Poging om een property dubbel te definiëren"
};

//Property::Property(int handle,istream& in)

Property::Property(ipstream& in)
{
  int type;

  Name=in.readString();
  in >> type;
  Type=(Types)type;
  switch(Type)
  {
    case PTshort: in >> Pshort; break;
    case PTlong:  in >> Plong;  break;
    case PTreal:  in >> Preal;  break;
    case PTstr: Pstr=in.readString(); break;
    default: break;
  }
  in >> Mode;
}

Property::Property(const char *name)
:Type(PTundefined),Mode(0)
{
  Name=new char [strlen(name)+1];
  strcpy(Name,name);
}

Property::Property(const char *name,short pshort)
:Pshort(pshort),Type(PTshort),Mode(0)
{
  Name=new char [strlen(name)+1];
  strcpy(Name,name);
}

Property::Property(const char *name,long plong)
:Plong(plong),Type(PTlong),Mode(0)
{
  Name=new char [strlen(name)+1];
  strcpy(Name,name);
}

Property::Property(const char *name,real preal)
:Preal(preal),Type(PTreal),Mode(0)
{
  Name=new char [strlen(name)+1];
  strcpy(Name,name);
  Type=PTreal;
}

Property::Property(const char *name,const char *pstr)
:Type(PTstr),Mode(0)
{
  Name=new char [strlen(name)+1];
  strcpy(Name,name);
  Pstr=new char [strlen(pstr)+1];
  strcpy(Pstr,pstr);
}

Property::Property(const Property& prop)
:Type(prop.Type),Mode(prop.Mode)
{
  Name=new char [strlen(prop.Name)+1];
  strcpy(Name,prop.Name);
  switch(Type)
  {
    case PTshort: Pshort=prop.Pshort; break;
    case PTlong:  Plong =prop.Plong;  break;
    case PTreal:  Preal =prop.Preal;  break;
    case PTstr:
      Pstr=new char [strlen(prop.Pstr)+1];
      strcpy(Pstr,prop.Pstr);
      break;
    default: break;
  }
}

Property& Property::operator = (const Property& prop)
{
  Mode=prop.Mode;
  Type=prop.Type;
  if (Type==PTstr) delete Pstr;
  delete Name;
  Name=new char [strlen(prop.Name)+1];
  strcpy(Name,prop.Name);
  switch(Type)
  {
    case PTshort: Pshort=prop.Pshort; break;
    case PTlong:  Plong =prop.Plong;  break;
    case PTreal:  Preal =prop.Preal;  break;
    case PTstr:
      Pstr=new char [strlen(prop.Pstr)+1];
      strcpy(Pstr,prop.Pstr);
      break;
    default: break;
  }
  return *this;
}

int Property::operator == (const Property& prop) const
{
  return strcmp(Name,prop.Name)!=0;
}

Property::~Property()
{
  if (Type==PTstr) delete Pstr;
  delete Name;
}

long Property::GetMode() const
{
  return Mode;
}

Property::Types Property::GetType() const
{
  return Type;
}

const char *Property::GetName() const
{
  return Name;
}

void Property::SetMode(long mode)
{
  Mode=mode;
}

int Property::operator == (const char *name) const
{
  return strcmp(name,Name)==0;
}

int Property::operator == (Types type) const
{
  return Type==type;
}

Property::operator short () const
{
  if (Type!=PTshort) throw XProperty(XProperty::XPType);
  return Pshort;
}

Property::operator short * ()
{
  if (Type!=PTshort) throw XProperty(XProperty::XPType);
  return &Pshort;
}

Property& Property::operator = (int pshort)
{
  if (Type==PTundefined) Type=PTshort;
  else if (Type!=PTshort && Type!=PTlong && Type!=PTreal)
    throw XProperty(XProperty::XPType);
  if (Type==PTshort) Pshort=pshort;
  else if (Type==PTlong) Plong=pshort;
  else Preal=pshort;
  return *this;
}

Property::operator long () const
{
  if (Type!=PTshort && Type!=PTlong) throw XProperty(XProperty::XPType);
  if (Type==PTlong) return Plong;
  return Pshort;
}

Property::operator long * ()
{
  if (Type!=PTlong) throw XProperty(XProperty::XPType);
  return &Plong;
}

Property& Property::operator = (long plong)
{
  if (Type==PTundefined) Type=PTlong;
  else if (Type!=PTlong && Type!=PTreal) throw XProperty(XProperty::XPType);
  if (Type==PTreal) Preal=plong;
  else Plong=plong;
  return *this;
}

Property::operator real () const
{
  if (Type!=PTshort && Type!=PTlong && Type!=PTreal)
    throw XProperty(XProperty::XPType);
  if (Type==PTlong) return Plong;
  if (Type==PTshort) return Pshort;
  return Preal;
}

Property::operator real * ()
{
  if (Type!=PTreal) throw XProperty(XProperty::XPType);
  return &Preal;
}

Property& Property::operator = (real preal)
{
  if (Type==PTundefined) Type=PTreal;
  else if (Type!=PTreal) throw XProperty(XProperty::XPType);
  Preal=preal;
  return *this;
}

Property::operator char * () const
{
  if (Type!=PTstr) throw XProperty(XProperty::XPType);
  return Pstr;
}

Property& Property::operator = (const char *pstr)
{
  if (Type==PTundefined) Type=PTstr;
  else if (Type==PTstr) delete Pstr;
  else throw XProperty(XProperty::XPType);

  Pstr=new char [strlen(pstr)+1];
  strcpy(Pstr,pstr);

  return *this;
}

ostream& operator << (ostream& out,const Property& prop)
{
  out << prop.Name << " = ";
  switch(prop.Type)
  {
    case Property::PTshort: out << prop.Pshort; break;
    case Property::PTlong:  out << prop.Plong;  break;
    case Property::PTreal:  out << prop.Preal;  break;
    case Property::PTstr:   out << prop.Pstr;   break;
    default: break;
  }
  out << endl;

  return out;
}
//istream& operator >> (istream& in,Property& prop);
opstream& operator << (opstream& out,const Property& prop)
{
  out.writeString(prop.Name);
  out << prop.Type;
  switch(prop.Type)
  {
    case Property::PTshort: out << prop.Pshort; break;
    case Property::PTlong:  out << prop.Plong;  break;
    case Property::PTreal:  out << prop.Preal;  break;
    case Property::PTstr: out.writeString(prop.Pstr); break;
    default: break;
  }
  out << prop.Mode;
  return out;
}

ipstream& operator >> (ipstream& in,Property& prop)
{
  delete prop.Name;
  int i;

  prop.Name=in.readString();
  in >> i;
  prop.Type=(Property::Types)i;
  switch(prop.Type)
  {
    case Property::PTshort: in >> prop.Pshort; break;
    case Property::PTlong:  in >> prop.Plong;  break;
    case Property::PTreal:  in >> prop.Preal;  break;
    case Property::PTstr:
      delete prop.Pstr;
      prop.Pstr=in.readString();
      break;
    default: break;
  }
  in >> prop.Mode;
  return in;
}

Properties::Properties():NextMode(1L){};

//Properties(istream& in);

Properties::Properties(ipstream& in)
//:properties(in)
{
  int i;
  in >> i;
  while (i--) properties.Add(Property(in));

  in >> NextMode;
}

void Properties::Add(const char *Name)
{
  ListIterator<Property> Iterator(properties);

  while (Iterator)
    if ((Iterator++)==Name) throw XProperty(XProperty::XPDuplicate);

  properties.Add(Property(Name));
}

int Properties::NumberOfProperties()
{
  return properties.Count();
}

long Properties::CreateMode()
{
  long mode=NextMode;
  NextMode<<=1;
  return mode;
}

Property& Properties::operator [] (const char *Name)
{
  ListIterator<Property> Iterator(properties);

  while (Iterator)
  {
    if (Iterator.Current()==Name) return Iterator.Current();
    Iterator++;
  }

  Add(Name);

  return (*this)[Name];
}

Property& Properties::operator [] (const char *Name) const
{
  ListIterator<Property> iter(properties);

  while (iter)
  {
    if (iter.Current()==Name) return iter.Current();
    iter++;
  }

  throw XProperty(XProperty::XPUndefined);
}

//friend ostream& operator << (ostream& out,Properties& props);
//friend istream& operator >> (istream& in,Properties& props);
opstream& operator << (opstream& out,Properties& props)
{
  out << props.properties.Count();
  ListIterator<Property> iter(props.properties);
  while (iter) out << (iter++);

//  out << props.properties;
  out << props.NextMode;
  return out;
}

ipstream& operator >> (ipstream& in,Properties& props)
{
  int i;
  in >> i;
  while (i--) props.properties.Add(Property(in));
//  in >> props.properties;
  in >> props.NextMode;
  return in;
}

