#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operations.hpp>
#include <boost/rational.hpp>

#include <ostream>

namespace minesweeper {
namespace engine {
namespace solver {

template<typename T>
bool operator==(const boost::numeric::ublas::matrix<T>& m, const boost::numeric::ublas::matrix<T>& n)
{
  bool returnValue =
    (m.size1() == n.size1()) &&
    (m.size2() == n.size2());

  if (returnValue)
  {
    for (unsigned int i = 0; returnValue && i < m.size1(); ++i)
    {
      for (unsigned int j = 0; returnValue && j < m.size2(); ++j)
      {
        returnValue &= m(i,j) == n(i,j);
      }
    }
  }
  return returnValue;
}

template<typename T>
bool operator==(const boost::numeric::ublas::vector<T>& m, const boost::numeric::ublas::vector<T>& n)
{
  bool returnValue = (m.size() == n.size());

  if (returnValue)
  {
    for (unsigned int i = 0; returnValue && i < m.size(); ++i)
    {
        returnValue &= m(i) == n(i);
    }
  }
  return returnValue;
}

template<typename T>
std::ostream& operator<<(std::ostream& o, const boost::numeric::ublas::matrix<T>& m)
{
    o << "{";
    for (unsigned int i = 0; i < m.size1(); ++i)
    {
      o << "{";
      for (unsigned int j = 0; j < m.size2(); ++j)
      {
        o << m(i,j) << ", ";
      }
      o << "}, ";
    }
    o << "}";
  return o;
}

template<typename T>
std::ostream& operator<<(std::ostream& o, const boost::numeric::ublas::vector<T>& m)
{
    o << "{";
    for (unsigned int i = 0; i < m.size(); ++i)
    {
        o << m(i) << ", ";
    }
    o << "}";
  return o;
}

std::ostream& operator<<(std::ostream& o, const int8_t& x)
{
  return o << (int)x;
}

/**
 * @brief The Equations struct
 *
 * Represtnts lineral equations system
 * Ax = b
 * x < bound
 * x in [0, ..., bound]
 */
struct Equations {
    using bound_type = int8_t;
    using value_type = boost::rational<int64_t>;
    using matrix_type = boost::numeric::ublas::matrix<value_type>;
    using vector_type = boost::numeric::ublas::vector<value_type>;
    using bound_vector_type = boost::numeric::ublas::vector<bound_type>;

    Equations(const matrix_type& A, const vector_type& b, const bound_vector_type& bound)
        : _A(A), _b(b), _bound(bound) {
        assert(_A.size1() == _b.size());
        assert(_bound.size() == _A.size2());
    }

    Equations(const size_t row_count, const size_t column_count)
        : _A(row_count, column_count)
        , _b(row_count)
        , _bound(column_count)
    {

    }

    bool operator==(const Equations& arg) const {
        return _A == arg._A && _b == arg._b && _bound == arg._bound;
    }

    matrix_type _A;
    vector_type _b;
    bound_vector_type _bound;

    friend std::ostream& operator<<(std::ostream& o, const Equations& x) {
        return o << "A: " << x._A << ", b: " << x._b << ", bound: " << x._bound;
    }
};


}}}
