#include "boardwrapper.h"

bool draw_board(Board* board, Vector2* mouse_click_pos)
{
	Rectangle board_rect = {ORIGIN_X, ORIGIN_Y, GRID_W * CELL_SIZE, GRID_H * CELL_SIZE};
	DrawRectangleRec(board_rect, COLOR_BOARD_BG);
	

	for (int x = 0; x < GRID_W; x++)
	{
		for (int y = 0; y < GRID_H; y++)
		{
			int i = y * GRID_W + x;

			Rectangle rect = {(float)ORIGIN_X + CELL_SIZE * x, (float)ORIGIN_Y + CELL_SIZE * y, CELL_SIZE, CELL_SIZE};
			DrawRectangleLinesEx(rect, 1, COLOR_BOARD_LINE);

		}
	}

	Vector2 mouse_pos = GetMousePosition();
	if (CheckCollisionPointRec(mouse_pos, board_rect))
	{
		Vector2 grid_pos;
		grid_pos.x = min(GRID_W-2, (int)floorf((mouse_pos.x - board_rect.x) / CELL_SIZE));
		grid_pos.y = min(GRID_H-2, (int)floorf((mouse_pos.y - board_rect.y) / CELL_SIZE));

		DrawRectangleLinesEx({
			board_rect.x + grid_pos.x * CELL_SIZE,
			board_rect.y + grid_pos.y * CELL_SIZE,
			CELL_SIZE*2,
			CELL_SIZE*2
		}, 4, {255,0,255,255});

	}

	return false;

}
