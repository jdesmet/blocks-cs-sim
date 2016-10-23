#ifndef __JO__ILIST__
#define __JO__ILIST__
#include <classlib\objstrm.h>

template <class T>
class IListElement
{
public:
  IListElement *next;
  T *element;
  IListElement(T* el):element(el),next(0){};
  ~IListElement(){ delete element; };
};

template <class T> class IListIterator;

template <class T>
class IList
{
protected:
  IListElement<T> *head,*tail;
  int count;
public:
  IList():head(0),tail(0),count(0){};
  IList(const IList<T>& aIList);
  ~IList();

  void Delete(T *element);
  void Flush();
  T* First() { return head->element; }
  void Add(T *element);
  int Count() const { return count; }

//  friend ostream& operator << (ostream& out,const List& list);
//  friend istream& operator >> (istream& in,List& list);
  friend opstream& operator << (opstream& out,const IList<T>& ilist);
  friend ipstream& operator >> (ipstream& in,IList<T>& ilist);

  friend class IListIterator<T>;
};

template <class T>
class IListIterator
{
protected:
  IListElement<T> *curr;
  const IList<T> &ilist;
public:
  IListIterator(const IList<T>& aIList):curr(aIList.head),ilist(aIList){};
  void Reset() {curr=ilist.head; };
  T* Current();
  T* operator ++ (int);
  operator int ();
};




//ostream& operator << (ostream& out,const ListElement& listelement)
//friend istream& operator >> (istream& in,ListElement& listelement)

template <class T>
opstream& operator << (opstream& out,const IListElement<T>& ilistelement)
{
  return out << *ilistelement.element;
}

template <class T>
ipstream& operator >> (ipstream& in,IListElement<T>& ilistelement)
{
  return in >> *ilistelement.element;
}

template <class T>
IList<T>::~IList()
{
  IListElement<T> *curr;
  while ((curr=head)!=0)
  {
    head=curr->next;
    delete curr;
  }
}

template <class T>
IList<T>::IList(const IList<T> &aIList):head(0),tail(0)
{
  IListElement<T> *curr=aIList.head;
  while (curr)
  {
    Add(curr->element);
    curr=curr->next;
  }
}

template <class T>
void IList<T>::Delete(T *element)
{
  IListElement<T> *curr,*prev;

  if (count==0) return;
  curr=prev=head;
  curr=head->next;
  do
  {
    if (*curr->element==*element)
    {
      prev->next=curr->next;
      delete curr;
      count--;
      return;
    }
    prev=curr;
  } while ((curr=curr->next)!=0);
}

template <class T>
void IList<T>::Flush()
{
  IListElement<T> *curr;
  while ((curr=head)!=0)
  {
    head=curr->next;
    delete curr;
  }
  head=0;
  count=0;
}

template <class T> inline
void IList<T>::Add(T *element)
{
  if (head)
  {
    tail->next=new IListElement<T>(element);
    tail=tail->next;
  }
  else tail=head=new IListElement<T>(element);
  count++;
}

//  friend ostream& operator << (ostream& out,const IList& list);
//  friend istream& operator >> (istream& in,IList& list);

template <class T>
opstream& operator << (opstream& out,const IList<T>& ilist)
{
  IListIterator<T> Iter(ilist);
  out << ilist.count;
  while (Iter) out << Iter++;
  return out;
}

template <class T>
ipstream& operator >> (ipstream& in,IList<T>& ilist)
{
  ilist.Flush();
  int i;
  in >> i;
  do ilist.Add(T(in));
  while (i--);
  return in;
}

template <class T>
T* IListIterator<T>::Current()
{
  return curr->element;
}

template <class T>
T* IListIterator<T>::operator ++ (int)
{
  T *element=curr->element;
  curr=curr->next;
  return element;
}

template <class T>
IListIterator<T>::operator int ()
{
  return curr!=0;
}

#endif //__JO__ILIST__

