#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <cstdlib>

// ============================================================================
// Class: Stack<T>
// Description: A custom generic Last-In-First-Out (LIFO) stack data structure
//              implemented using a dynamically resizing array.
// Purpose: Used by iterative backtracking and Algorithm X exact cover solvers
//          to store search frames without relying on system call stack recursion.
// ============================================================================
template <typename T>
class Stack {
private:
    // Pointer to the dynamically allocated internal array storing elements
    T* elementsArray;

    // Maximum number of elements the current array can hold
    int currentCapacity;

    // Current number of valid elements stored in the stack
    int currentElementCount;

    // Resizes the internal array when capacity is reached.
    // Doubles the capacity, copies existing elements, and frees old memory.
    void resize(int newCapacity) {
        T* newArray = new T[newCapacity];

        // Copy all existing elements to the newly allocated array
        for (int index = 0; index < currentElementCount; index++) {
            newArray[index] = elementsArray[index];
        }

        // Delete the old internal array to prevent memory leaks
        delete[] elementsArray;

        // Point internal array pointer to new array and update capacity
        elementsArray = newArray;
        currentCapacity = newCapacity;
    }

    // Disable copy constructor by making it private.
    // This prevents accidental shallow copies that cause double-free bugs.
    Stack(const Stack<T>& other);

    // Disable assignment operator by making it private.
    Stack<T>& operator=(const Stack<T>& other);

public:
    // Constructor: Allocates internal array with a default or specified capacity.
    Stack(int initialCapacity = 16) {
        if (initialCapacity <= 0) {
            initialCapacity = 16;
        }
        currentCapacity = initialCapacity;
        currentElementCount = 0;
        elementsArray = new T[currentCapacity];
    }

    // Destructor: Frees dynamically allocated memory to prevent leaks.
    ~Stack() {
        if (elementsArray != nullptr) {
            delete[] elementsArray;
            elementsArray = nullptr;
        }
        currentCapacity = 0;
        currentElementCount = 0;
    }

    // Pushes a new element onto the top of the stack.
    // If the internal array is full, it doubles the capacity.
    void push(const T& value) {
        if (currentElementCount == currentCapacity) {
            resize(currentCapacity * 2);
        }
        elementsArray[currentElementCount] = value;
        currentElementCount++;
    }

    // Pops the top element from the stack into outValue.
    // Returns true if an element was popped successfully, false if the stack was empty.
    bool pop(T& outValue) {
        if (isEmpty()) {
            return false;
        }
        currentElementCount--;
        outValue = elementsArray[currentElementCount];
        return true;
    }

    // Peeks at the top element without removing it, storing it into outValue.
    // Returns true if an element was read successfully, false if stack was empty.
    bool peek(T& outValue) const {
        if (isEmpty()) {
            return false;
        }
        outValue = elementsArray[currentElementCount - 1];
        return true;
    }

    // Returns a copy of the top element directly.
    // Safe fallback: prints error and exits if stack is empty.
    T peek() const {
        if (isEmpty()) {
            std::cout << "Error: Attempted to peek an empty stack!" << std::endl;
            std::exit(1);
        }
        return elementsArray[currentElementCount - 1];
    }

    // Checks whether the stack contains zero elements.
    bool isEmpty() const {
        return (currentElementCount == 0);
    }

    // Returns the current number of elements stored in the stack.
    int size() const {
        return currentElementCount;
    }

    // Returns the current allocated capacity of the internal array.
    int capacity() const {
        return currentCapacity;
    }

    // Returns element at index (0 <= index < size) without modifying stack
    const T& get(int index) const {
        if (index < 0 || index >= currentElementCount) {
            std::cout << "Error: Stack index out of bounds!" << std::endl;
            std::exit(1);
        }
        return elementsArray[index];
    }

    // Clears the stack by resetting the element counter to zero.
    // Kept capacity avoids unnecessary reallocations if reused.
    void clear() {
        currentElementCount = 0;
    }
};

#endif // STACK_H
