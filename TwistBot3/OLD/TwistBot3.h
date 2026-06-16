using namespace std::chrono;
typedef struct MoveCandidate
{
	Vector2 pos;
	int score;
} MoveCandidate; 
MoveCandidate FindBestMove(Board* board, int level);
