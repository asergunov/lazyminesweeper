#include <gtest/gtest.h>

#include <PlayerBoardData.hpp>
#include <PrivateBoardData.hpp>
#include <SquareBoard.hpp>

using namespace minesweeper::engine::solver;
using namespace minesweeper::engine::square_board;

TEST(SuqareBoardDataPrivate, init) {
    PrivateBoardData<Topology> privateData;
}


TEST(SuqareBoardDataPlayer, init) {
    PlayerBoardData<Topology> playerData;
}

TEST(SuqareBoardData, openFieldSimple) {
     PrivateBoardData<Topology> priv;
     PlayerBoardData<Topology> player;

     Topology t(1,1);
     priv.openField(player, {0,0}, t);

     PlayerBoardData<Topology>::opened_items_type expected={{{0,0},0}};

     EXPECT_EQ(expected, player._openedItems);
     EXPECT_EQ(0, player.totalBombCount);
}

TEST(SuqareBoardData, openFieldSimple_one) {
     PrivateBoardData<Topology> priv;
     PlayerBoardData<Topology> player;

     Topology t(2,2);

     priv.bombs = {{0,0}};

     priv.openField(player, {1,0}, t);

     PlayerBoardData<Topology>::opened_items_type expected={{{1,0},1}};
     EXPECT_EQ(expected, player._openedItems);
     EXPECT_EQ(1, player.totalBombCount);

     priv.openField(player, {1,1}, t);
     expected.insert({{1,1}, 1});

     EXPECT_EQ(expected, player._openedItems);
     EXPECT_EQ(1, player.totalBombCount);

     priv.openField(player, {0,1}, t);
     expected.insert({{0,1}, 1});

     EXPECT_EQ(expected, player._openedItems);
     EXPECT_EQ(1, player.totalBombCount);
}

TEST(SuqareBoardData, openFieldSimple_lose) {
     PrivateBoardData<Topology> priv;
     PlayerBoardData<Topology> player;

     Topology t(1,1);
     priv.bombs = {{0,0}};

     priv.openField(player, {0,0}, t);

     PlayerBoardData<Topology>::opened_items_type expected={};

     EXPECT_EQ(expected, player._openedItems);
     EXPECT_TRUE(player.isGameOver());
     EXPECT_EQ(Topology::index_type(0,0), player.looseIndex());
}

TEST(SuqareBoardData, openFieldRecursiveOne) {
     PrivateBoardData<Topology> priv;
     PlayerBoardData<Topology> player;

     Topology t(3,3);

     priv.bombs = {{0,0}};
     priv.openField(player, {2,2}, t);

     PlayerBoardData<Topology>::opened_items_type expected={
                    {{0,1},1}, {{0,2},0},
         {{1,0},1}, {{1,1},1}, {{1,2},0},
         {{2,0},0}, {{2,1},0}, {{2,2},0}};

     EXPECT_EQ(expected, player._openedItems);
     EXPECT_EQ(1, player.totalBombCount);
}


TEST(SuqareBoardData, openFieldRecursiveNone) {
     PrivateBoardData<Topology> priv;
     PlayerBoardData<Topology> player;

     Topology t(2,2);
     priv.openField(player, {0,0}, t);

     PlayerBoardData<Topology>::opened_items_type expected={{{0,0},0}, {{0,1},0}, {{1,0},0}, {{1,1},0}};

     EXPECT_EQ(expected, player._openedItems);
     EXPECT_EQ(0, player.totalBombCount);
}

