#pragma once

#include "Equations.hpp"
#include "PlayerBoardData.hpp"

#include <set>
#include <map>
#include <algorithm>

#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

namespace minesweeper {
namespace engine {
namespace solver {

template <typename T>
struct Solver
{
    typedef T topology_type;
    typedef PlayerBoardData<topology_type> player_data_type;
    typedef typename topology_type::index_type index_type;
    typedef typename topology_type::neighbour_count_type neighbour_count_type;

    typedef std::vector<std::set<index_type>> mapping_type;
    typedef std::pair<Equations, mapping_type> mapped_equations_type;

    typedef Equations::value_type value_type;
    typedef Equations::vector_type value_vector_type;
    typedef uint_least64_t variation_count_type;
    typedef Equations::bound_type bound_type;

    typedef std::map<index_type, value_type> index_porapablities;

    boost::numeric::ublas::triangular_matrix<bound_type, boost::numeric::ublas::upper> pascal_triangle;

    struct specter_type : public std::vector<std::map<bound_type, variation_count_type>> {
        variation_count_type total;

        specter_type(const size_t& s)
            : std::vector<std::map<bound_type, variation_count_type>>(s)
            , total(0)
        {

        }

        static specter_type fromTrivialNone() {
            specter_type r(0);
            r.total = 0;
            return r;
        }

        static specter_type fromTrivialOne() {
            specter_type r(0);
            r.total = 1;
            return r;
        }

        static specter_type fromTrivialValueNone() {
            specter_type r(1);
            r.total = 0;
            return r;
        }

        static specter_type fromTrivialValue(const bound_type& value, const variation_count_type& count) {
            specter_type r(1);
            r.total = count;
            r[0].insert({value, count});
            return r;
        }
    }; // specter[column][value] = variant_count

    struct EquationsOrder {
        bool operator()(const Equations::vector_type& a, const Equations::vector_type& b) const {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
        }

        bool operator()(const Equations::bound_vector_type& a, const Equations::bound_vector_type& b) const {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
        }

        bool operator()(const Equations::matrix_type& a, const Equations::matrix_type& b) const {
            return std::lexicographical_compare(a.data().begin(), a.data().end(), b.data().begin(), b.data().end());
        }

        bool operator()(const Equations& a, const Equations& b) const {
            if(a._A == b._A) {
                if(a._b == b._b) {
                    return this->operator()(a._bound, b._bound);
                }
                return this->operator()(a._b, b._b);
            }

            return this->operator()(a._A, b._A);
        }
    };

    std::map<Equations, variation_count_type, EquationsOrder> cache_number_of_solutions;

    Solver(const size_t& max_group_size)
        :pascal_triangle(max_group_size+1, max_group_size+1) {
        pascal_triangle(0,0)=1;
        for(size_t i = 1; i < pascal_triangle.size1(); ++i) {
            pascal_triangle(0,i)=1;
            pascal_triangle(i,i)=1;
            for(size_t j = 1; j < i; ++j) {
                pascal_triangle(j, i) = pascal_triangle(j, i-1)+pascal_triangle(j-1, i-1);
            }
        }
    }

    Solver(const topology_type& topology)
        : Solver(topology.cellsCount())
    {
    }

    mapped_equations_type parseBoard(const topology_type& topology, const player_data_type& data) {
        struct opened_item_data_type {
            size_t row;
            neighbour_count_type bombs_count;
        };

        auto restCells = data.unknownCells(topology);

        std::map<index_type, opened_item_data_type> opened_item_data;
        std::map<index_type, std::set<index_type> > groups_coloring; ///< index -> neghbour opened items
        std::set<index_type> zeros;
        const auto& openedItems = data.openedItems();

        for(const auto& item : openedItems) {
            if(item.second == 0) {
                zeros.insert(item.first);
            }
        }

        for(const auto& item : openedItems) {
            opened_item_data.insert({item.first, {opened_item_data.size(), static_cast<neighbour_count_type>(item.second)}});
            for(const auto& neighbour : topology.neighbours(item.first)) {
                restCells.erase(neighbour);
                if(!data.isOpened(neighbour) && zeros.find(neighbour) == zeros.end()) {
                    groups_coloring[neighbour].insert(item.first);
                }
            }
        }

        struct group_data {
            std::set<index_type> group;
            size_t column;
        };

        std::map<std::set<index_type>, group_data > groups; ///< opened items -> group indeces
        for(const auto& coloring : groups_coloring) {
            auto result = groups.insert({coloring.second, {{}, groups.size()}});
            result.first->second.group.insert(coloring.first);
        }

        mapping_type mapping(groups.size());
        Equations equations(openedItems.size()+1, groups.size()+1);


        for(const auto& opened_item_info: opened_item_data) {
            equations._b(opened_item_info.second.row) = opened_item_info.second.bombs_count;
        }

        for(const auto& group_data_item : groups) {
            mapping[group_data_item.second.column]=group_data_item.second.group;
            equations._bound(group_data_item.second.column) = group_data_item.second.group.size();
            for(const auto& opened_index : group_data_item.first) {
                equations._A(opened_item_data[opened_index].row, group_data_item.second.column)=1;
            }
        }

        // fill up the total bombs column
        std::fill_n((equations._A.begin1()+openedItems.size()).begin(), groups.size()+1, 1);
        equations._b(openedItems.size()) = data.totalBombCount;
        equations._bound(groups.size()) = restCells.size();
        mapping.push_back(restCells); // empty set to indicate the rest


        return {equations, mapping};
    }

