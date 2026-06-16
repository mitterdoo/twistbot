from ctypes import *
from ctypes.wintypes import *
from struct import pack, unpack
from enum import Enum
import time
import math
import psutil
import os
import pypresence
import re
import pyautogui
import keyboard

class Permute():
    def __init__(self, *args):
        self.choices = args
    
    def __iter__(self):
        self.nums = [0] * len(self.choices)
        return self
    
    def __next__(self):
        end = len(self.nums)-1

        if self.nums[end] >= self.choices[end]:
            raise StopIteration
        ret = tuple(self.nums)
        self.nums[0] += 1
        for i in range(end):
            if self.nums[i] >= self.choices[i]:
                self.nums[i] = 0
                self.nums[i+1] += 1
            
        return ret


pid = None
for i in range(20):
	for p in psutil.process_iter():
		if 'popcapgame1' in p.name().lower():
			pid = p.pid
			break
	if pid:
		break
	time.sleep(0.2)
if not pid:
	raise ProcessLookupError("Couldn't find process!")
print('process found')

FindWindowA = windll.user32.FindWindowA
GetWindowRect = windll.user32.GetWindowRect

def click(x, y):
	wind = FindWindowA(None, b'Bejeweled Twist  1.0')
	rect = RECT()
	GetWindowRect(wind, byref(rect))

	x = rect.left + x
	y = rect.top + y + 26
	pyautogui.click(x, y, duration=0)

click(1,1)

OpenProcess = windll.kernel32.OpenProcess
ReadProcessMemory = windll.kernel32.ReadProcessMemory
WriteProcessMemory = windll.kernel32.WriteProcessMemory
CloseHandle = windll.kernel32.CloseHandle

PROCESS_ALL_ACCESS = 0x1F0FFF

STATIC_REALM_PTR_ADDR = 0x0085AEF4

processHandle = OpenProcess(PROCESS_ALL_ACCESS, False, pid)


def readProgramMemory(offset, count):
	buffer = c_char_p(b'\x01' * count)
	bytesRead = c_ulong(0)
	if not ReadProcessMemory(processHandle, offset, buffer, count, byref(bytesRead)):
		raise MemoryError(f'Unable to read from offset {hex(offset)} with {hex(count)} ({str(count)}) byte(s)')
	else:
		return buffer._objects

def writeProgramMemory(offset, data):
	buffer = c_char_p(data)
	count = len(data)
	bytesWritten = c_ulong(0)
	if not WriteProcessMemory(processHandle, offset, buffer, count, byref(bytesWritten)):
		raise MemoryError(f'Unable to write to offset {hex(offset)} with {hex(count)} ({str(count)}) byte(s)')
	else:
		return True

import random


BOARD_ADDR = 0

def readInt(where):
	return unpack('<I', readProgramMemory(where, 4))[0]
def readFloat(where):
	return unpack('<f', readProgramMemory(where, 4))[0]

def writeInt(where, what):
	writeProgramMemory(where, pack('<I', what))



GEM_RED = 0
GEM_WHITE = 1
GEM_GREEN = 2
GEM_YELLOW = 3
GEM_PURPLE = 4
GEM_ORANGE = 5
GEM_BLUE = 6
GEM_COAL = 7

GEM_FLAG_FRUIT = 0x0001
GEM_FLAG_LIGHTNING = 0x0008
GEM_FLAG_FLAME = 0x0010
GEM_FLAG_DOOM = 0x0100
GEM_FLAG_BOMB = 0x0200
GEM_FLAG_COAL = 0x0400

class Gem:
	def __init__(self, color, flags=None, x=None, y=None, pixelX=None, pixelY=None):

		if isinstance(color, Gem): # clone
			self.color = color.color
			self.flags = color.flags
			self.x = color.x
			self.pixelX = color.pixelX
			self.y = color.y
			self.pixelY = color.pixelY
			return

		assert flags != None
		assert x != None
		assert y != None

		self.color = color
		self.flags = flags
		self.x = x
		self.y = y
		self.pixelX = pixelX
		self.pixelY = pixelY
	
	def isA(self, a):
		return self.flags & a
	
	def move(self, x, y):
		self.x = x
		self.y = y
		return self



matchPattern = re.compile(r'((\w)\2{2,})')

def findMatches(gems, minimum=3):
	matches = [] # list containing tuples of matches
	currentMatch = []
	for gem in gems:
		if len(currentMatch) == 0 or currentMatch[0].color == gem.color and gem.color != GEM_COAL:
			currentMatch.append(gem)
		else:
			if len(currentMatch) >= minimum:
				matches.append(tuple(currentMatch))
			currentMatch = [gem]
	
	if len(currentMatch) >= minimum:
		matches.append(tuple(currentMatch))
	return matches

