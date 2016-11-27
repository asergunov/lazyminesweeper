#pragma once

#include "PlayerBoardData.hpp"

#include <set>
#include <algorithm>

namespace minesweeper {
namespace engine {
namespace solver {

template <typename T>
struct PrivateBoardData {
    typedef T topology_type;
    using index_type = typename topology_type::index_type;
    using index_set_type = typename topology_type::index_set_type;

    index_set_type bombs;

    void openField(PlayerBoardData<T>& playerData, const index_type& index, const topology_type& topology) const {
        if(playerData.isOpened(index))
            return;

        if(playerData._openedItems.empty())
            playerData.totalBombCount = bombs.size();

        if(isBombAt(index)) {
            playerData.setLoose(index);
        } else {
            const auto count = bombsCountIn(topology.neighbours(index));
            playerData.setOpened(index, count);
            if(0 == count) {
                for(const auto i : topology.neighbours(index))
                    openField(playerData, i, topology);
            }
        }
    }

    bool isBombAt(const index_type& index) const {
        return bombs.find(index) != bombs.end();
    }

    size_t bombsCountIn(const index_set_type& indeces) const {
        size_t count = 0;
        index_type index;

        struct count_inserter {
            size_t& _count;
            index_type &_index;

            count_inserter(size_t& count, index_type& index)
            : _count(count), _index(index) {

            }

            count_inserter& operator++() {
                _count++;
            }

            index_type& operator*() {
                return _index;
            }
        };



        std::set_intersection(indeces.begin(), indeces.end(),
                              bombs.begin(), bombs.end(),
                              count_inserter(count, index));
        return count;
    }

    void generate(const topology_type& t, size_t bombs) {
        this->bombs.clear();
        std::set<size_t> bomb_indeces;
        for(size_t i = 0; i<bombs; ++i) {
            auto place = std::rand() % (t.cellsCount() - i);
            for(const auto& index : bomb_indeces) {
                if(place >= index)
                    ++place;
                else
                    break;
            }
            bomb_indeces.insert(place);
        }

        for(const auto& index : bomb_indeces) {
            this->bombs.insert(*(t.begin() + index));
        }
    }
};

}}}


