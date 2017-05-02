#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class LinkedList
{
public:
  using difference_type = std::ptrdiff_t;   //odejmowanie pomiedzy wskaznikami
  using size_type = std::size_t;            //
  using value_type = Type;                  //
  using pointer = Type*;                    //
  using reference = Type&;                  //
  using const_pointer = const Type*;        //
  using const_reference = const Type&;      //

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  struct Object
  {
    Type*   Data;
    Object* Next;
    Object* Prev;
    Object() : Data(NULL), Next(NULL), Prev(NULL) {}
    Object(Type d) : Object() { Data = new Type(d); }
    ~Object() { delete Data; }
  };
  Object*	FirstObject;
  Object*	LastObject;
  size_type ListSize;

public:
  LinkedList() : FirstObject(NULL), LastObject(NULL), ListSize(0)
  {
    FirstObject = new Object;
    LastObject  = new Object;
    FirstObject->Next = LastObject;
    LastObject->Prev  = FirstObject;
  }

  LinkedList(std::initializer_list<Type> l) : LinkedList()
  {
    //std::initializer_list<Type>::iterator it; //zle? - potrzeba typenam?
    for (auto it = l.begin(); it != l.end(); ++it)
      append(*it);
  }

  LinkedList(const LinkedList& other) : LinkedList()
  {
    for (auto it = other.begin(); it != other.end(); it++)
      append(*it);
  }

  //friend LinkedList<Type>::LinkedList(LinkedList&&);
  LinkedList(LinkedList&& other) : LinkedList() //kopiuje wskazniki, other el = NULL, ja to musze zrobic
  {
    /*while (!other.isEmpty()) {
      append(*(other.begin()));
      other.erase(other.begin());
    }*/
    Object*	F = FirstObject;
    Object*	L = LastObject;

    FirstObject = other.FirstObject;
    LastObject  = other.LastObject;
    ListSize    = other.ListSize;

    other.FirstObject = F;
    other.LastObject  = L;
    other.ListSize    = 0;
  }

  ~LinkedList()
  {
    while (!isEmpty())
      erase(begin());
    delete FirstObject;
    delete LastObject;
  }

  LinkedList& operator=(const LinkedList& other)
  {
    while (!isEmpty())
      erase(begin());
    for (auto it = other.begin(); it != other.end(); it++)
      append(*it);

    return *this;
  }

  LinkedList& operator=(LinkedList&& other)
  {
    while (!isEmpty())
      erase(begin());
    /*while (!other.isEmpty()) {
      append(*(other.begin()));
      other.erase(other.begin());
    }*/
    Object*	F = FirstObject;
    Object*	L = LastObject;

    FirstObject = other.FirstObject;
    LastObject  = other.LastObject;
    ListSize    = other.ListSize;

    other.FirstObject = F;
    other.LastObject  = L;
    other.ListSize    = 0;

    return *this;
  }

  bool isEmpty() const
  {
    return ListSize==0;
  }

  size_type getSize() const
  {
    return ListSize;
  }

  void append(const Type& item) //dodac na koncu
  {
    LastObject->Data = new Type(item);
    Object* New = new Object;

    New->Prev = LastObject;
    LastObject->Next = New;
    LastObject = New;

    ListSize++;
  }

  void prepend(const Type& item) //dodac na poczatku
  {
    Object* New = new Object(item);

    New->Prev = FirstObject;
    New->Next = FirstObject->Next;

    FirstObject->Next->Prev = New;
    FirstObject->Next = New;

    ListSize++;
  }

  void insert(const const_iterator& insertPosition, const Type& item) //el wstawiony w to miejsce
  {
    if (insertPosition == begin() )
      prepend(item);
    else if (insertPosition == end())
      append(item);
    else {
      Object* New = new Object(item);

      New->Next = insertPosition.it;
      New->Prev = insertPosition.it->Prev;

      insertPosition.it->Prev->Next = New;
      insertPosition.it->Prev = New;
      ListSize++;
    }
  }

  Type popFirst()
  {
    if (isEmpty())
      throw std::logic_error("cant_pop_object");

    Type Data = *begin();
    erase(begin());
    return Data;
  }

  Type popLast()
  {
    if (isEmpty())
      throw std::logic_error("cant_pop_object");

    Type Data = *(--end());
    erase((--end()));
    return Data;
  }

  void erase(const const_iterator& possition)
  {
    if (possition == end())
      throw std::out_of_range("cant_eres_object");
    possition.it->Next->Prev = possition.it->Prev;
    possition.it->Prev->Next = possition.it->Next;
    delete possition.it;
    ListSize--;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    auto it = firstIncluded;
    while (it != lastExcluded) {
      it++;
      erase(it - 1);
    }
  }

  iterator begin()
  {
    iterator it = iterator(FirstObject->Next);
    return it;
  }

  iterator end()
  {
    iterator it = iterator(LastObject);
    return it;
  }

  const_iterator cbegin() const
  {
    const_iterator it = const_iterator(FirstObject->Next);
    return it;
  }

  const_iterator cend() const
  {
    const_iterator it = const_iterator(LastObject);
    return it;
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename Type>
class LinkedList<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename LinkedList::value_type;
  using difference_type = typename LinkedList::difference_type;
  using pointer = typename LinkedList::const_pointer;
  using reference = typename LinkedList::const_reference;

private:
  Object* it;
  friend void LinkedList <Type>::insert(const const_iterator&, const Type&);
  friend void LinkedList <Type>::erase(const const_iterator&);
public:
  explicit ConstIterator(Object* i = nullptr) : it(i)
  {}

  reference operator*() const
  {
    if (it->Data == nullptr)
      throw std::out_of_range("range_check");
    return *(it->Data);
  }

  ConstIterator& operator++()
  {
    if (it->Next == nullptr)
      throw std::out_of_range("range_check");
    it = it->Next;
    return *this;
  }

  ConstIterator operator++(int)
  {
    auto result = *this;
    operator++();
    return result;
  }

  ConstIterator& operator--()
  {
    if (it->Prev->Prev == nullptr)
      throw std::out_of_range("range_check");
    it = it->Prev;
    return *this;
  }

  ConstIterator operator--(int)
  {
    auto result = *this;
    operator--();
    return result;
  }

  ConstIterator operator+(difference_type d) const
  {
    auto result = *this;
    if (d >= 0)
      for (difference_type i = 0; i < d; i++)
        result++;
    else
      for (difference_type i = d; i > 0; i--)
        result--;
    return result;
  }

  ConstIterator operator-(difference_type d) const
  {
    auto result = *this;
    if (d >= 0)
      for (difference_type i = 0; i < d; i++)
        result--;
    else
      for (difference_type i = d; i > 0; i--)
        result++;
    return result;
  }

  bool operator==(const ConstIterator& other) const
  {
    return it == other.it;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return it != other.it;
  }
};

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator //zrobione
{
public:
  using pointer = typename LinkedList::pointer;
  using reference = typename LinkedList::reference;

public:
  explicit Iterator(Object* i = nullptr) : ConstIterator(i)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  Iterator operator+(difference_type d) const
  {
    return ConstIterator::operator+(d);
  }

  Iterator operator-(difference_type d) const
  {
    return ConstIterator::operator-(d);
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif // AISDI_LINEAR_LINKEDLIST_H