class Board:
	def __init__(self, bonus, curBonus = 0):
		self.gems = [[],[],[],[],[],[],[],[]]
		for x, y in Permute(8, 8):
			self[x].insert(y, Gem((x + y*8) % 7, 0, x, y))


		if isinstance(bonus, Board):
			other = bonus
			for x, y in Permute(8, 8):
				self[x][y] = Gem(other[x][y])
			self.bonus = other.bonus
			self.curBonus = other.curBonus
			return

		self.bonus = bonus # list
		self.curBonus = curBonus
		
	def __getitem__(self, item):
		return self.gems[item]
	
	def rotate(self, x, y):
		assert x < 7
		assert y < 7
		
		current = (self[x][y], self[x+1][y], self[x+1][y+1], self[x][y+1])

		self[x][y] = current[3].move(x, y)
		self[x+1][y] = current[0].move(x + 1, y)
		self[x+1][y+1] = current[1].move(x + 1, y + 1)
		self[x][y+1] = current[2].move(x, y + 1)

	def antiRotate(self, x, y):
		assert x < 7
		assert y < 7
		
		current = (self[x][y], self[x+1][y], self[x+1][y+1], self[x][y+1])

		self[x][y] = current[1].move(x, y)
		self[x+1][y] = current[2].move(x + 1, y)
		self[x+1][y+1] = current[3].move(x + 1, y + 1)
		self[x][y+1] = current[0].move(x, y + 1)


	def getScoreFromMove(self, x, y):

		self.rotate(x, y)

		score = 0

		strips = [[self[ox][oy] for ox in range(8)] for oy in range(8)] +   \
			[[self[ox][oy] for oy in range(8)] for ox in range(8)]

		for strip in strips:
			for match in findMatches(strip):
				if self.bonus != None and self.curBonus < 4 and match[0].color == self.bonus[self.curBonus]:
					score += 100

				for gem in match:
					if gem.isA(GEM_FLAG_FLAME) and not gem.isA(GEM_FLAG_LIGHTNING):
						score += 10
					elif gem.isA(GEM_FLAG_LIGHTNING) and not gem.isA(GEM_FLAG_FLAME):
						score += 25
					elif gem.isA(GEM_FLAG_LIGHTNING) and gem.isA(GEM_FLAG_FLAME):
						score += 50
				score += len(match)
	

		self.antiRotate(x, y)
		return score

OFFSET_LEVEL = 0x158
OFFSET_LEVEL_SCORE = 0x200
OFFSET_COMBO = 0x224
OFFSET_BOARD = 0x1338
OFFSET_CANMOVE = 0x22B8
OFFSET_REPLAY_ADDR = 0x2624
OFFSET_BONUS_ADDR = 0x264C
OFFSET_BONUS_IDX = 0x2660

def twist():
	while True:
		if keyboard.is_pressed('f11'):
			break
		time.sleep(0)
		# value at static addr is pointer to game's dynamic memory
		REALM_ADDR = readInt(STATIC_REALM_PTR_ADDR)
		if readInt(REALM_ADDR + OFFSET_CANMOVE) == 0 and readInt(REALM_ADDR + OFFSET_LEVEL_SCORE) > 0:
			continue


		# 0x1338 from realm is pointer to board
		BOARD_ADDR = readInt(REALM_ADDR + OFFSET_BOARD)

		bonusAddr = readInt(REALM_ADDR + OFFSET_BONUS_ADDR)
		bonus = None
		curBonus = None
		if bonusAddr != 0:
			bonus = (readInt(bonusAddr), readInt(bonusAddr + 0x4), readInt(bonusAddr + 0x8), readInt(bonusAddr + 0xC))
			curBonus = readInt(REALM_ADDR + OFFSET_BONUS_IDX)
		
		board = Board(bonus, curBonus)
		bail = False
		for x, y in Permute(8, 8):
			index = x + y*8
			gemAddr = readInt(BOARD_ADDR + 0x4 * index)

			if gemAddr == 0:
				bail = True
				break
			gem = Gem(
				readInt(gemAddr + 0x8),
				readInt(gemAddr + 0x84),
				x, y,
				readFloat(gemAddr + 0xC),
				readFloat(gemAddr + 0x10)
			)
			board[x][y] = gem

		if bail:
			continue
		curMove = (-1, -1, 0)

		matches = None
		for x, y in Permute(7, 7):
			score = board.getScoreFromMove(x, y)
			if score >= curMove[2]:
				curMove = (x, y, score)
		
		if curMove[0] == -1:
			print("no more moves")
			continue

		print(curMove)
		thisGem = board[curMove[0]][curMove[1]]

		pixelX = thisGem.pixelX + 85
		pixelY = thisGem.pixelY + 82

		if abs( pixelY % 1 ) > 0.01:
			continue

		click(pixelX, pixelY)


REALM_ADDR = readInt(STATIC_REALM_PTR_ADDR)
BOARD_ADDR = readInt(REALM_ADDR + OFFSET_BOARD)
def coal(x, y):
	index = x + y * 8
	gem = readInt(BOARD_ADDR + index * 0x4)
	writeInt(gem + 0x8, 7)
	writeInt(gem + 0x84, GEM_FLAG_COAL)


twist()


#board contains 4-byte pointers to gems

'''


1a9424a4

PIECE:

			+Offset (Size)
Piece Type: +0x8 (0x1)
			RED		0		000
			WHITE	1		001
			GREEN	2		010
			YELLOW	3		011
			PURPLE	4		100
			ORANGE	5		101
			BLUE	6		110
			COAL	7		111


GEM BITFLAG 0x84
	0	1						Fruit
	1	2						Skull
	2	4						Locked
	3	8						Lightning
	4	16						Flame
	5	32						(can't move)
	6	64						Ice
	7	128						Angel Gem
	8	256						Doom Gem
	9	512						Bomb Gem
	10	1024					Coal
	11	2048				<unused>
	12	4096					Purple sparklies
	13	8192					


'''


