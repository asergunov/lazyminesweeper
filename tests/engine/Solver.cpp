#include <gtest/gtest.h>

#include <SquareBoard.hpp>
#include <Solver.hpp>

using namespace minesweeper::engine::solver;
using namespace minesweeper::engine::square_board;

struct SquareFixture {
    
};

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

TEST(Solver, combinatoins_count) {
    Solver<Topology> s;
    EXPECT_EQ(1, s.combinations_count(0, 4));
    EXPECT_EQ(1, s.combinations_count(0, 3));
    EXPECT_EQ(1, s.combinations_count(3, 3));
    EXPECT_EQ(1, s.combinations_count(6, 6));

    EXPECT_EQ(3, s.combinations_count(1, 3));
    EXPECT_EQ(8, s.combinations_count(1, 8));

    EXPECT_EQ(3*2*1/(2*1), s.combinations_count(2, 3));
    EXPECT_EQ(8*7*6*5*4*3*2*1/(2*1 * 6*5*4*3*2*1), s.combinations_count(2, 8));

    EXPECT_EQ(8, s.combinations_count(1, 8));
    EXPECT_EQ(8*7*6*5*4*3*2*1/(5*4*3*2*1)/(3*2*1), s.combinations_count(5, 8));
}

TEST(SqureSolver, SimpleNumberOfSolutions) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);

    Solver<Topology> s;

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(8, s.number_of_solutions(e.first));
}

TEST(SqureSolver, SimpleNumberOfSolutions_corner) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);

    Solver<Topology> s;

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(3, s.number_of_solutions(e.first));
}

TEST(SqureSolver, ComplexNumberOfSolutions) {
    Topology t(4,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);
    data.setOpened({2,1}, 1);

    Solver<Topology> s;

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(3*3+4, s.number_of_solutions(e.first));
}

TEST(SqureSolver, Corner3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 3);

    Solver<Topology> s;

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(1, s.number_of_solutions(e.first));
}

TEST(SqureSolver, Corner3_1) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 3);
    data.setOpened({1,0}, 1);

    Solver<Topology> s;

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(0, s.number_of_solutions(e.first));
}

TEST(SqureSolver, Side3_1) {
    Topology t(4,2);
    PlayerBoardData<Topology> data;
    data.setOpened({1,0}, 3);
    data.setOpened({2,0}, 1);

    Solver<Topology> s;

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(2, s.number_of_solutions(e.first));
}

TEST(SqureSolver, Porabablities_Corner_3) {
    Topology t(2,2);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 3);

    Solver<Topology> s;
    Solver<Topology>::index_porapablities expected =
    {{{0,1}, 1}, {{1,1}, 1}, {{1,0}, 1} };

    EXPECT_EQ(expected, s.probablities(t, data));
}

TEST(SqureSolver, Porabablities_Corner_1) {
    Topology t(2,2);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);

    Solver<Topology> s;
    Solver<Topology>::index_porapablities expected =
    {{{0,1}, {1,3}}, {{1,1}, {1,3}}, {{1,0}, {1,3}} };

    EXPECT_EQ(expected, s.probablities(t, data));
}

TEST(SqureSolver, Porabablities_Side3_1) {
    Topology t(4,2);
    PlayerBoardData<Topology> data;
    data.setOpened({1,0}, 3);
    data.setOpened({2,0}, 1);

    Solver<Topology> s;
    Solver<Topology>::index_porapablities expected =
    {{{0,0}, 1}, {{0,1}, 1},
    {{3,0}, 0}, {{3,1}, 0},
    {{1,1}, {1,2}}, {{2,1}, {1,2}}};

    EXPECT_EQ(expected, s.probablities(t, data));
}
