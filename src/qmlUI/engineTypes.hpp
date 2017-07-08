#pragma once

#include "../engine/SquareBoard.hpp"
#include "../engine/PlayerBoardData.hpp"
#include "../engine/PrivateBoardData.hpp"
#include "../engine/Solver.hpp"
#include "../engine/Board.hpp"

using Topology = minesweeper::engine::square_board::Topology;
using GameEngine = minesweeper::engine::solver::GameEngine<Topology>;
