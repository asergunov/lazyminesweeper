#include <gtest/gtest.h>

#include <SquareBoard.hpp>


namespace {

using namespace ::minesweeper::engine::square_board;

TEST (SquareBoard, Init) {
    Board b(10, 10);
    EXPECT_EQ(Index(10,10), b.topology.m_dims);
}

TEST (SquareBoard, BasicNeigboursAll) {
    Board b(3, 3);
    EXPECT_EQ(IndexSet({
                           {0,0}, {0,1}, {0,2},
                           {1,0},        {1,2},
                           {2,0}, {2,1}, {2,2}}),
              b.topology.neighbours({1,1}));
}

TEST (SquareBoard, BasicNeigboursCorner) {
    Board b(3, 3);
    EXPECT_EQ(IndexSet({
                                  {0,1},
                           {1,0}, {1,1}}),
              b.topology.neighbours({0,0}));

    EXPECT_EQ(IndexSet({ {1,1}, {1,2},
                         {2,1}         }),
              b.topology.neighbours({2,2}));

    EXPECT_EQ(IndexSet({
                           {0,1},
                           {1,1}, {1,2}}),
              b.topology.neighbours({0,2}));
}

TEST (SquareBoard, BasicNeigboursSide) {
    Board b(3, 3);
    EXPECT_EQ(IndexSet({ {0,0},        {0,2},
                         {1,0}, {1,1}, {1,2}}),
              b.topology.neighbours({0,1}));

    EXPECT_EQ(IndexSet({
                           {1,0}, {1,1}, {1,2},
                           {2,0},        {2,2},
                       }),
              b.topology.neighbours({2,1}));

    EXPECT_EQ(IndexSet({
                           {0,0}, {0,1},
                                  {1,1},
                           {2,0}, {2,1},
                       }),
              b.topology.neighbours({1,0}));


    EXPECT_EQ(IndexSet({
                           {0,1}, {0,2},
                           {1,1},
                           {2,1}, {2,2},
                       }),
              b.topology.neighbours({1,2}));
}


}
