#include <gtest/gtest.h>

#include <SquareBoard.hpp>
#include <Solver.hpp>

using namespace minesweeper::engine::solver;
using namespace minesweeper::engine::square_board;

using gauss_key_columns_type = Solver<Topology>::gauss_key_columns_type;

struct SquareFixture {
    
};

/**
 * ...
 * .1.
 * ...
 */
TEST(SqureSolver, simple3x3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);
    data.totalBombCount = 1;

    Solver<Topology> s(t);
    const auto& result = s.parseBoard(t, data);

    auto equations1 = Equations(1,1);
    equations1._A(0,0) = 1;
    equations1._b(0) = 1;
    equations1._bound(0) = 8;

    auto equations2 = Equations(2,2);
    equations2._A(0,0) = 1;
    equations2._A(0,1) = 0;

    equations2._A(1,0) = 1;
    equations2._A(1,1) = 1;

    equations2._b(0) = 1;
    equations2._b(1) = 1;
    equations2._bound(0) = 8;
    equations2._bound(1) = 0;

    EXPECT_EQ(equations2, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({t.neighbours({1,1}), {}}), result.second);
}

/**
 * ...
 * .3.
 * ...
 */
TEST(SqureSolver, simple3x3_3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 3);
    data.totalBombCount = 3;

    Solver<Topology> s(t);
    const auto& result = s.parseBoard(t, data);

    auto equations = Equations(2,2);
    equations._A(0,0) = 1;
    equations._A(1,0) = 1;
    equations._A(1,1) = 1;
    equations._b(0) = 3;
    equations._b(1) = 3;
    equations._bound(0) = 8;
    equations._bound(1) = 0;

    EXPECT_EQ(equations, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({t.neighbours({1,1}), {}}), result.second);
}

/**
 * 1..
 * ...
 * ...
 */
TEST(SqureSolver, corner3x3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);
    data.totalBombCount = 3;

    Solver<Topology> s(t);
    const auto& result = s.parseBoard(t, data);

    auto equations = Equations(2,2);
    equations._A(0,0) = 1;
    equations._A(1,0) = 1;
    equations._A(1,1) = 1;
    equations._b(0) = 1;
    equations._b(1) = 3;
    equations._bound(0) = 3;
    equations._bound(1) = 5;

    EXPECT_EQ(equations, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({t.neighbours({0,0}), {{0,2}, {1,2},{2,2}, {2,1}, {2,0}}}), result.second);
}

/**
 * ...
 * .1.
 * .2.
 * ...
 */
TEST(SqureSolver, corner4x3_1_1) {
    Topology t(3,4);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);
    data.setOpened({1,2}, 2);
    data.totalBombCount = 3;

    Solver<Topology> s(t);
    const auto& result = s.parseBoard(t, data);

    auto equations = Equations(3,4);
    equations._A(0,0) = 1;
    equations._A(0,1) = 1;
    equations._A(1,1) = 1;
    equations._A(1,2) = 1;
    equations._A(2,0) = 1;
    equations._A(2,1) = 1;
    equations._A(2,2) = 1;
    equations._A(2,3) = 1;
    equations._b(0) = 1;
    equations._b(1) = 2;
    equations._b(2) = 3;
    equations._bound(0) = 3;
    equations._bound(1) = 4;
    equations._bound(2) = 3;
    equations._bound(3) = 0;

    EXPECT_EQ(equations, result.first);
    EXPECT_EQ(Solver<Topology>::mapping_type({{{0,0},{1,0},{2,0}},{{0,1},{0,2},{2,1},{2,2}},{{0,3},{1,3},{2,3}}, {}}), result.second);
}

TEST(Solver, combinatoins_count) {
    Solver<Topology> s(8);
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

TEST(Solver, number_of_solutions_corner_solvable_1) {
    Solver<Topology> s(3);
    Equations e(1,0);
    e._b(0) = 0;
    EXPECT_EQ(1, s.number_of_solutions(e));
}

TEST(Solver, number_of_solutions_corner_solvable_4) {
    Solver<Topology> s(8);
    Equations e(4,0);
    e._b(0) = 0;
    e._b(1) = 0;
    e._b(2) = 0;
    e._b(3) = 0;
    EXPECT_EQ(1, s.number_of_solutions(e));
}

TEST(Solver, number_of_solutions_corner_non_solvable_1) {
    Solver<Topology> s(8);
    Equations e(1,0);
    e._b(0) = 1;
    EXPECT_EQ(0, s.number_of_solutions(e));
}

TEST(Solver, number_of_solutions_corner_non_solvable_3) {
    Solver<Topology> s(8);
    Equations e(3,0);
    e._b(0) = 0;
    e._b(1) = 1;
    e._b(2) = -1;
    EXPECT_EQ(0, s.number_of_solutions(e));
}

/**
 * ...
 * .1.
 * ...
 */
TEST(SqureSolver, SimpleNumberOfSolutions) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);
    data.totalBombCount = 1;

    Solver<Topology> s(t);

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(8, s.number_of_solutions(e.first));
}

