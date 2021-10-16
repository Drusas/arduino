#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include <stdio.h>

template<class T>
class CircularBuffer {

    size_t bufSize, head, tail;
    bool full;
    T *buffer;

    void advancePointer() {
        if(isFull())
        {
            tail = (tail + 1) % bufSize;
        }

        head = (head + 1) % bufSize;

        // We mark full because we will advance tail on the next time around
        full = (head == tail);
    }

    void retreatPointer() {
        full = false;
	    tail = (tail + 1) % bufSize;
    }

public:
    CircularBuffer(T arr[], int size) :
    bufSize(size),
    head(0),
    tail(0),
    full(false)
    {
        buffer = arr;
    }

    bool isNotFull() {
        return !isFull();
    }

    bool isFull() {
        return full;
    }

    bool isNotEmpty() {
        return !isEmpty();
    }

    bool isEmpty() {
        return (!isFull() && (head == tail));
    }

    void addElement(T element) {
        buffer[head] = element;
	    advancePointer();
    }

    T getElement() {
        T result = T();
        if(!isEmpty())
        {
            result = buffer[tail];
            retreatPointer();
        }
        return result;
    }

    size_t getCapacity() {
        return bufSize;
    }

    size_t getSize() {
        size_t size = bufSize;

		if(!isFull())
		{
			if(head >= tail)
			{
				size = head - tail;
			}
			else
			{
				size = bufSize + head - tail;
			}
		}

		return size;
    }


};
#endif