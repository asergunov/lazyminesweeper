#pragma once

#include <map>
#include <cassert>

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
    index_set_type exploded;    ///< the set of bombs exploded
    size_t totalBombCount;
    bool loose = false;
    index_type _loose_index;

    void setOpened(const index_type& index, const neighbour_count_type& bombsNearCount) {
        assert(_openedItems.find(index) == _openedItems.end());
        _openedItems.insert({index, bombsNearCount});
    }

    void setLoose(const index_type& index) {
        _loose_index = index;
        loose = true;
    }

    bool isGameOver() const {
        return loose;
    }

    const index_type looseIndex() const {
        return _loose_index;
    }

    const opened_items_type& openedItems() const {
        return _openedItems;
    }

    /**
     * @brief check if bomb exploded in cell
     * @param index of cell
     * @return true if exploded
     */
    bool isExploded(const index_type& index) const {
        return exploded.find(index) != exploded.end();
    }

    /**
     * @brief mark cell as exploded
     * @param index
     * @param exploded
     */
    void setExploded(const index_type& index, bool exploded = true) {
        if(exploded) {
            this->exploded.insert(index);
        } else {
            this->exploded.erase(index);
        }
    }

    bool isOpened(const index_type& index) const {
        return _openedItems.find(index) != _openedItems.end();
    }

    bool hasFlag(const index_type& index) const {
        return flags.find(index) != flags.end();
    }

    bool setFlag(const index_type& index) {
        return flags.insert(index).second;
    }

    index_set_type unknownCells(const topology_type& topology) const {
        index_set_type result;
        for(const auto& cell : topology) {
            if(!isOpened(cell))
                result.insert(cell);
        }
        return result;
    }
};

}}}

