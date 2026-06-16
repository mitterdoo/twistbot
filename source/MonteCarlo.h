#pragma once
#include <list>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#include <stdlib.h>
#include <cstdint>

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
		Vector2 movePos = {-1, -1};

		// Score of this node
		int64_t t = 0;
		// Number of passes on this node
		int n = 0;
		int64_t t_initial = 0;
		int64_t rollout_t = 0;
		
		int depth = 0;

		double logN = 0.0;

		bool leaf = true;
		bool bombDefused = false;
		int bombDefusedInXMoves = 999;

		// Board representing current state
		Board board;

		Node* children[7*7];
		int children_size = 0;

		Node* parent = nullptr;

		// Pointer to a list of Node*
		list<Node*>* nodesList;

		Node(list<Node*>* nodesList);

		Node* GetBestChild();
		Node* GetFinalBestChild();
		void Expand();
		int64_t Rollout();

		double UCB1();

		inline void SetParent(Node* parent)
		{
			this->parent = parent;
		}

	};

	Vector2 RunAlgorithm(Board* board, int runs);
}


