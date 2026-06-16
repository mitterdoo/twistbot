#include "MonteCarlo.h"

namespace MonteCarlo
{
	Node::Node(list<Node*>* nodesList)
	{
		this->nodesList = nodesList;
	}

	double Node::UCB1()
	{
		if (parent == nullptr)
		{
			throw new runtime_error("Parent is null");
		}
		//return ((double)t / (double)n + UCB1_CONST * sqrt(parent->logN / n));
		return ((double)t / (double)n + UCB1_CONST * sqrt(log(parent->n) / n));
	}

	Node* Node::GetBestChild()
	{
		if (leaf)
		{
			throw new runtime_error("Leaves have no children.");
		}

		double best_score = 0.0;
		Node* best_node = children[0];

		for (int i = 0; i < children_size; i++)
		{
			Node* child = children[i];
			double ucb1;
			if (child == nullptr)
			{
				throw new runtime_error("Found null child");
			}
			if (child->n == 0)
			{
				return child;
			}
			else if ((ucb1 = child->UCB1()) >= best_score)
			{
				best_score = ucb1;
				best_node = child;
			}
		}

		return best_node;
	}

	Node* Node::GetFinalBestChild()
	{
		if (leaf)
		{
			throw new runtime_error("Leaves have no children.");
		}

		double best_score = 0.0;
		Node* best_node = children[0];

		for (int i = 0; i < children_size; i++)
		{
			Node* child = children[i];
			if (child == nullptr)
			{
				throw new runtime_error("Found null child");
			}
			double calculated = (double)child->t / (double)child->n;
			if (calculated > best_score)
			{
				best_score = calculated;
				best_node = child;
			}
		}

		return best_node;
	}



	void Node::Expand()
	{
		if (!leaf)
		{
			throw new runtime_error("Cannot expand node that isn't a leaf");
		}
		for (int x = 0; x < 7; x++)
		{
			for (int y = 0; y < 7; y++)
			{
				// redundant move
				if (board.gems[x][y].color == board.gems[x + 1][y].color &&
					board.gems[x + 1][y].color == board.gems[x + 1][y + 1].color &&
					board.gems[x + 1][y + 1].color == board.gems[x][y + 1].color)
					continue;

				// impossible move
				if (board.gems[x][y].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
					board.gems[x + 1][y].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
					board.gems[x + 1][y + 1].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
					board.gems[x][y + 1].flags & (GemFlags::LOCKED | GemFlags::DOOM))
					continue;


				/*
				board.Rotate({x, y});
				int check = board.CheckIfGemInMatch(&board.gems[x][y]);
				check = board.CheckIfGemInMatch(&board.gems[x + 1][y]);
				check = board.CheckIfGemInMatch(&board.gems[x + 1][y + 1]);
				check = board.CheckIfGemInMatch(&board.gems[x + 1][y]);
				*/


				/*
				board.Rotate({x, y});
				if (!board.CheckIfGemInMatch(&board.gems[x][y]) &&
					!board.CheckIfGemInMatch(&board.gems[x + 1][y]) &&
					!board.CheckIfGemInMatch(&board.gems[x + 1][y + 1]) &&
					!board.CheckIfGemInMatch(&board.gems[x][y + 1]))
				{
					board.AntiRotate({x, y});
					continue;
				}
				board.AntiRotate({x, y});
				*/

				Node* newNode = new Node(nodesList);
				memcpy(&newNode->board, &board, sizeof(Board));

				newNode->SetParent(this);
				newNode->movePos = {x, y};
				newNode->depth = depth + 1;
				//newNode->board.Rotate({x, y});
				//int score = newNode->board.RunMatch();
				//if (score == 0)
				//	score = -1000 * newNode->board.score.multiplier;
				//newNode->t_initial = score;
				//printf("EXPANSION: (%i, %i)\n%s\n\n", x, y, newNode->board.GetString().c_str());
				children[children_size++] = newNode;
				nodesList->push_back(newNode);

			}
		}
		leaf = false;
	}

