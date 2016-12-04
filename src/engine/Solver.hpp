#pragma once

#include "Equations.hpp"
#include "PlayerBoardData.hpp"
#include "Specter.hpp"

#include <set>
#include <map>
#include <algorithm>

#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <boost/range.hpp>
#include <boost/multiprecision/cpp_int.hpp>

namespace minesweeper {
namespace engine {
namespace solver {

using namespace std;

template <typename T>
struct Solver
{

    typedef T topology_type;
    typedef PlayerBoardData<topology_type> player_data_type;
    typedef typename topology_type::index_type index_type;
    typedef typename topology_type::neighbour_count_type neighbour_count_type;
    using index_set_type = std::set<index_type>;
    using mapping_type = std::vector<index_set_type>;
    typedef std::pair<Equations, mapping_type> mapped_equations_type;

    typedef Equations::value_type value_type;
    typedef Equations::vector_type value_vector_type;
    typedef boost::multiprecision::cpp_int variation_count_type;
    typedef boost::rational<variation_count_type> probablity_type;

    typedef boost::numeric::ublas::vector<probablity_type> propbablity_vector_type;
    typedef Equations::bound_type bound_type;

    typedef std::map<index_type, probablity_type> index_porapablities;

    typedef std::vector<size_t> gauss_key_columns_type;

    typedef Specter<bound_type, variation_count_type> specter_type;

    static double toDouble(const probablity_type& a) {
        return a.numerator().convert_to<double>()/a.denominator().convert_to<double>();
    }

    boost::numeric::ublas::triangular_matrix<variation_count_type, boost::numeric::ublas::upper> pascal_triangle;

    /**
     * @brief data from previouse iteraqiton to reuse
     */
    struct IntermediateData {
        //map<index_set_type, neighbour_count_type> known_values;
        index_set_type bombs;
        index_set_type clear;

        bool isClear(const index_type& i) const {
            return clear.end() != clear.find(i);
        }

        bool isBomb(const index_type& i) const {
            return bombs.end() != bombs.find(i);
        }

        void merge(const IntermediateData& other) {
            auto merge_sets = [](index_set_type& out, const index_set_type& other) {
                auto i_out = begin(out);
                auto i_other = begin(other);
                const auto end_out = end(out);
                const auto end_other = end(other);

                for(;i_other != end_other; ++i_other) {
                    if(i_out == end_out || out.key_comp()(*i_other, *i_out)) {
                        out.insert(i_out, *i_other);
                    } else if(out.key_comp()(*i_out, *i_other)) {
                        i_out++;
                    }
                }
            };

            merge_sets(bombs, other.bombs);
            merge_sets(clear, other.clear);
        }
    };

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

    //std::map<Equations, variation_count_type, EquationsOrder> cache_number_of_solutions;
    //std::map<Equations, specter_type, EquationsOrder> cache_specter;

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

    mapped_equations_type parseBoard(const topology_type& topology, const player_data_type& data) const {
        IntermediateData Intermediate;
        return parseBoard(topology, data, Intermediate);
    }

    mapped_equations_type parseBoard(const topology_type& topology, const player_data_type& data, IntermediateData& Intermediate) const {
        auto restCells = data.unknownCells(topology);

        std::map<index_type, std::set<index_type> > closed_to_neghbour_opened; ///< index -> neghbour opened items
        const auto& openedItems = data.openedItems();

        struct opened_item_data_type {
            neighbour_count_type bombs_count;
        };
        using opened_item_data_map_type = std::map<index_type, opened_item_data_type>;
        opened_item_data_map_type opened_item_data;

        // scan opened item. filling up neighbours and correccting with Intermediate data
        for(const auto& item : openedItems) {
            auto i_inserted = opened_item_data.insert({
                item.first, {
                    static_cast<neighbour_count_type>(item.second)
                }
            }).first;
            for(const auto& neighbour : topology.neighbours(item.first)) {
                restCells.erase(neighbour);
                if(!data.isOpened(neighbour) && !Intermediate.isClear(neighbour)) {
                    if(Intermediate.isBomb(neighbour)) {
                        --i_inserted->second.bombs_count;
                    } else {
                        closed_to_neghbour_opened[neighbour].insert(item.first);
                    }
                }
            }
        }

        struct group_data {
            set<index_type> group;
            size_t column;
        };

        // find groups out
        using group_type = map<set<index_type>, group_data >;
        group_type opened_items_to_closed_group; ///< opened items -> group indeces
        for(const auto& coloring : closed_to_neghbour_opened) {
            const auto i_inserted = opened_items_to_closed_group.insert({coloring.second, {{}, opened_items_to_closed_group.size()}}).first;
            i_inserted->second.group.insert(coloring.first);
        }

        map<std::pair<set<size_t>,neighbour_count_type>, size_t> rows;
        for(const auto& opened_item : opened_item_data ) {
            const auto& opened_item_index = opened_item.first;
            auto opened_item_bombs_count = opened_item.second.bombs_count;

            set<size_t> envolved_columns;
            for(const auto& item: opened_items_to_closed_group) {
                const auto& opened_items = item.first;
                const auto& group_data = item.second;

                if(opened_items.find(opened_item_index) == opened_items.end())
                    continue;

                envolved_columns.insert(group_data.column);
            }

            if(envolved_columns.empty())
                continue;

            rows.insert({{move(envolved_columns), move(opened_item_bombs_count)}, rows.size()});
        }

        mapping_type mapping(opened_items_to_closed_group.size());
        Equations equations(rows.size()+1, opened_items_to_closed_group.size()+1);

        for(const auto& row_info: rows) {
            equations._b(row_info.second) = row_info.first.second;
            for(const auto& column : row_info.first.first) {
                equations._A(row_info.second, column)=1;
            }
        }

        for(const auto& group_data_item : opened_items_to_closed_group) {
            mapping[group_data_item.second.column]=group_data_item.second.group;
            equations._bound(group_data_item.second.column) = group_data_item.second.group.size();
        }

        // fill up the total bombs column
        std::fill_n((equations._A.begin1()+rows.size()).begin(), opened_items_to_closed_group.size()+1, 1);
        equations._b(rows.size()) = data.totalBombCount;
        equations._bound(opened_items_to_closed_group.size()) = restCells.size();
        mapping.push_back(restCells); // empty set to indicate the rest


        return {equations, mapping};
    }