/**
 * 1..
 * ...
 * ...
 * total: 1
 */
TEST(SqureSolver, SimpleNumberOfSolutions_corner_1) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);
    data.totalBombCount = 1;

    Solver<Topology> s(t);

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(3, s.number_of_solutions(e.first));
}

/**
 * 1..
 * ...
 * ...
 *
 * total: 2
 */
TEST(SqureSolver, SimpleNumberOfSolutions_corner_2) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);
    data.totalBombCount = 2;

    Solver<Topology> s(t);

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(3*5, s.number_of_solutions(e.first));
}

/**
 * ....
 * .11.
 * ....
 */
TEST(SqureSolver, ComplexNumberOfSolutions) {
    Topology t(4,3);
    PlayerBoardData<Topology> data;
    data.setOpened({1,1}, 1);
    data.setOpened({2,1}, 1);

    data.totalBombCount = 1;

    Solver<Topology> s(t);

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(4, s.number_of_solutions(e.first));
}

/**
 * 3..
 * ...
 * ...
 */
TEST(SqureSolver, Corner3) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 3);

    data.totalBombCount = 3;

    Solver<Topology> s(t);

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(1, s.number_of_solutions(e.first));
}

/**
 * 31.
 * ...
 * ...
 */
TEST(SqureSolver, Corner3_1) {
    Topology t(3,3);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 3);
    data.setOpened({1,0}, 1);

    data.totalBombCount = 3;

    Solver<Topology> s(t);

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(0, s.number_of_solutions(e.first));
}

/**
 * .31.
 * ....
 */
TEST(SqureSolver, Side3_1) {
    Topology t(4,2);
    PlayerBoardData<Topology> data;
    data.setOpened({1,0}, 3);
    data.setOpened({2,0}, 1);

    data.totalBombCount = 3;

    Solver<Topology> s(t);

    const auto& e = s.parseBoard(t, data);
    EXPECT_EQ(2, s.number_of_solutions(e.first));
}

/**
 * 3.
 * ..
 */
TEST(SqureSolver, Porabablities_Corner_3) {
    Topology t(2,2);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 3);

    data.totalBombCount = 3;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected =
    {{{0,1}, {1,1}}, {{1,1}, {1,1}}, {{1,0}, {1,1}} };

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * 1.
 * ..
 */
TEST(SqureSolver, Porabablities_Corner_1) {
    Topology t(2,2);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);
    data.totalBombCount = 1;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected =
    {{{0,1}, {1,3}}, {{1,1}, {1,3}}, {{1,0}, {1,3}} };

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * .31.
 * ....
 */