	int64_t Node::Rollout()
	{

		if (n != 0)
		{
			throw new runtime_error("Cannot roll out with n != 0");
		}

		//printf("rolling out board (%i, %i)\n%s\n", movePos.x, movePos.y, board.GetString().c_str());

		board.Rotate(movePos);
		int64_t initialScore = board.RunMatch() * 1000; // 12000;
		int64_t score = initialScore;
		if (score == 0)
		{
			//score -= 2000;
		}
		if (board.gameOver)
		{
			// backpropagate
			Node* curNode = this;
			do
			{
				curNode->logN = log(++curNode->n);
				curNode = curNode->parent;
			} while (curNode != nullptr);
			return 0;
		}
		if (board.matchResultFlags & MATCHRESULT_DEFUSED_DANGER)
		{
			printf("BOMB DEFUSED IN MAIN MOVE LEVEL %i\n", depth);
			bombDefusedInXMoves = 0;
			bombDefused = true;
		}

		Board tempBoard;
		memcpy(&tempBoard, &board, sizeof(Board));

		//int score = t;

		int matchlessMoves = 0;
		for (int i = 0; i < 8; i++)
		//while (matchlessMoves < 2)
		{
			vector<Vector2> possibleMoves;
			for (int x = 0; x < 7; x++)
			{
				for (int y = 0; y < 7; y++)
				{
					// redundant move
					if (tempBoard.gems[x][y].color == tempBoard.gems[x + 1][y].color &&
						tempBoard.gems[x + 1][y].color == tempBoard.gems[x + 1][y + 1].color &&
						tempBoard.gems[x + 1][y + 1].color == tempBoard.gems[x][y + 1].color)
						continue;

					// impossible move
					if (tempBoard.gems[x][y].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
						tempBoard.gems[x + 1][y].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
						tempBoard.gems[x + 1][y + 1].flags & (GemFlags::LOCKED | GemFlags::DOOM) ||
						tempBoard.gems[x][y + 1].flags & (GemFlags::LOCKED | GemFlags::DOOM))
						continue;

					
					/*
					tempBoard.Rotate({x, y});
					if (tempBoard.CheckIfGemInMatch(&tempBoard.gems[x][y]) || 
						tempBoard.CheckIfGemInMatch(&tempBoard.gems[x + 1][y]) || 
						tempBoard.CheckIfGemInMatch(&tempBoard.gems[x + 1][y + 1]) || 
						tempBoard.CheckIfGemInMatch(&tempBoard.gems[x][y + 1]))
					{
						possibleMoves.push_back({x, y});
					}
					tempBoard.AntiRotate({x, y});
					*/


					possibleMoves.push_back({x, y});
				}
			}

			int size = possibleMoves.size();
			if (size == 0)
			{
				break;
			}
			
			Vector2 action = possibleMoves[rand() % size];
			tempBoard.Rotate(action);
			int moveScore = tempBoard.RunMatch(false);

			if (tempBoard.matchResultFlags & MATCHRESULT_DEFUSED_DANGER)
			{
				bombDefusedInXMoves = min(bombDefusedInXMoves, i + 1);
				printf("BOMB DEFUSE IN %i MOVES\n", bombDefusedInXMoves + depth);
				bombDefused = true;
			}
			if (tempBoard.matchResultFlags & MATCHRESULT_BOMBZERO)
			{
				score = initialScore;
				possibleMoves.clear();
				break;
			}

			if (moveScore == 0)
			{
				matchlessMoves++;
			}
			else
			{
				if (matchlessMoves > 0 && (tempBoard.matchResultFlags & MATCHRESULT_DEFUSED) == 0) // only reward bomb defusals after making matchless moves
				{
					//moveScore = 0;
				}
				matchlessMoves = 0;
			}
			score += moveScore;// > 0;

			//printf("Board state %i:\n%s\n\n", i, tempBoard.GetString().c_str());
			possibleMoves.clear();

		}

		score = (1.0 / (double)(depth)) * score;
		rollout_t = score;
		

		// backpropagate
		Node* curNode = this;
		do
		{
			curNode->logN = log(++curNode->n);
			curNode->t += score;
			curNode = curNode->parent;
		} while (curNode != nullptr);

		return score;

	}

