#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operations.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

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

//std::ostream& operator<<(std::ostream& o, const int8_t& x)
//{
//  return o << (int)x;
//}

/**
 * @brief The Equations struct
 *
 * Represtnts lineral equations system
 * Ax = b
 * x < bound
 * x in [0, ..., bound]
 */
struct Equations {
    using bound_type = int64_t;
    using value_type = boost::rational<int64_t>;
    using matrix_type = boost::numeric::ublas::matrix<value_type>;
    using vector_type = boost::numeric::ublas::vector<value_type>;
    using bound_vector_type = boost::numeric::ublas::vector<bound_type>;

    /**
     * @brief The Reduction struct
     * reduction of equation system setting variable to fixed value
     */
    using reduction_type = std::map<size_t, bound_type>;

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

    size_t count() const {
        return _A.size1();
    }

    size_t variables_count() const {
        return _A.size2();
    }

    matrix_type _A;
    vector_type _b;
    bound_vector_type _bound;

    friend std::ostream& operator<<(std::ostream& o, const Equations& x) {
        return o << "A: " << x._A << ", b: " << x._b << ", bound: " << x._bound;
    }


    Equations reduced(const reduction_type& reduction) const {
        Equations result(_A.size1(), _A.size2() - reduction.size());
        result._b = _b;

        auto i_A_out = result._A.begin1();
        const auto end_A_out = result._A.end1();

        auto i_A_in = _A.begin1();
        const auto end_A_in = _A.end1();

        auto i_b_out = result._b.begin();
        auto end_b_out = result._b.end();

        auto i_b_in = _b.begin();
        auto end_b_in = _b.end();

        {
            auto i_bound_in = _bound.begin();
            const auto end_bound_in = _bound.end();

            auto i_bound_out = result._bound.begin();

            auto i_values = reduction.begin();
            const auto end_values = reduction.end();

            for(;i_bound_in != end_bound_in;) {
                if(i_values != end_values && i_values->first == i_bound_in.index()) {
                    ++i_values;
                } else {
                    *i_bound_out = *i_bound_in;
                    ++i_bound_out;
                }
                ++i_bound_in;
            }
        }

        for(;i_A_in != end_A_in; ++i_b_in, ++i_b_out, ++i_A_in) {
            assert(i_b_in != end_b_in);
            assert(i_b_out != end_b_out);

            auto i2_A_in = i_A_in.begin();
            const auto end2_A_in = i_A_in.end();

            auto i2_A_out = i_A_out.begin();
            const auto end2_A_out = i_A_out.end();

            auto i_values = reduction.begin();
            const auto end_values = reduction.end();

            for(;i2_A_in != end2_A_in;) {
                if(i_values != end_values && i_values->first == i2_A_in.index2()) {
                    *i_b_out -= *i2_A_in*i_values->second;
                    ++i_values;
                } else {
                    assert(i2_A_out != end2_A_out);
                    *i2_A_out = *i2_A_in;
                    ++i2_A_out;
                }
                ++i2_A_in;
            }
             ++i_A_out;
        }
        return result;
    }

    void swapRows(size_t i, size_t j) {
        using namespace boost::numeric::ublas;
        auto iRow = row(_A, i);
        auto jRow = row(_A, j);
        std::swap_ranges(iRow.begin(), iRow.end(), jRow.begin());
        std::swap(_b(i), _b(j));
    }

    /**
     * @brief addRow
     * @param changed
     * @param other
     * @param k
     *
     * row[changed] += row[other]*k;
     */
    void addRow(size_t changed, size_t other, const value_type& k) {
        if(k==0)
            return;
        auto changedRow = row(_A, changed);
        const auto otherRow = row(_A, other);

        changedRow += otherRow*k;
        _b(changed) += k*_b(other);
    }

    void multRow(size_t changed, const value_type& k) {
        auto changedRow = row(_A, changed);
        changedRow *= k;
        _b(changed) *= k;
    }
};


}}}
