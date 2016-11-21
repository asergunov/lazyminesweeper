#include <gtest/gtest.h>

#include <SquareBoard.hpp>
#include <Solver.hpp>

using namespace minesweeper::engine::solver;
using namespace minesweeper::engine::square_board;

TEST(SqureSolver, simple3x3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);

    Solver<Topology> s;
    const auto& result = s.parseBoard(t, data);

    auto equations = Equations(1,1);
    equations._A(0,0) = 1;
    equations._b(0) = 1;
    equations._bound(0) = 8;

    EXPECT_EQ(equations, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({t.neighbours({1,1})}), result.second);
}

TEST(SqureSolver, simple3x3_3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 3);

    Solver<Topology> s;
    const auto& result = s.parseBoard(t, data);

    auto equations = Equations(1,1);
    equations._A(0,0) = 1;
    equations._b(0) = 3;
    equations._bound(0) = 8;

    EXPECT_EQ(equations, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({t.neighbours({1,1})}), result.second);
}

TEST(SqureSolver, corner3x3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);

    Solver<Topology> s;
    const auto& result = s.parseBoard(t, data);

    auto equations = Equations(1,1);
    equations._A(0,0) = 1;
    equations._b(0) = 1;
    equations._bound(0) = 3;

    EXPECT_EQ(equations, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({t.neighbours({0,0})}), result.second);
}

TEST(SqureSolver, corner4x3_1_1) {
    Topology t(3,4);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);
    data.setOpened({1,2}, 2);

    Solver<Topology> s;
    const auto& result = s.parseBoard(t, data);

    auto equations = Equations(2,3);
    equations._A(0,0) = 1;
    equations._A(0,1) = 1;
    equations._A(1,1) = 1;
    equations._A(1,2) = 1;
    equations._b(0) = 1;
    equations._b(1) = 2;
    equations._bound(0) = 3;
    equations._bound(1) = 4;
    equations._bound(2) = 3;

    EXPECT_EQ(equations, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({{{0,0},{1,0},{2,0}},{{0,1},{0,2},{2,1},{2,2}},{{0,3},{1,3},{2,3}}}), result.second);
}