    Equations orthogonalize(const Equations& equations) const {
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

    /**
     * @brief gauss_inplace
     * @param equations
     * @return key cells
     *
     * perfoms gaussian transform of equation system
     *
     * equations becomes upper triangilar
     * key cells is cells with value of 1. All other cells key columns are 0.
     * Every nonzero row has only one key cell
     *
     * @example
     * 1 x 0 x x 0 x | x
     * 0 0 1 x x 0 x | x
     * 0 0 0 0 0 1 x | x
     * Key columns are
     * 0,  2,    5
     * Free columns are
     *   1,  3,4,  6
     */
    std::pair<Equations, gauss_key_columns_type> gauss(Equations&& equations) const {
        using namespace boost::numeric::ublas;

        gauss_key_columns_type key_columns;

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
                key_columns.emplace_back(colA); // put row, column key pair

                for(size_t irow = 0; irow < A.size1(); ++irow) {
                    if(irow == colU)
                        continue;

                    auto rowA = row(A, irow);
                    equations.addRow(irow, colU, -rowA[colA]);
                }
                ++colU;
            }
        }
        return {equations, key_columns};
    }

    std::pair<Equations, gauss_key_columns_type> gauss(const Equations& equations) const {
        using namespace boost::numeric::ublas;

        auto result = gauss(std::move(Equations(equations)));
        return {Equations(subrange(result.first._A, 0, result.second.size(), 0, result.first._A.size2()),
                          subrange(result.first._b, 0, result.second.size()),
                          result.first._bound),
                    result.second};
    }

    std::vector<mapped_equations_type> decompose(mapped_equations_type&& e) const {
        auto g = gauss(std::move(e.first));
        return decompose(g.first, g.second, e.second);
    }

    std::vector<std::pair<Equations, std::vector<size_t>>> decompose(Equations equations) const {
        auto g = gauss(std::move(equations));
        return decompose(g.first, g.second);
    }

    std::vector<std::pair<Equations, std::vector<size_t>>> decompose(const Equations& equations, const gauss_key_columns_type& keys) const {
        std::vector<std::pair<Equations, std::vector<size_t>>> result;


        auto generateSet = [](size_t begin, size_t end) {
            std::set<size_t> result;
            for(size_t i = begin; i < end; ++i)
                result.insert(result.end(), i);
            return  result;
        };

        std::set<size_t>
                columns = generateSet(0, equations.variables_count()),
                rows = generateSet(0, keys.size());

        while (!columns.empty()) {
            std::set<size_t> connectedRows, connectedColumns;

            // take a first column
            std::set<size_t> columnsToCheck(columns.begin(), std::next(columns.begin()));
            while (!columnsToCheck.empty()) {
                // find dependent rows
                std::set<size_t> rowsToCheck;
                for(const auto& columnIndex : columnsToCheck) {
                    auto found = columns.find(columnIndex);
                    if(found==columns.end())
                        continue; // already processed

                    connectedColumns.insert(*found);
                    columns.erase(found);

                    const auto& colA = column(equations._A, columnIndex);
                    for(const auto& rowIndex : rows) {
                        if(colA(rowIndex) != 0)
                            rowsToCheck.insert(rowIndex);
                    }
                }
                columnsToCheck.clear();

                // find dependent columns
                for(const auto& rowIndex : rowsToCheck) {
                    auto found = rows.find(rowIndex);
                    if(found==rows.end())
                        continue; // already processed

                    connectedRows.insert(*found);
                    rows.erase(found);

                    const auto& rowA = row(equations._A, rowIndex);
                    for(const auto& columnIndex : columns) {
                        if(rowA(columnIndex) != 0)
                            columnsToCheck.insert(columnIndex);
                    }
                }
            }

            if(!connectedRows.empty() && !connectedColumns.empty()) {
                auto e = Equations(connectedRows.size(), connectedColumns.size());
                std::vector<size_t> m; m.reserve(connectedColumns.size());
                size_t c = 0;
                for(const auto& column : connectedColumns) {
                    m.push_back(column);
                    size_t r = 0;
                    for(const auto& row : connectedRows) {
                        e._A(r, c) = equations._A(row, column);
                        e._b(r) = equations._b(row);
                        e._bound(c) = equations._bound(column);
                        ++r;
                    }
                    ++c;
                }
                result.emplace_back(e, m);
            }
        }

        return result;
    }

