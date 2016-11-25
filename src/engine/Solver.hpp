#pragma once

#include "Equations.hpp"
#include "PlayerBoardData.hpp"

#include <set>
#include <map>
#include <algorithm>

#include <boost/numeric/ublas/triangular.hpp>

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
    typedef size_t variation_count_type;
    typedef Equations::bound_type bound_type;

    typedef std::map<index_type, value_type> index_porapablities;

    boost::numeric::ublas::triangular_matrix<bound_type, boost::numeric::ublas::upper> pascal_triangle;

    Solver()
        :pascal_triangle(T::max_neighbours+1, T::max_neighbours+1) {
        pascal_triangle(0,0)=1;
        for(size_t i = 1; i < pascal_triangle.size1(); ++i) {
            pascal_triangle(0,i)=1;
            pascal_triangle(i,i)=1;
            for(size_t j = 1; j < i; ++j) {
                pascal_triangle(j, i) = pascal_triangle(j, i-1)+pascal_triangle(j-1, i-1);
            }
        }
    }

    mapped_equations_type parseBoard(const topology_type& topology, const player_data_type& data) {
        struct opened_item_data_type {
            size_t row;
            neighbour_count_type bombs_count;
        };

        std::map<index_type, opened_item_data_type> opened_item_data;
        std::map<index_type, std::set<index_type> > groups_coloring; ///< index -> neghbour opened items
        const auto& openedItems = data.openedItems();
        for(const auto& item : openedItems) {
            opened_item_data.insert({item.first, {opened_item_data.size(), static_cast<neighbour_count_type>(item.second)}});
            for(const auto& neighbour : topology.neighbours(item.first)) {
                if(!data.isOpened(neighbour)) {
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
        Equations equations(openedItems.size(), groups.size());


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

        return {equations, mapping};
    }

    variation_count_type combinations_count(const bound_type& bombs, const bound_type& bound) {
        assert(bound >= bombs);
        return pascal_triangle(bombs, bound);
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
            result += number_of_solutions(equations.reduced({{minimal_bound.index(), b}}))*combinations_count(b, *minimal_bound);
        }

        return result;
    }

    value_vector_type probablities(const Equations& e) {
        value_vector_type result(e.variables_count());
        auto i_result = result.begin();
        for(size_t column = 0; column < e.variables_count(); ++column, ++i_result) {
            *i_result = 0;
            const auto& bound = e._bound(column);
            for(bound_type value = 1; value <= bound; ++value) {
                *i_result += number_of_solutions(e.reduced({{column, value}}))*combinations_count(value-1, bound-1);
            }
        }
        return  result/number_of_solutions(e);
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
