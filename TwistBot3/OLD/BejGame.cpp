#include "BejGame.h"

BejGame::BejGame()
{
	//Finding Proccess

	hWnd = FindWindowA(NULL, "Bejeweled Twist  1.0");

	if (!hWnd)
	{
		throw new std::runtime_error("Could not find Game window");
	}

	DWORD pID = NULL;
	GetWindowThreadProcessId(hWnd, &pID);

	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pID);

	if (!hProc)
	{
		throw new std::runtime_error("Process could not be opened!");
	}

	SetForegroundWindow(hWnd);

}

BejGame::~BejGame()
{

}

void BejGame::ReadMemory(SIZE_T address, SIZE_T count, void* buffer)
{
	SIZE_T bytesRead = 0;
	if (!ReadProcessMemory(hProc, (LPCVOID)address, buffer, count, &bytesRead))
	{
		throw new std::runtime_error("Access violation on read!");
	}
}

void BejGame::WriteMemory(SIZE_T address, SIZE_T count, void* buffer)
{
	SIZE_T bytesWritten = 0;
	if (!WriteProcessMemory(hProc, (LPVOID)address, buffer, count, &bytesWritten))
	{
		throw new std::runtime_error("Access violation on write!");
	}
}

char BejGame::ReadChar(SIZE_T address)
{
	char out = 0;
	SIZE_T bytesRead;
	if (!ReadProcessMemory(hProc, (LPCVOID)address, &out, sizeof(char), &bytesRead))
	{
		throw new std::runtime_error("Access violation on read!");
	}
	return out;
}

uint32_t BejGame::ReadUInt(SIZE_T address)
{
	uint32_t out = 0;
	SIZE_T bytesRead;
	if (!ReadProcessMemory(hProc, (LPCVOID)address, &out, sizeof(uint32_t), &bytesRead))
	{
		throw new std::runtime_error("Access violation on read!");
	}
	return out;
}

float BejGame::ReadFloat(SIZE_T address)
{
	float out = 0;
	SIZE_T bytesRead;
	if (!ReadProcessMemory(hProc, (LPCVOID)address, &out, sizeof(float), &bytesRead))
	{
		throw new std::runtime_error("Access violation on read!");
	}
	return out;
}

void BejGame::WriteUInt(SIZE_T address, uint32_t value)
{
	WriteMemory(address, sizeof(uint32_t), (void*)&value);
}

void BejGame::SetGemFlag(Vector2 pos, GemFlags flag)
{
	int index = pos.x + pos.y * 8;
	SIZE_T boardAddr = (SIZE_T)ReadUInt(MEM_STATIC_BOARD);
	SIZE_T piecesAddr = (SIZE_T)ReadUInt(boardAddr + MEM_OFFSET_BOARD);
	SIZE_T gem = (SIZE_T)ReadUInt(piecesAddr + index * 0x4);

	WriteUInt(gem + 0x84, (int)flag);
}

void BejGame::SetGemColor(Vector2 pos, GemColor color)
{
	int index = pos.x + pos.y * 8;
	SIZE_T boardAddr = (SIZE_T)ReadUInt(MEM_STATIC_BOARD);
	SIZE_T piecesAddr = (SIZE_T)ReadUInt(boardAddr + MEM_OFFSET_BOARD);
	SIZE_T gem = (SIZE_T)ReadUInt(piecesAddr + index * 0x4);

	WriteUInt(gem + 0x8, (int)color);
}


void BejGame::Click(Vector2 pos)
{

	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);

	RECT screenSize = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};

	POINT newPos = {
		(65535 * ((LONG)pos.x + 8 + windowRect.left)) / screenSize.right,
		(65535 * ((LONG)pos.y + 31 + windowRect.top)) / screenSize.bottom
	};

	INPUT inputs[2] = {};
	ZeroMemory(inputs, sizeof(inputs));

	inputs[0].type = INPUT_MOUSE;
	inputs[0].mi.dx = newPos.x;
	inputs[0].mi.dy = newPos.y;
	inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN;

	inputs[1].type = INPUT_MOUSE;
	inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
	if (uSent != ARRAYSIZE(inputs))
	{
		throw new std::runtime_error("Array size does not match!");
	}
}

Board* BejGame::FetchBoard()
{

	Board* board = new Board();
	SIZE_T boardAddr = (SIZE_T)ReadUInt(MEM_STATIC_BOARD);
	SIZE_T piecesAddr = (SIZE_T)ReadUInt(boardAddr + MEM_OFFSET_BOARD);
	SIZE_T bonusAddr = (SIZE_T)ReadUInt(boardAddr + MEM_OFFSET_BONUS_ADDR);
	
	ReadMemory(bonusAddr, sizeof(uint32_t) * 4, (void*)board->bonus.colors);
	board->bonus.fruitBonus = ReadUInt(boardAddr + MEM_OFFSET_BONUS_MEGA) >= 3;
	board->bonus.index = (int)ReadUInt(boardAddr + MEM_OFFSET_BONUS_IDX);
	//board->score.multiplier = (int)ReadUInt(boardAddr + MEM_OFFSET_MULTIPLIER);

	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			int index = x + y * 8;
			SIZE_T gemAddr = (SIZE_T)ReadUInt(piecesAddr + 0x4 * index);
			
			if (gemAddr == 0)
			{
				delete board;
				return NULL;
			}

			Gem* gem = &board->gems[x][y];
			gem->color = (GemColor)ReadUInt(gemAddr + MEM_OFFSET_GEM_COLOR);
			gem->flags = (GemFlags)ReadUInt(gemAddr + MEM_OFFSET_GEM_FLAGS);
			gem->pos = {x, y};
			gem->pixelPos = {
				(int)ReadFloat(gemAddr + MEM_OFFSET_GEM_SCREEN_X),
				(int)ReadFloat(gemAddr + MEM_OFFSET_GEM_SCREEN_Y)
			};

		}
	}

	return board;

}

Vector2 BejGame::GetCursor()
{
	SIZE_T boardAddr = (SIZE_T)ReadUInt(MEM_STATIC_BOARD);
	return {
		(int)ReadUInt(boardAddr + MEM_OFFSET_CURSOR_X),
		(int)ReadUInt(boardAddr + MEM_OFFSET_CURSOR_Y)
	};
}


