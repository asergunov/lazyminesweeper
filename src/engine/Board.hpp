#pragma once

#include "PlayerBoardData.hpp"
#include "PrivateBoardData.hpp"

#include <algorithm>

namespace minesweeper {
namespace engine {
namespace solver {

template<typename T>
struct Turn {
    typedef T topology_type;
    using index_type = typename topology_type::index_type;

    /**
     * @brief apply turn to player data
     */
    virtual void redo(const PrivateBoardData<T>&, PlayerBoardData<T>&)=0;
};

template<typename T>
struct GameEngine {
    typedef T topology_type;
    using index_type = typename topology_type::index_type;


    void openField(const index_type& index) {
        _privateData.openField(index, _playerData, _topology);
    }

private:
    topology_type _topology;
    PlayerBoardData<T> _playerData;
    PrivateBoardData<T> _privateData;
};


}}}