	Vector2 RunAlgorithm(Board* board, int runs)
	{

		printf("BOARD:\n%s\n", board->GetString().c_str());
		list<Node*> nodes;
		Node* primeNode = new Node(&nodes);
		memcpy(&primeNode->board, board, sizeof(Board));

		bool gameEnding = false;
		int gameEndsInXMoves = 999;
		for (int x = 0; x < 8; x++)
		{
			for (int y = 0; y < 8; y++)
			{
				Gem* gem = &board->gems[x][y];
				bool isDoom = gem->Is(GemFlags::DOOM);
				bool isBomb = gem->Is(GemFlags::BOMB);
				if (isBomb || isDoom)				
				{
					gameEnding = true;
					if (gem->count < gameEndsInXMoves)
					{
						gameEndsInXMoves = gem->count;
					}
				}
			}
		}

		primeNode->Expand();

		int bombDefusedInXMoves = 999;
		Vector2 moveToDefuseBomb = {6, 6};
		for (int i = 0; i < runs; i++)
		{
			Node* current = primeNode;
			while (!current->leaf)
			{
				current = current->GetBestChild();
			}
			if (current->n == 0)
			{
				current->Rollout();
				if (current->bombDefused)
				{
					bombDefusedInXMoves = min(bombDefusedInXMoves, current->depth + current->bombDefusedInXMoves);
					if (current->bombDefusedInXMoves == 0)
					{
						moveToDefuseBomb = current->movePos;
						Node* parent = current->parent;
						while (parent != primeNode)
						{
							moveToDefuseBomb = parent->movePos;
							parent = parent->parent;
						}
					}
				}
			}
			else
			{
				current->Expand();
				if (current->children_size == 0)
				{
					printf("children size is 0, can't go deeper\n");
					break;
					//throw new runtime_error("Tried to expand Node, but it resulted in no children!");
				}
				current->children[0]->Rollout();
				if (current->children[0]->bombDefused)
				{
					bombDefusedInXMoves = min(bombDefusedInXMoves, current->children[0]->depth + current->children[0]->bombDefusedInXMoves);
					if (current->children[0]->bombDefusedInXMoves == 0)
					{
						moveToDefuseBomb = current->children[0]->movePos;
						Node* parent = current->children[0]->parent;
						while (parent != primeNode)
						{
							moveToDefuseBomb = parent->movePos;
							parent = parent->parent;
						}
					}
				}
			}

			/*
			if (gameEnding && bombDefusedInXMoves > gameEndsInXMoves && gameEndsInXMoves <= 4)
			{

				if (i == runs - 1)
				{
					printf("GAME ENDS IN %i MOVES; ADDING ANOTHER RUN\n", gameEndsInXMoves);
					runs = min(15000, runs + 1);
					if (runs == 15000)
						printf("poop");
				}
			}
			*/
		}

		if (gameEnding && bombDefusedInXMoves == 1 && gameEndsInXMoves <= 3)
		{
			// do this right now
			// this may prioritize defusing bombs at any length

			
			Vector2 result = moveToDefuseBomb;
			printf("DEFUSING BOMB WITH (%i, %i) (%i TO DEFUSAL)\n", result.x, result.y, bombDefusedInXMoves);
			for (Node* node : nodes)
			{
				delete node;
			}
			return result;
			

		}

		Node* best_node = primeNode->GetFinalBestChild();
		if (best_node->t == 0.0)
		{
			printf("stop\n");
		}
		Vector2 result = best_node->movePos;

		printf("PROPOSED MOVE: (%i, %i)\n", result.x, result.y);
		
		

		int i = 0;
		while (1)
		{
			//if (i == 0) printf("%s\n\n", best_node->board.GetString().c_str());
			printf("%s\n", best_node->board.GetString().c_str());
			printf(": Level %i: (%i, %i)\tScore: %d\tt: %d\trt: %d\tn: %i\n\n", ++i, best_node->movePos.x, best_node->movePos.y, best_node->board.score.score, best_node->t, best_node->rollout_t, best_node->n);
			if (best_node->children_size == 0) break;
			best_node = best_node->GetFinalBestChild();
		}

		int asdf = 0;

		

		for (Node* node : nodes)
		{
			delete node;
		}

		return result;
	}


}




