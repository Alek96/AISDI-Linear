#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
//
namespace aisdi
{

template <typename Type>
class Vector
{
public:
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using const_pointer = const Type*;
  using const_reference = const Type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  pointer	T;
  int		VectorSize;
  int     VectorSizeMax;
  int     VectorSizeRatio;

public:
  Vector() : T(NULL), VectorSize(0), VectorSizeMax(1), VectorSizeRatio(2)
  {
    T = new value_type[10];
  }

  Vector(std::initializer_list<Type> l) : Vector()
  {
    for (auto it = l.begin(); it != l.end(); ++it)
      append(*it);
  }

  Vector(const Vector& other) : Vector()
  {
    for (auto it = other.begin(); it != other.end(); it++)
      append(*it);
  }

  Vector(Vector&& other) : Vector()
  {
    /*while (!other.isEmpty()) {
      append(*(other.begin()));
      other.erase(other.begin());
    }*/
    pointer	T   = this->T;
    int		VS  = VectorSize;
    int     VSM = VectorSizeMax;

    this->T         = other.T;
    VectorSize      = other.VectorSize;
    VectorSizeMax   = other.VectorSizeMax;

    other.T             = T;
    other.VectorSize    = VS;
    other.VectorSizeMax = VSM;
  }

  ~Vector()
  {
    delete [] T;
  }

  Vector& operator=(const Vector& other)
  {
    while (!isEmpty())
      erase(--end());
    for (auto it = other.begin(); it != other.end(); it++)
      append(*it);
    return *this;
  }

  Vector& operator=(Vector&& other)
  {
    while (!isEmpty())
      erase(--end());
    /*while (!other.isEmpty()) {
      append(*(other.begin()));
      other.erase(other.begin());
    }*/
    pointer	T   = this->T;
    int		VS  = VectorSize;
    int     VSM = VectorSizeMax;

    this->T         = other.T;
    VectorSize      = other.VectorSize;
    VectorSizeMax   = other.VectorSizeMax;

    other.T             = T;
    other.VectorSize    = VS;
    other.VectorSizeMax = VSM;

    return *this;
  }

  bool isEmpty() const
  {
    return VectorSize==0;
  }

  size_type getSize() const
  {
    return VectorSize;
  }


private:
  void resize()
  {
    pointer newT = new Type[VectorSizeMax * VectorSizeRatio];
    for(int i=0; i<VectorSize; i++)
      newT[i] = T[i];
    delete [] T;
    T = newT;
    VectorSizeMax *= VectorSizeRatio;
  }
public:
  void append(const Type& item)
  {
    if( VectorSize+1 == VectorSizeMax)
			resize();
    T[VectorSize++]=item;
  }

  void prepend(const Type& item)
  {
    if( VectorSize+1 == VectorSizeMax)
			resize();
    for(int i=VectorSize; i>0; i--)
    	T[i] = T[i-1];
		T[0] = item;
    VectorSize++;
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    if( VectorSize+1 == VectorSizeMax)
      resize();

    for (int i = VectorSize; i>insertPosition.nr; i--)
      T[i] = T[i - 1];
    T[insertPosition.nr] = item;
    VectorSize++;
  }

  Type popFirst()
  {
    if (isEmpty())
      throw std::logic_error("cant_pop_element-empty_vector");
    value_type Old = T[0];
    erase(begin());
    return Old;
  }

  Type popLast()
  {
    if (isEmpty())
      throw std::logic_error("cant_pop_element-empty_not_exist");
    value_type Old = T[VectorSize-1];
    erase(--end());
    return Old;
  }

  void erase(const const_iterator& possition)
  {
    if (possition.vec != this || possition == end())
      throw std::out_of_range("cant_erase_object");
    for (int i = possition.nr ; i<VectorSize; i++)
      T[i] = T[i+1];
    VectorSize--;
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
    int range = lastExcluded.nr - firstIncluded.nr;
    if(range<0)
      throw std::out_of_range("cant_erase_object");

    for (int i = firstIncluded.nr; i + range < VectorSize; i++)
      T[i] = T[i + range];
    VectorSize-= range;
  }

  iterator begin()
  {
    iterator it = iterator(this, 0);
    return it;
  }

  iterator end()
  {
    iterator it = iterator(this, VectorSize);
    return it;
  }

  const_iterator cbegin() const
  {
    const_iterator it = const_iterator(this, 0);
    return it;
  }

  const_iterator cend() const
  {
    const_iterator it = const_iterator(this, VectorSize);
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
class Vector<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;

private:
  const Vector<value_type>* vec;
  int nr;
  friend void Vector<Type>::erase(const const_iterator&);
  friend void Vector<Type>::erase(const const_iterator&, const const_iterator&);
  friend void Vector<Type>::insert(const const_iterator&, const Type&);

public:
  explicit ConstIterator(const Vector<value_type>* v=nullptr, int n=0) : vec(v), nr(n)
  {}

  reference operator*() const
  {
    if (nr == vec->VectorSize)
      throw std::out_of_range("range_check");
    return vec->T[nr];
  }

  ConstIterator& operator++()
  {
    if (nr == vec->VectorSize)
      throw std::out_of_range("range_check");
    else
      nr++;
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
    if (nr == 0)
      throw std::out_of_range("range_check");
    else
      nr--;
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
    /*auto result = *this;
    if (d >= 0)
      for (difference_type i = 0; i < d; i++)
        result++;
    else
      for (difference_type i = d; i > 0; i--)
        result--;
    return result;*/

    if (d >=0 && nr + d > vec->VectorSize)
      throw std::out_of_range("range_check");
    else
      nr+=d;
    return *this;
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
    return vec == other.vec && nr == other.nr;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(vec == other.vec && nr == other.nr);
  }
};

template <typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;


  explicit Iterator(Vector<value_type>* v = nullptr, int n = 0) : ConstIterator(v,n)
  {}

  Iterator(const ConstIterator& other) : ConstIterator(other)
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

#endif // AISDI_LINEAR_VECTOR_H
