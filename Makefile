CC = g++
CFLAGS = 

INCLUDE = -Iinclude -Iextern/raylib/include
LIB = -LC:/raylib/lib
LINK = -lraylib -lopengl32 -lgdi32 -lwinmm
SRC = $(wildcard ./source/*.c) $(wildcard ./source/*.cpp)

TARGET = binaries/twist

dev:
	$(CC) $(CFLAGS) -g $(INCLUDE) $(LIB) $(SRC) $(LINK) -o $(TARGET)

release:
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB) $(SRC) $(LINK) -o $(TARGET)

clean:
	rm $(TARGET)