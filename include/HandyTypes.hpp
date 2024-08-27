#pragma once
#include <stdexcept>
#include <cstring>

typedef struct Vec2 {
	int x;
	int y;

	friend inline bool operator==(const Vec2& a, const Vec2& b)
	{
		return memcmp((void*)&a, (void*)&b, sizeof(Vec2)) == 0;
	}

} Vec2;

template <typename T> class ArrayPtr
{
private:
	T* ptr;
	int size;
	int max_size;

public:
	ArrayPtr(T* pointer, int maxSize, int size=0);

	inline void Push(T item)
	{
		if (size >= max_size - 1)
			throw new std::runtime_error("Array at max size!");
		ptr[size++] = item;
	}

	T operator[](int index) { return ptr[index]; };
};

template <typename T> ArrayPtr<T>::ArrayPtr(T* pointer, int maxSize, int size)
{
	ptr = pointer;
	max_size = maxSize;
	this->size = size;
}

