#include "boardwrapper.h"

void load_sprites()
{

	printf("Loading sprites...");

	memset((void*)GEM_SPRITES, 0, sizeof(Texture2D) * 9);

	GEM_SPRITES[0] = LoadTexture("../res/gem_0.png");
	GEM_SPRITES[1] = LoadTexture("../res/gem_1.png");
	GEM_SPRITES[2] = LoadTexture("../res/gem_2.png");
	GEM_SPRITES[3] = LoadTexture("../res/gem_3.png");
	GEM_SPRITES[4] = LoadTexture("../res/gem_4.png");
	GEM_SPRITES[5] = LoadTexture("../res/gem_5.png");
	GEM_SPRITES[6] = LoadTexture("../res/gem_6.png");
	GEM_SPRITES[7] = LoadTexture("../res/gem_7.png");

	for (int i = 0; i < 8; i++)
	{
		while(!IsTextureReady(GEM_SPRITES[i]));
	}

	printf("loaded.\n");

}

void unload_sprites()
{
	for (int i = 0; i < 8; i++)
	{
		UnloadTexture(GEM_SPRITES[i]);
	}
}

bool draw_board(Board* board, Vector2* cursor_pos)
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
			if (gem != NULL && gem->color != GemColor::EMPTY)
			{
				bool is_flame = gem->Is(GemFlags::FLAME);
				bool is_lightning = gem->Is(GemFlags::LIGHTNING);

				if (is_flame && !is_lightning)
				{
					DrawRectangleRec(rect, {255, 128, 0, 96});
				}
				else if (!is_flame && is_lightning)
				{
					DrawRectangleRec(rect, {255, 255, 255, 96});
				}
				else if (is_flame && is_lightning)
				{
					DrawRectangleRec(rect, {255, 0, 255, 128});
				}


				Texture2D tex = GEM_SPRITES[(int)gem->color];
				DrawTextureEx(GEM_SPRITES[(int)gem->color], {rect.x, rect.y}, 0, 0.5, {255,255,255,255});

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
		cursor_pos->x = grid_pos.x;
		cursor_pos->y = grid_pos.y;
		if (clicked)
		{
			return true;
		}

	}

	return false;

}