TEST(SqureSolver, Porabablities_Side3_1) {
    Topology t(4,2);
    PlayerBoardData<Topology> data;
    data.setOpened({1,0}, 3);
    data.setOpened({2,0}, 1);
    data.totalBombCount = 3;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected =
    {{{0,0}, {1,1}}, {{0,1}, {1,1}},
    {{3,0}, {0,1}}, {{3,1}, {0,1}},
    {{1,1}, {1,2}}, {{2,1}, {1,2}}};

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * 0.
 * ..
 */
TEST(Solver, Porabablities_Zero) {
    Topology t(2,2);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 0);

    data.totalBombCount = 0;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected =
    {{{0,1}, {0,1}},
    {{1,0}, {0,1}}, {{1,1}, {0,1}}};

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * 0
 */
TEST(Solver, Porabablities_AllZeros) {
    Topology t(1,1);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 0);
    data.totalBombCount = 0;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected = {};

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * ..1
 * total: 2
 */
TEST(Solver, Porabablities_unknown_simpe) {
    Topology t(3,1);
    PlayerBoardData<Topology> data;
    data.setOpened({2,0}, 1);
    data.totalBombCount = 2;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected = {{{0,0}, {1,1}}, {{1,0}, {1,1}}};

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * ..1
 * total: 1
 */
TEST(Solver, Porabablities_unknown_simpe_2) {
    Topology t(3,1);
    PlayerBoardData<Topology> data;
    data.setOpened({2,0}, 1);
    data.totalBombCount = 1;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected = {{{0,0}, {0,1}}, {{1,0}, {1,1}}};

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * 1....
 * total: 2
 */
TEST(Solver, Porabablities_unknown_simpe_3) {
    Topology t(5,1);
    PlayerBoardData<Topology> data;
    data.setOpened({0,0}, 1);
    data.totalBombCount = 2;

    Solver<Topology> s(t);
    Solver<Topology>::index_porapablities expected = {{{1,0}, {1,1}}, {{2,0}, {1,3}}, {{3,0}, {1,3}}, {{4,0}, {1,3}}};

    EXPECT_EQ(expected, s.probablities(t, data));
}

/**
 * x1 + x2 = 1
 * x1 + x2 = 1
 */
TEST(Solver, orthogonalize) {
    Equations e(2, 2);
    e._A(0,0) = 1; e._A(0,1) = 1;
    e._A(1,0) = 1; e._A(1,1) = 1;
    e._b(0) = 1;
    e._b(1) = 1;
    e._bound(0) = 1;
    e._bound(1) = 1;


    Equations expected(1, 2);
    expected._A(0,0) = 2; expected._A(0,1) = 2;
    expected._b(0) = 2;
    expected._bound(0) = 1;
    expected._bound(1) = 1;

    Solver<Topology> s(8);

    EXPECT_EQ(expected, s.orthogonalize(e));
}


/**
 * x1 + x2 = 1
 * x3 + x4 = 1
 * 3*x3 + 3*x4 = 3
 */
TEST(Solver, orthogonalize_2) {
    Equations e(3, 4);
    e._b(0) = e._A(0,0) = e._A(0,1) = 1;
    e._b(1) = e._A(1,2) = e._A(1,3) = 1;
    e._b(2) = e._A(2,2) = e._A(2,3) = 3;
    e._bound(0) = 1;
    e._bound(1) = 1;
    e._bound(2) = 1;
    e._bound(3) = 1;


    Equations expected(2, 4);
    expected._b(0) = expected._A(0,0) = expected._A(0,1) = 1;
    expected._b(1) = expected._A(1,2) = expected._A(1,3) = 10;

    expected._bound(0) = 1;
    expected._bound(1) = 1;
    expected._bound(2) = 1;
    expected._bound(3) = 1;

    Solver<Topology> s(8);

    EXPECT_EQ(expected, s.orthogonalize(e));
}

/**
 * x1 + x2 = 1
 * x1 + x2 + x3 + x4 = 2
 */
TEST(Solver, gauss) {
    Equations e(2, 4);
    e._b(0) = e._A(0,0) = e._A(0,1) = 1;
    e._A(1,0) = e._A(1,1) = e._A(1,2) = e._A(1,3) = 1;
    e._b(1) = 2;
    e._bound(0) = 1;
    e._bound(1) = 1;
    e._bound(2) = 1;
    e._bound(3) = 1;


    Equations expected(2, 4);
    expected._A(0,0) = 1; expected._A(0,1) = 1;
    expected._A(1,2) = 1; expected._A(1,3) = 1;

    expected._b(0) = 1;
    expected._b(1) = 1;

    expected._bound = e._bound;

    Solver<Topology> s(8);

    EXPECT_EQ(std::make_pair(expected, gauss_key_columns_type{0, 2}), s.gauss(e));
    EXPECT_EQ(s.specter(e), s.specter(s.gauss(e).first));
}

/**
 * x1 + x2           = 1
 * x1 + x2 + x3 + x4 = 3
 *      x2           = 1
 *
 * should become
 *
 * x1               = 1
 *      x2          = 1
 *          x3 + x4 = 2
 */
TEST(Solver, gauss_2) {
    Equations e(3, 4);
    e._b(0) = e._A(0,0) = e._A(0,1) = 1;
    e._A(1,0) = e._A(1,1) = e._A(1,2) = e._A(1,3) = 1;
    e._b(1) = 3;
    e._b(2) = e._A(2,1) = 1;
    e._bound(0) = 1;
    e._bound(1) = 1;
    e._bound(2) = 1;
    e._bound(3) = 1;


    Equations expected(3, 4);
    expected._A(0,0) = expected._A(1,1) = expected._A(2,2) = expected._A(2,3) = 1;

    expected._b(0) = 0;
    expected._b(1) = 1;
    expected._b(2) = 2;

    expected._bound = e._bound;

    Solver<Topology> s(8);

    EXPECT_EQ(std::make_pair(expected, gauss_key_columns_type{0, 1, 2}), s.gauss(e));
    EXPECT_EQ(s.specter(e), s.specter(s.gauss(e).first));
}

/**
 * 1..1
 */
TEST(Solver, decompose) {
    Topology t(4, 1);

    PlayerBoardData<Topology> p;
    p.setOpened({0,0}, 1);
    p.setOpened({3, 0}, 1);
    p.totalBombCount = 2;

    Solver<Topology> s(t);

    std::vector<Solver<Topology>::mapped_equations_type> expected;

    EXPECT_EQ(expected, s.decompose(s.parseBoard(t, p)));
}
