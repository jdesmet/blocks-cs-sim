#ifndef __JO__LIST__
#define __JO__LIST__
//#include <classlib\objstrm.h>

template <class T>
class ListElement
{
public:
  ListElement *next;
  T element;
  ListElement(const T& el):element(el),next(0){};
};

template <class T> class ListIterator;

template <class T>
class List
{
protected:
  ListElement<T> *head,*tail;
  int count;
public:
  List():head(0),tail(0),count(0){};
//  List(istream& in);
//  List(ipstream& in);
  List(const List<T>& aList);
  ~List();

  void Delete(const T &element);
  void Flush();
  T& First() { return head->element; }
  void Add(const T& element);
  int Count() const { return count; }

////  friend ostream& operator << (ostream& out,const List& list);
////  friend istream& operator >> (istream& in,List& list);
//  friend opstream& operator << (opstream& out,const List<T>& list);
//  friend ipstream& operator >> (ipstream& in,List<T>& list);

  friend class ListIterator<T>;
};

template <class T>
class ListIterator
{
protected:
  ListElement<T> *curr;
  const List<T> &list;
public:
  ListIterator(const List<T>& aList):curr(aList.head),list(aList){};
  void Reset() {curr=list.head; };
  T& Current();
  T& operator ++ (int);
  operator int ();
};

//ostream& operator << (ostream& out,const ListElement& listelement)
//friend istream& operator >> (istream& in,ListElement& listelement)

/*
template <class T>
opstream& operator << (opstream& out,const ListElement<T>& listelement)
{
  return out << listelement.element;
}

template <class T>
ipstream& operator >> (ipstream& in,ListElement<T>& listelement)
{
  return in >> listelement.element;
}
*/

template <class T>
List<T>::~List()
{
  ListElement<T> *curr;
  while ((curr=head)!=0)
  {
    head=curr->next;
    delete curr;
  }
}

template <class T>
List<T>::List(const List<T> &aList):head(0),tail(0),count(0)
{
  ListElement<T> *curr=aList.head;
  while (curr)
  {
    Add(curr->element);
    curr=curr->next;
  }
}

/*
template <class T>
List<T>::List(ipstream& in):head(0),tail(0),count(0)
{
  int i;
  in >> i;
  while (i--) Add(T(in));
} */

template <class T>
void List<T>::Delete(const T &element)
{
  ListElement<T> *curr,*prev;

  if (count==0) return;
  curr=prev=head;
  curr=head->next;
  do
  {
    if (curr->element==element)
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
void List<T>::Flush()
{
  ListElement<T> *curr;
  while ((curr=head)!=0)
  {
    head=curr->next;
    delete curr;
  }
  head=0;
  count=0;
}

template <class T> inline
void List<T>::Add(const T &element)
{
  if (head)
  {
    tail->next=new ListElement<T>(element);
    tail=tail->next;
  }
  else tail=head=new ListElement<T>(element);
  count++;
}

//  friend ostream& operator << (ostream& out,const List& list);
//  friend istream& operator >> (istream& in,List& list);
/*
template <class T>
opstream& operator << (opstream& out,const List<T>& list)
{
  ListIterator<T> Iter(list);
  out << list.count;
  while (Iter) out << Iter++;
  return out;
}

template <class T>
ipstream& operator >> (ipstream& in,List<T>& list)
{
  list.Flush();
  int i;
  in >> i;
  do list.Add(T(in));
  while (i--);
  return in;
}    */

template <class T>
T& ListIterator<T>::Current()
{
  return curr->element;
}

template <class T>
T& ListIterator<T>::operator ++ (int)
{
  ListElement<T> *listelement=curr;
  curr=curr->next;
  return listelement->element;
}

template <class T>
ListIterator<T>::operator int ()
{
  return curr!=0;
}

#endif //__JO__LIST__

