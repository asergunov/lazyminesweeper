#pragma once

#include "PlayerBoardData.hpp"

namespace minesweeper {
namespace engine {
namespace solver {

template <typename T>
struct PrivateBoardData {
    typedef T topology_type;
    using index_type = typename topology_type::index_type;
    using index_set_type = typename topology_type::index_set_type;

    index_set_type bombs;

    void openField(PlayerBoardData<T>& playerData, const index_type& index, const topology_type& topology) {
        if(isBombAt(index)) {
            playerData.setLoose(index);
        } else {
            playerData.setOpened(index, bombsCountIn(topology.neighbours(index)));
        }
    }
};

}}}


