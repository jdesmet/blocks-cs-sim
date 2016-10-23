#ifndef __JO__STACK__
#define __JO__STACK__

template <class T>
class StackElement
{
public:
  StackElement *next;
  T element;
  StackElement(const T& el):element(el),next(0){};
};

template <class T> class StackIterator;

template <class T>
class Stack
{
protected:
  StackElement<T> *head,*tail;
  int count;
public:
  Stack():head(0),tail(0),count(0){};
  Stack(const Stack<T>& aStack);
  ~Stack();

  void Flush();
  T& Top() { return head->element; }
  T Pop();
  void Push(const T& element);
  int Count() const { return count; }
};

template <class T>
Stack<T>::~Stack()
{
  StackElement<T> *curr;
  while ((curr=head)!=0)
  {
    head=curr->next;
    delete curr;
  }
}

template <class T>
Stack<T>::Stack(const Stack<T> &aStack):head(0),tail(0),count(0)
{
  StackElement<T> *curr=aStack.head;
  while (curr)
  {
    Push(curr->element);
    curr=curr->next;
  }
}

template <class T>
void Stack<T>::Flush()
{
  StackElement<T> *curr;
  while ((curr=head)!=0)
  {
    head=curr->next;
    delete curr;
  }
  head=0;
  count=0;
}

template <class T>
T Stack<T>::Pop()
{
  if (count==0) throw;
  T Temp(head->element);
  StackElement<T> *tmp=head;
  head=head->next;
  delete tmp;
  count--;
  return Temp;
}

template <class T> inline
void Stack<T>::Push(const T &element)
{
  StackElement<T> *temp=head;
  head = new StackElement<T>(element);
  head->next = temp;
  count++;
}

#endif //__JO__STACK__

