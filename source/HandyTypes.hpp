#pragma once
#include <stdexcept>

typedef struct Vector2 {
	int x;
	int y;

	friend inline bool operator==(const Vector2& a, const Vector2& b)
	{
		return memcmp((void*)&a, (void*)&b, sizeof(Vector2)) == 0;
	}

} Vector2;

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

