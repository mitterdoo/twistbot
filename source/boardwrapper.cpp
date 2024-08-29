#include "boardwrapper.h"

bool draw_board(Board* board, Vector2* mouse_click_pos)
{
	if (board == NULL)
	{
		throw new std::exception();
	}

	Rectangle board_rect = {ORIGIN_X, ORIGIN_Y, GRID_W * CELL_SIZE, GRID_H * CELL_SIZE};
	DrawRectangleRec(board_rect, COLOR_BOARD_BG);

	for (int x = 0; x < GRID_W; x++)
	{
		for (int y = 0; y < GRID_H; y++)
		{
			int i = y * GRID_W + x;

			Rectangle rect = {(float)ORIGIN_X + CELL_SIZE * x, (float)ORIGIN_Y + CELL_SIZE * y, CELL_SIZE, CELL_SIZE};
			DrawRectangleLinesEx(rect, 1, COLOR_BOARD_LINE);

			// draw the gem fool
			Gem* gem = &(board->gems[x][y]);
			if (gem != NULL)
			{
				Color gem_color = GEM_COLORS[(int)gem->color];
				Rectangle gem_rect = {
					rect.x + GEM_PADDING,
					rect.y + GEM_PADDING,
					rect.width - GEM_PADDING*2,
					rect.height - GEM_PADDING*2
				};
				DrawRectangleRounded(gem_rect, 0.5, 16, gem_color);
				DrawRectangleRoundedLines(gem_rect, 0.5, 16, 4, {0, 0, 0, 255});

			}

		}
	}

	Vector2 mouse_pos = GetMousePosition();



	if (CheckCollisionPointRec(mouse_pos, board_rect))
	{
		Vector2 grid_pos;
		grid_pos.x = max(0, min(GRID_W-2, (int)floorf((mouse_pos.x - board_rect.x - CELL_SIZE/2) / CELL_SIZE)));
		grid_pos.y = max(0, min(GRID_H-2, (int)floorf((mouse_pos.y - board_rect.y - CELL_SIZE/2) / CELL_SIZE)));

		bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

		DrawRing({board_rect.x + grid_pos.x * CELL_SIZE + CELL_SIZE, board_rect.y + grid_pos.y * CELL_SIZE + CELL_SIZE},
			CELL_SIZE - 2, CELL_SIZE + 2, 0, 360, 32, {255,0,255,255});

		/* DrawRectangleLinesEx({
			board_rect.x + grid_pos.x * CELL_SIZE,
			board_rect.y + grid_pos.y * CELL_SIZE,
			CELL_SIZE*2,
			CELL_SIZE*2
		}, 4, {255,0,255,255}); */
		if (clicked)
		{
			mouse_click_pos->x = grid_pos.x;
			mouse_click_pos->y = grid_pos.y;
			return true;
		}

	}

	return false;

}
