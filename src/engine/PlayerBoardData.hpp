#pragma once

#include <map>

namespace minesweeper {
namespace engine {
namespace solver {

template <typename T>
struct PlayerBoardData {
    typedef T topology_type;
    using index_type = typename topology_type::index_type;
    using index_set_type = typename topology_type::index_set_type;
    using neighbour_count_type = typename topology_type::neighbour_count_type;

    using opened_items_type = std::map<index_type, neighbour_count_type>;

    opened_items_type _openedItems;
    index_set_type flags;
    size_t totalBombCount;
    bool loose = false;
    index_type loose_index;

    void setOpened(const index_type& index, const neighbour_count_type& bombsNearCount) {
        assert(_openedItems.find(index) == _openedItems.end());
        _openedItems.insert({index, bombsNearCount});
    }

    void setLoose(const index_type& index) {
        loose_index = index;
        loose = true;
    }

    const opened_items_type& openedItems() const {
        return _openedItems;
    }

    bool isOpened(const index_type& index) const {
        return _openedItems.find(index) != _openedItems.end();
    }
};

}}}