    std::vector<mapped_equations_type> decompose(const Equations& equations, const gauss_key_columns_type& key_columns, const mapping_type& mapping) const
    {
        std::vector<mapped_equations_type> result;
        for(const auto& item : decompose(equations, key_columns)) {
            mapping_type newMapping;
            for(const auto& m : item.second)
                newMapping.push_back(mapping[m]);

            result.emplace_back(item.first, newMapping);
        }
        return result;
    }

    variation_count_type combinations_count(const bound_type& bombs, const bound_type& bound) const {
        assert(bound >= bombs);
        return pascal_triangle(bombs, bound);
    }

    specter_type specter(const Equations& equations) const {
        return specter_no_cahce(equations);
//        auto i = cache_specter.find(equations);
//        if(i == cache_specter.end()) {
//            i = cache_specter.emplace(equations, specter_no_cahce(equations)).first;
//        }
//        return i->second;
    }

    specter_type specter_no_cahce(const Equations& equations) const {
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

        std::vector<specter_type> independentResults;
        std::vector<std::vector<size_t>> independentMappings;
        for(const auto& independent: decompose(equations)) {
            const auto& equations = independent.first;
            const auto& reindex = independent.second;

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

            independentResults.push_back(result);
            independentMappings.push_back(reindex);
        }

        specter_type result(equations.variables_count());

        const auto end_result = independentResults.end();
        auto i_mapping = independentMappings.begin();
        const auto end_mapping = independentMappings.end();

        for(auto i_result = independentResults.begin(); i_result != end_result; ++i_result, ++i_mapping) {

            variation_count_type mult = 1;
            for(auto j = independentResults.begin(); j != end_result; ++j) {
                if(j!=i_result) {
                    mult*=j->total;
                }
            }

            auto i = i_result->begin();
            for(const auto index : *i_mapping) {
                auto& r = result[index];

                // remove imposible values
                for(auto j = begin(r); j != end(r); ) {
                    if(j->second == 0)
                        j = r.erase(j);
                    else
                        ++j;
                }

                r = *i;
                for(auto& p : r) {
                    p.second*=mult;
                }
                ++i;
            }
        }

        result.total = 1;
        for(auto j = independentResults.begin(); j != end_result; ++j) {
            result.total*=j->total;
        }

        return result;
    }

    variation_count_type number_of_solutions(const Equations& equations) const {
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

    variation_count_type number_of_solutions_caching(const Equations& equations) const {
        return number_of_solutions(equations);
//        auto i = cache_number_of_solutions.find(equations);
//        if(i == cache_number_of_solutions.end()) {
//            i = cache_number_of_solutions.insert({equations, number_of_solutions(equations)}).first;
//        }

//        return i->second;
    }

    propbablity_vector_type probablities(const Equations& equations) const {
        propbablity_vector_type consolidated_result(equations.variables_count());
        for(const auto& independent : decompose(equations))
        {
            const auto& e = independent.first;
            const auto& reindex = independent.second;

            propbablity_vector_type result(equations.variables_count());
            const auto s = specter(e);
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
            result/=s.total;

            {
                auto i_result = result.begin();
                for(const auto& index : reindex) {
                    consolidated_result[index] = *i_result;
                    ++i_result;
                }
            }
        }
        assert(all_of(begin(consolidated_result), end(consolidated_result), [&](const probablity_type& p){
            return p >= 0 && p <= 1;
        }));
        return consolidated_result;
    }

    index_porapablities probablities(const topology_type& topology, const player_data_type& data) {
        IntermediateData Intermediate;
        return probablities(topology, data, Intermediate);
    }

    index_porapablities probablities(const topology_type& topology, const player_data_type& data, IntermediateData& Intermediate) const {
        const auto& e = parseBoard(topology, data, Intermediate);
        const auto& porabablities = probablities(e.first);

        index_porapablities result;

        for(const auto& index : Intermediate.bombs)
            result[index] = 1;

        for(const auto& index : Intermediate.clear)
            result[index] = 0;

        auto i_mapping = e.second.begin();
        auto i_porabablities = porabablities.begin();
        for(; i_porabablities != porabablities.end(); ++i_mapping, ++i_porabablities) {
            for(const auto& index : *i_mapping) {
                result[index] = *i_porabablities;
            }

            if((*i_porabablities) == 1) {
                for(const auto& index : *i_mapping) {
                    Intermediate.bombs.insert(index);
                }
            } else if (*i_porabablities == 0) {
                for(const auto& index : *i_mapping) {
                    Intermediate.clear.insert(index);
                }
            }
        }

        return result;
    }
};

}}}
