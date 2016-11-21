#pragma once

#include "Equations.hpp"
#include "PlayerBoardData.hpp"

#include <set>
#include <map>

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

    std::pair<Equations, mapping_type> parseBoard(const topology_type& topology, const player_data_type& data) {
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
};

}}}
