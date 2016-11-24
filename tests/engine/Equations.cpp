#include <gtest/gtest.h>

#include <SquareBoard.hpp>
#include <Solver.hpp>

using namespace minesweeper::engine::solver;
using namespace minesweeper::engine::square_board;

TEST(Equations, SizeCheck) {
    Equations e(4, 7);

    EXPECT_EQ(4, e._A.size1());
    EXPECT_EQ(7, e._A.size2());

    EXPECT_EQ(4, e._b.size());
    EXPECT_EQ(7, e._bound.size());

    EXPECT_EQ(4, e.count());
    EXPECT_EQ(7, e.variables_count());
}

TEST(Equations, SimpleReduce) {
    Equations e(1, 1);
    e._A(0,0) = 1;
    e._b(0) = 1;

    Equations::reduction_type reduction{{0, 1}};
    Equations res(1,0);
    res._b(0) = 0;
    EXPECT_EQ(res, e.reduced(reduction));
}

/**
 * 1*x1 + 3*x2 = 1
 * 4*x1 + 2*x2 = 1
 *
 * x1 = 1
 *
 * 3*x2 = 0
 * 2*x2 = -3
 */
TEST(Equations, Reduce2x2) {
    Equations e(2, 2);
    e._A(0,0) = 1; e._A(0,1) = 3;
    e._A(1,0) = 4; e._A(1,1) = 2;

    e._b(0) = 1;
    e._b(1) = 1;

    e._bound[0] = 3;
    e._bound[1] = 6;

    Equations::reduction_type reduction{{0, 1}};
    Equations res(2,1);
    res._A(0,0) = 3;
    res._A(1,0) = 2;
    res._bound[0] = 6;
    res._b(0) = 0;
    res._b(1) = -3;
    EXPECT_EQ(res, e.reduced(reduction));
}

/**
 * 1*x1 + 3*x2 + 4*x3 + 1*x4 = 1
 * 0*x1 + 5*x2 + 2*x3 + 3*x4 = 1
 *
 * x1 = 1, x4 = -1
 *
 * 3*x2 + 4*x3  = 1
 * 5*x2 + 2*x3  = 4
 *
 */
TEST(Equations, Reduce2x4) {
    Equations e(2, 4);
    e._A(0,0) = 1; e._A(0,1) = 3; e._A(0,2) = 4; e._A(0,3) = 1;
    e._A(1,0) = 0; e._A(1,1) = 5; e._A(1,2) = 2; e._A(1,3) = 3;

    e._b(0) = 1;
    e._b(1) = 1;

    e._bound(0) = 1;
    e._bound(1) = 2;
    e._bound(2) = 3;
    e._bound(3) = 4;

    Equations::reduction_type reduction{{0, 1}, {3, -1}};
    Equations res(2,2);
    res._A(0,0) = 3; res._A(0,1) = 4;
    res._A(1,0) = 5; res._A(1,1) = 2;

    res._bound(0) = 2;
    res._bound(1) = 3;

    res._b(0) = 1;
    res._b(1) = 4;
    EXPECT_EQ(res, e.reduced(reduction));
}
