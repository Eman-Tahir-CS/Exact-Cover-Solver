#include <iostream>
#include <string>
#include "Stack.h"

// Sample struct to verify stack operations with non-primitive data types
struct TestFrame {
    int row;
    int col;
    int digit;
};

// Simple test helper to report pass/fail
void assertCondition(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << "[PASS] " << testName << std::endl;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
        std::exit(1);
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "       RUNNING TEST: Stack<T>           " << std::endl;
    std::cout << "========================================" << std::endl;

    // Test 1: Stack initialization and empty state
    Stack<int> intStack(4);
    assertCondition(intStack.isEmpty(), "Initial stack is empty");
    assertCondition(intStack.size() == 0, "Initial stack size is 0");
    assertCondition(intStack.capacity() == 4, "Initial stack capacity is 4");

    // Test 2: Underflow protection
    int dummyValue = -1;
    bool popSuccess = intStack.pop(dummyValue);
    assertCondition(!popSuccess, "Popping an empty stack returns false");
    bool peekSuccess = intStack.peek(dummyValue);
    assertCondition(!peekSuccess, "Peeking an empty stack returns false");

    // Test 3: Push operations and peek
    intStack.push(10);
    intStack.push(20);
    intStack.push(30);
    assertCondition(!intStack.isEmpty(), "Stack is not empty after pushes");
    assertCondition(intStack.size() == 3, "Stack size is 3 after 3 pushes");

    int peekedValue = 0;
    assertCondition(intStack.peek(peekedValue) && peekedValue == 30, "Peek returns top element 30 without popping");
    assertCondition(intStack.size() == 3, "Size remains 3 after peek");

    // Test 4: Dynamic array resizing (doubling capacity)
    // Initial capacity was 4; pushing 2 more elements will exceed 4 and trigger resize to 8
    intStack.push(40);
    intStack.push(50);
    assertCondition(intStack.size() == 5, "Size is 5 after 5 pushes");
    assertCondition(intStack.capacity() == 8, "Capacity doubled to 8 upon exceeding initial capacity");

    // Test 5: LIFO order pop verification
    int poppedValue = 0;
    assertCondition(intStack.pop(poppedValue) && poppedValue == 50, "Popped 50 (LIFO)");
    assertCondition(intStack.pop(poppedValue) && poppedValue == 40, "Popped 40 (LIFO)");
    assertCondition(intStack.pop(poppedValue) && poppedValue == 30, "Popped 30 (LIFO)");
    assertCondition(intStack.pop(poppedValue) && poppedValue == 20, "Popped 20 (LIFO)");
    assertCondition(intStack.pop(poppedValue) && poppedValue == 10, "Popped 10 (LIFO)");
    assertCondition(intStack.isEmpty(), "Stack is empty after popping all elements");

    // Test 6: Clear method
    intStack.push(100);
    intStack.push(200);
    assertCondition(intStack.size() == 2, "Stack size is 2 before clear");
    intStack.clear();
    assertCondition(intStack.isEmpty(), "Stack is empty after clear()");
    assertCondition(intStack.size() == 0, "Stack size is 0 after clear()");

    // Test 7: Custom struct compatibility (BacktrackFrame simulation)
    Stack<TestFrame> frameStack(2);
    TestFrame frame1;
    frame1.row = 2;
    frame1.col = 5;
    frame1.digit = 9;

    TestFrame frame2;
    frame2.row = 8;
    frame2.col = 1;
    frame2.digit = 4;

    frameStack.push(frame1);
    frameStack.push(frame2);

    TestFrame poppedFrame;
    assertCondition(frameStack.pop(poppedFrame) && poppedFrame.row == 8 && poppedFrame.digit == 4, 
                    "Struct frame popped correctly with matching fields");
    assertCondition(frameStack.pop(poppedFrame) && poppedFrame.row == 2 && poppedFrame.col == 5, 
                    "Second struct frame popped correctly");
    assertCondition(frameStack.isEmpty(), "Struct stack is empty");

    // Test 8: Large scale stress test (100,000 pushes and pops)
    const int largeScaleCount = 100000;
    for (int counter = 1; counter <= largeScaleCount; counter++) {
        intStack.push(counter);
    }
    assertCondition(intStack.size() == largeScaleCount, "Stress test: Successfully pushed 100,000 items");

    bool stressTestPassed = true;
    for (int counter = largeScaleCount; counter >= 1; counter--) {
        int retrievedValue = 0;
        if (!intStack.pop(retrievedValue) || retrievedValue != counter) {
            stressTestPassed = false;
            break;
        }
    }
    assertCondition(stressTestPassed, "Stress test: Successfully popped all 100,000 items in strict LIFO order");
    assertCondition(intStack.isEmpty(), "Stack is empty after stress test");

    std::cout << "========================================" << std::endl;
    std::cout << "   ALL STACK UNIT TESTS PASSED (8/8)    " << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
