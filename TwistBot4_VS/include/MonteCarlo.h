#pragma once
#include <list>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#include <stdlib.h>

#include "Board.h"
#include "HandyTypes.hpp"

using namespace std;
constexpr double UCB1_CONST = 10000.0;
namespace MonteCarlo
{

	// To be stored on heap
	class Node
	{
	public:
		// The coordinates of the move (upon the parent Node) that results in this Node
		Vec2 move_pos = {-1, -1};

		// Score of this node
		int64_t t = 0;
		// Number of passes on this node
		int n = 0;
		int64_t t_initial = 0;
		int64_t rollout_t = 0;
		
		int depth = 0;

		double log_n = 0.0;

		bool leaf = true;
		bool bomb_defused = false;
		int moves_to_defuse_bomb = 999;

		// Board representing current state
		Board board;

		Node* children[7*7];
		int children_size = 0;

		Node* parent = nullptr;

		// Pointer to a list of Node*
		list<Node*>* node_list;

		Node(list<Node*>* node_list);

		Node* get_best_child_ucb1();

		// less expensive than get_best_child_ucb1
		Node* get_best_child_by_score();
		void expand();
		int64_t rollout();

		double calculate_ucb1();

		inline void set_parent(Node* parent)
		{
			this->parent = parent;
		}

	};

	Vec2 run_algorithm(Board* board, int runs);
}