    Equations orthogonalize(const Equations& equations) {
        using namespace boost::numeric::ublas;
        const auto& A = equations._A;

        Equations::matrix_type U(A.size1(), A.size2()); ///< Otrhogonalization matrix
        size_t colU = 0, colA = 0;
        for(; colA < A.size2(); ++colA) {
            const auto columnA = column(A, colA); // basis vector pretendent
            auto columnU = column(U, colU);
            columnU = columnA;
            for(int j = 0; j<colU; ++j) {
                const auto b = column(U, j); // prev basis vector
                columnU -= (inner_prod(columnU, b)/inner_prod(b,b))*b;
            }

            for(const auto &v : columnU) {
                if(v != 0) {
                    ++colU;
                    break;
                }
            }
        }

        const auto reducedU = trans(subrange(U, 0,U.size1(), 0,colU));

        Equations result(prod(reducedU,equations._A), prod(reducedU,equations._b), equations._bound);

        return result;
    }

    size_t gauss_inplace(Equations& equations) {
        using namespace boost::numeric::ublas;

        auto& A = equations._A;
        size_t colU = 0, colA = 0;
        for(; colA < A.size2(); ++colA) {
            const auto columnA = column(A, colA);
            // find non-zero row
            auto i_itemA = columnA.begin()+colU;
            for(;i_itemA != columnA.end(); ++i_itemA) {
                if(*i_itemA != 0)
                    break;
            }

            if(i_itemA != columnA.end()) {
                equations.swapRows(colU, i_itemA.index());
                auto mainRow = row(A, colU);
                const auto k = Equations::matrix_type::value_type(1)/mainRow(colA);
                equations.multRow(colU, k);

                for(size_t irow = 0; irow < A.size1(); ++irow) {
                    if(irow == colU)
                        continue;

                    auto rowA = row(A, irow);
                    equations.addRow(irow, colU, -rowA[colA]);
                }
                ++colU;
            }
        }
        return colU;
    }

    Equations gauss(const Equations& equations) {
        using namespace boost::numeric::ublas;

        Equations e = equations;
        auto colU = gauss_inplace(e);
        return Equations(subrange(e._A, 0, colU, 0, e._A.size2()), subrange(e._b, 0, colU), equations._bound);
    }

    std::vector<mapped_equations_type> decompose(const mapped_equations_type& e) {


        return {};
    }

    variation_count_type combinations_count(const bound_type& bombs, const bound_type& bound) {
        assert(bound >= bombs);
        return pascal_triangle(bombs, bound);
    }


