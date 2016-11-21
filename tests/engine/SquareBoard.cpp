#include <gtest/gtest.h>

#include <SquareBoard.hpp>


namespace {

using namespace ::minesweeper::engine::square_board;

TEST (SquareTopology, Init) {
    Topology b(10, 10);
    EXPECT_EQ(Index(10,10), b.m_dims);
}

TEST (SquareTopology, BasicNeigboursAll) {
    Topology b(3, 3);
    EXPECT_EQ(IndexSet({
                           {0,0}, {0,1}, {0,2},
                           {1,0},        {1,2},
                           {2,0}, {2,1}, {2,2}}),
              b.neighbours({1,1}));
}

TEST (SquareTopology, BasicNeigboursCorner) {
    Topology b(3, 3);
    EXPECT_EQ(IndexSet({
                                  {0,1},
                           {1,0}, {1,1}}),
              b.neighbours({0,0}));

    EXPECT_EQ(IndexSet({ {1,1}, {1,2},
                         {2,1}         }),
              b.neighbours({2,2}));

    EXPECT_EQ(IndexSet({
                           {0,1},
                           {1,1}, {1,2}}),
              b.neighbours({0,2}));
}

TEST (SquareTopology, BasicNeigboursSide) {
    Topology b(3, 3);
    EXPECT_EQ(IndexSet({ {0,0},        {0,2},
                         {1,0}, {1,1}, {1,2}}),
              b.neighbours({0,1}));

    EXPECT_EQ(IndexSet({
                           {1,0}, {1,1}, {1,2},
                           {2,0},        {2,2},
                       }),
              b.neighbours({2,1}));

    EXPECT_EQ(IndexSet({
                           {0,0}, {0,1},
                                  {1,1},
                           {2,0}, {2,1},
                       }),
              b.neighbours({1,0}));


    EXPECT_EQ(IndexSet({
                           {0,1}, {0,2},
                           {1,1},
                           {2,1}, {2,2},
                       }),
              b.neighbours({1,2}));
}

}
