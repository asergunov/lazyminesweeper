#pragma once

#include "PlayerBoardData.hpp"
#include "PrivateBoardData.hpp"
#include "Solver.hpp"

#include <algorithm>
#include <thread>
#include <functional>

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
    using PlayerData = PlayerBoardData<topology_type>;
    using PrivateData = minesweeper::engine::solver::PrivateBoardData<topology_type>;
    using Solver = minesweeper::engine::solver::Solver<topology_type>;
    using IntermediateData = typename Solver::IntermediateData;
    using index_porapablities = typename Solver::index_porapablities;
    using probablity_type = typename Solver::probablity_type;

    GameEngine(size_t w, size_t h, size_t bombs)
        : _topology(w, h)
        , _player_data()
        , _private_data()
        , _solver(_topology)
    {
        _player_data.totalBombCount = bombs;
        _private_data.generate(_topology, bombs);

    }

    bool openField(const index_type& index) {
        if(_private_data.openField(_player_data, index, _topology)) {
            ++_dataVersion;
            return true;
        }
        return false;
    }

    bool isFieldOpened(const index_type& index) const {
        return _player_data.isOpened(index);
    }

    /**
     * @brief starts worker thread
     * @param data
     * @param on_finish function will be called from worker thread to merge data
     * @return thread
     */
    static std::thread start_worker(std::shared_ptr<GameEngine> data,
                                    std::function<void(const std::shared_ptr<GameEngine>& data,
                                                       size_t version,
                                                       const IntermediateData& intermediate,
                                                       const index_porapablities& porapablities)> on_finish) {
        const auto player_data = data->_player_data;
        auto intermediate = data->_intermediate;
        auto version = data->_dataVersion;
        auto scedule = [data, player_data, version, intermediate, on_finish]() mutable {
            auto porapablities = data->_solver.probablities(data->_topology, player_data, intermediate);
            on_finish(data, version, intermediate, porapablities);
        };

        return std::thread(scedule);
    }

    bool merge_solution(size_t version,
                        const IntermediateData& intermediate,
                        const index_porapablities& porapablities) {

        if(this->_porapablitiesVersion < version) {
            this->_porapablities = porapablities;
            this->_porapablitiesVersion = version;
            this->_intermediate.merge(intermediate);
            return true;
        }
        return false;
    }

    const topology_type& topology() const {
        return _topology;
    }

    PlayerData& player() {
        return _player_data;
    }

    const PlayerData& player() const {
        return _player_data;
    }


    const index_porapablities& probablities() const {
        return _porapablities;
    }

    probablity_type porabablity(const index_type& index) const {
        const auto i = _porapablities.find(index);
        if(i == _porapablities.end())
            return {-1, 1};
        return i->second;
    }

    double porabablityDouble(const index_type& index) const {
        const auto i = _porapablities.find(index);
        return i == _porapablities.end()
                ? -1.0
                : Solver::toDouble(i->second);
    }

    void openAllNear(const index_type& index) {
        const auto& opened = _player_data.openedItems();
        const auto i = opened.find(index);

        if(i==opened.end())
            return;

        for(const auto& ni : _topology.neighbours(index)) {
            if(_intermediate.isClear(ni) && !_player_data.isOpened(ni)) {
                openField(ni);
            }
        }
    }

    void makeBestTurn() {
        if(_intermediate.clear.empty()) {
            // TODO: find lowest probablity
        } else {
            auto clear = _intermediate.clear; // make a copy
            for(const auto& index : clear) {
                openField(index);
            }
        }
    }

    bool isPorabablitiesCalculated() const {
         return _dataVersion > _porapablitiesVersion;
    }

    size_t bombRemains() const {
        return _player_data.totalBombCount - _player_data.flags.size();
    }

private:
    topology_type _topology;
    PlayerData _player_data;
    PrivateBoardData<T> _private_data;

    Solver _solver;
    IntermediateData _intermediate;
    index_porapablities _porapablities;

    size_t _porapablitiesVersion = 0;
    size_t _dataVersion = 1;
};


}}}