    specter_type specter(const Equations& equations) {
        if(equations.variables_count() == 0) {
            return std::all_of(equations._b.begin(), equations._b.end(), [](const value_type& v){
                return v == 0;
            }) ? specter_type::fromTrivialOne() : specter_type::fromTrivialNone();
        } else if(equations.variables_count() == 1) {
            /**
              * a[i][0]*x=b[i]
              * x<_bound[0]
              */
            auto i_b = equations._b.begin();
            value_type x;
            bool found = false;
            const auto& bound = equations._bound[0];
            for(auto i_a_row = equations._A.begin1(); i_a_row != equations._A.end1(); ++i_a_row, ++i_b) {
                const auto& a = i_a_row[0];
                const auto& b = *i_b;
                if(a == 0) {
                    if(b != 0)
                        return specter_type::fromTrivialValueNone();
                    continue; // nothing to check
                }

                if(!found) {
                    x = b/a;
                    found = true;
                    if(x<0 || x > bound || x.denominator() != 1)
                        return specter_type::fromTrivialValueNone();
                } else if (x != b/a){
                    return specter_type::fromTrivialValueNone();
                }
            }

            return specter_type::fromTrivialValue(x.numerator(), combinations_count(x.numerator(), bound));
        }

        auto minimal_bound = std::min_element(equations._bound.begin(), equations._bound.end());
        assert(minimal_bound != equations._bound.end());

        specter_type result(equations.variables_count());
        result.total = 0;
        for(bound_type b = 0; b<=*minimal_bound; ++b) {
            const auto reduced_specter = specter(equations.reduced({{minimal_bound.index(), b}}));
            const auto c = combinations_count(b, *minimal_bound);
            for(size_t i = 0; i<minimal_bound.index(); ++i) {
                const auto& reduced_result_column = reduced_specter[i];
                auto& result_column = result[i];
                for(const auto& reduced_item : reduced_result_column) {
                    auto i = result_column.insert({reduced_item.first, 0}).first;
                    i->second += c*reduced_item.second;
                }
            }

            {
                auto& result_column = result[minimal_bound.index()];
                result_column.insert({b, c*reduced_specter.total});
            }

            for(size_t i = minimal_bound.index()+1; i<result.size(); ++i) {
                const auto& reduced_result_column = reduced_specter[i-1];
                auto& result_column = result[i];
                for(const auto& reduced_item : reduced_result_column) {
                    auto i = result_column.insert({reduced_item.first, 0}).first;
                    i->second += c*reduced_item.second;
                }
            }

            result.total += reduced_specter.total*c;
        }

        return result;
    }

    variation_count_type number_of_solutions(const Equations& equations) {
        if(equations.variables_count() == 0) {
            return std::all_of(equations._b.begin(), equations._b.end(), [](const value_type& v){
                return v == 0;
            }) ? 1 : 0;
        } else if(equations.variables_count() == 1) {
            /**
              * a[i][0]*x=b[i]
              * x<_bound[0]
              */
            auto i_b = equations._b.begin();
            value_type x;
            bool found = false;
            const auto& bound = equations._bound[0];
            for(auto i_a_row = equations._A.begin1(); i_a_row != equations._A.end1(); ++i_a_row, ++i_b) {
                const auto& a = i_a_row[0];
                const auto& b = *i_b;
                if(a == 0) {
                    if(b != 0)
                        return 0;
                    continue; // nothing to check
                }

                if(!found) {
                    x = b/a;
                    found = true;
                    if(x<0 || x > bound || x.denominator() != 1)
                        return 0;
                } else if (x != b/a){
                    return 0;
                }
            }

            return combinations_count(x.numerator(), bound);
        }

        auto minimal_bound = std::min_element(equations._bound.begin(), equations._bound.end());
        assert(minimal_bound != equations._bound.end());

        variation_count_type result = 0;
        for(bound_type b = 0; b<=*minimal_bound; ++b) {
            result += number_of_solutions_caching(equations.reduced({{minimal_bound.index(), b}}))*combinations_count(b, *minimal_bound);
        }

        return result;
    }

    variation_count_type number_of_solutions_caching(const Equations& equations) {
        auto i = cache_number_of_solutions.find(equations);
        if(i == cache_number_of_solutions.end()) {
            i = cache_number_of_solutions.insert({equations, number_of_solutions(equations)}).first;
        }

        return i->second;
    }

    value_vector_type probablities(const Equations& e) {
        const auto s = specter(e);
        value_vector_type result(e.variables_count());
        auto i_result = result.begin();
        auto i_bound = e._bound.begin();

        for(auto i = s.begin(); i!= s.end(); ++i, ++i_result, ++i_bound) {
            *i_result = 0;
            const auto& bound = *i_bound;
            for(const auto item : *i) {
                if(item.first > 0)
                    *i_result += combinations_count(item.first-1, bound-1)*item.second/combinations_count(item.first, bound);
            }
        }
        return  result/s.total;
    }

    index_porapablities probablities(const topology_type& topology, const player_data_type& data) {
        const auto& e = parseBoard(topology, data);
        const auto& porabablities = probablities(e.first);

        index_porapablities result;

        auto i_mapping = e.second.begin();
        auto i_porabablities = porabablities.begin();
        for(; i_porabablities != porabablities.end(); ++i_mapping, ++i_porabablities) {
            for(const auto& index : *i_mapping)
                result[index] = *i_porabablities;
        }

        return result;
    }
};

}}}
