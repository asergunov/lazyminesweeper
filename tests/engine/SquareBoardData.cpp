#include <gtest/gtest.h>

#include <PlayerBoardData.hpp>
#include <PrivateBoardData.hpp>
#include <SquareBoard.hpp>

using namespace minesweeper::engine::solver;
using namespace minesweeper::engine::square_board;

TEST(SuqareBoardDataPrivate, init) {
    PlayerBoardData<Topology> playerData;
}


TEST(SuqareBoardDataPlayer, init) {
    PrivateBoardData<Topology> privateData;
}
