// ============================================================================
// File: src/main.cpp
// Project: P26 - Exact Cover Puzzle Solver (Group G08)
// Purpose: PROGRESS DEMO menu (17 Nov). Shows the three data structures that
//          are finished so far:
//            1. Stack<T>        (dynamic array stack)
//            2. DancingLinks    (circular doubly linked lists, cover/uncover)
//            3. HashTable       (djb2, separate chaining, rehashing)
//          It also links Stack + DancingLinks by solving Knuth's exact cover
//          example with an ITERATIVE Algorithm X (no recursion).
// Allowed headers only: iostream, fstream, string, cmath, ctime, cstdlib.
// Build: g++ -std=c++17 -Wall -Iinclude src/*.cpp -o solver
// ============================================================================
#include <iostream>
#include <string>
#include <cstdlib>

#include "Stack.h"
#include "DancingLinks.h"
#include "HashTable.h"
#include "PuzzleRecord.h"

// ----------------------------------------------------------------------------
// Input helpers (validated; never crash on bad input; stop cleanly on EOF)
// ----------------------------------------------------------------------------
static bool inputEnded = false;

// Removes spaces, tabs and carriage returns from both ends
static std::string trimString(const std::string& input) {
    int start = 0;
    int end = (int)input.length() - 1;
    while (start <= end && (input[start] == ' ' || input[start] == '\t' || input[start] == '\r' || input[start] == '\n')) {
        start++;
    }
    while (end >= start && (input[end] == ' ' || input[end] == '\t' || input[end] == '\r' || input[end] == '\n')) {
        end--;
    }
    if (start > end) {
        return "";
    }
    return input.substr(start, end - start + 1);
}

// Prints a prompt and reads one trimmed line. Returns false when input ends.
static bool readLine(const std::string& prompt, std::string& outLine) {
    std::cout << prompt;
    std::string raw;
    if (!std::getline(std::cin, raw)) {
        inputEnded = true;
        outLine = "";
        return false;
    }
    outLine = trimString(raw);
    return true;
}

// Reads a non-empty line, asking again if the user types nothing
static bool readNonEmpty(const std::string& prompt, std::string& outLine) {
    while (true) {
        if (!readLine(prompt, outLine)) {
            return false;
        }
        if (!outLine.empty()) {
            return true;
        }
        std::cout << "  Input cannot be empty. Try again.\n";
    }
}

// Reads a whole number (digits only, up to 9 digits) inside [minValue, maxValue]
static bool readInt(const std::string& prompt, int minValue, int maxValue, int& outValue) {
    while (true) {
        std::string text;
        if (!readLine(prompt, text)) {
            return false;
        }
        if (text.empty()) {
            std::cout << "  Input cannot be empty. Try again.\n";
            continue;
        }
        bool onlyDigits = true;
        for (int i = 0; i < (int)text.length(); i++) {
            if (text[i] < '0' || text[i] > '9') {
                onlyDigits = false;
            }
        }
        if (!onlyDigits || text.length() > 9) {
            std::cout << "  Please enter a whole number (digits only).\n";
            continue;
        }
        int value = 0;
        for (int i = 0; i < (int)text.length(); i++) {
            value = value * 10 + (text[i] - '0');
        }
        if (value < minValue || value > maxValue) {
            std::cout << "  Number must be between " << minValue << " and " << maxValue << ".\n";
            continue;
        }
        outValue = value;
        return true;
    }
}

// ============================================================================
// DEMO 1: Stack<T>
// ============================================================================
static void stackDemo() {
    Stack<int> stack(2); // tiny starting capacity so resize() is visible
    std::cout << "\n=== STACK DEMO (Stack<int>, starting capacity 2) ===\n";

    while (true) {
        std::cout << "\n  [Stack] size=" << stack.size() << "  capacity=" << stack.capacity() << "  top-to-bottom: ";
        if (stack.isEmpty()) {
            std::cout << "(empty)";
        } else {
            for (int i = stack.size() - 1; i >= 0; i--) {
                std::cout << stack.get(i) << " ";
            }
        }
        std::cout << "\n  1. Push a number\n  2. Pop\n  3. Peek\n  4. Push 1..20 (shows automatic resize)\n  5. Clear\n  0. Back\n";
        int choice;
        if (!readInt("  Choice: ", 0, 5, choice)) {
            return;
        }
        if (choice == 0) {
            return;
        }
        if (choice == 1) {
            int value;
            if (!readInt("  Number to push (0-999999): ", 0, 999999, value)) {
                return;
            }
            stack.push(value);
            std::cout << "  Pushed " << value << ".\n";
        } else if (choice == 2) {
            int value;
            if (stack.pop(value)) {
                std::cout << "  Popped " << value << ".\n";
            } else {
                std::cout << "  Stack is empty: nothing to pop.\n";
            }
        } else if (choice == 3) {
            int value;
            if (stack.peek(value)) {
                std::cout << "  Top element is " << value << ".\n";
            } else {
                std::cout << "  Stack is empty: nothing to peek.\n";
            }
        } else if (choice == 4) {
            int oldCapacity = stack.capacity();
            for (int i = 1; i <= 20; i++) {
                stack.push(i);
            }
            std::cout << "  Pushed 1..20. Capacity grew from " << oldCapacity << " to " << stack.capacity() << ".\n";
        } else if (choice == 5) {
            stack.clear();
            std::cout << "  Stack cleared.\n";
        }
    }
}

// ============================================================================
// DEMO 2: Dancing Links (Knuth's example) + iterative Algorithm X
// ============================================================================

// Builds Knuth's 7-column, 6-row exact cover example from his paper.
//   A: 1 4 7     B: 1 4     C: 4 5 7     D: 3 5 6     E: 2 3 6 7     F: 2 7
// The only exact cover is rows B, D and F.
static void buildKnuthExample(DancingLinks& dlx) {
    ColumnNode* col[8];
    for (int i = 1; i <= 7; i++) {
        col[i] = dlx.addColumn(std::to_string(i), true);
    }
    ColumnNode* rowA[3] = {col[1], col[4], col[7]};
    ColumnNode* rowB[2] = {col[1], col[4]};
    ColumnNode* rowC[3] = {col[4], col[5], col[7]};
    ColumnNode* rowD[3] = {col[3], col[5], col[6]};
    ColumnNode* rowE[4] = {col[2], col[3], col[6], col[7]};
    ColumnNode* rowF[2] = {col[2], col[7]};
    dlx.addRow(0, rowA, 3);
    dlx.addRow(1, rowB, 2);
    dlx.addRow(2, rowC, 3);
    dlx.addRow(3, rowD, 3);
    dlx.addRow(4, rowE, 4);
    dlx.addRow(5, rowF, 2);
}

// Prints every column still linked to the root, with its current size
static void printActiveColumns(const DancingLinks& dlx) {
    ColumnNode* root = dlx.getRoot();
    std::cout << "    Active columns (name:size): ";
    if (root->right == root) {
        std::cout << "(none: all covered)";
    }
    for (DLXNode* node = root->right; node != root; node = node->right) {
        ColumnNode* column = (ColumnNode*)node;
        std::cout << column->name << ":" << column->size << "  ";
    }
    std::cout << "\n";
}

// One stack frame of the iterative Algorithm X search
struct DLXFrame {
    ColumnNode* column;   // column chosen at this depth
    DLXNode* row;         // current candidate row in that column
    bool applied;         // true once this row's other columns were covered
};

// Iterative Algorithm X on the Knuth example. Uses our own Stack, no recursion.
static void solveKnuthIteratively() {
    DancingLinks dlx;
    buildKnuthExample(dlx);
    ColumnNode* root = dlx.getRoot();

    Stack<DLXFrame> stack;
    int solutionCount = 0;
    int stepCount = 0;

    std::cout << "\n  Searching (smallest-column heuristic)...\n";

    if (root->right != root) {
        ColumnNode* first = dlx.chooseSmallestColumn();
        if (first->size > 0) {
            dlx.cover(first);
            DLXFrame frame = {first, first->down, false};
            stack.push(frame);
        }
    }

    while (!stack.isEmpty()) {
        DLXFrame frame;
        stack.pop(frame);

        // Undo the previously applied row of this frame, then move to the next row
        if (frame.applied) {
            for (DLXNode* j = frame.row->left; j != frame.row; j = j->left) {
                dlx.uncover(j->column);
            }
            frame.row = frame.row->down;
            frame.applied = false;
        }

        // All rows of this column tried: restore the column and backtrack
        if (frame.row == frame.column) {
            dlx.uncover(frame.column);
            continue;
        }

        // Try this row: cover every other column it touches
        stepCount++;
        for (DLXNode* j = frame.row->right; j != frame.row; j = j->right) {
            dlx.cover(j->column);
        }
        frame.applied = true;
        stack.push(frame);

        if (root->right == root) {
            // Every primary column is covered: this is an exact cover
            solutionCount++;
            std::cout << "  Solution " << solutionCount << " found: rows ";
            for (int i = 0; i < stack.size(); i++) {
                if (stack.get(i).applied) {
                    std::cout << (char)('A' + stack.get(i).row->rowId) << " ";
                }
            }
            std::cout << "\n";
            continue; // next loop pass undoes this row and keeps searching
        }

        ColumnNode* next = dlx.chooseSmallestColumn();
        if (next->size == 0) {
            continue; // dead end: next pass backtracks
        }
        dlx.cover(next);
        DLXFrame nextFrame = {next, next->down, false};
        stack.push(nextFrame);
    }

    std::cout << "  Search finished. Solutions: " << solutionCount << ", steps (rows tried): " << stepCount << ".\n";
    if (solutionCount == 0) {
        std::cout << "  No exact cover exists.\n";
    }
    std::cout << "  After the search every column is restored:\n";
    printActiveColumns(dlx);
}

static void dancingLinksDemo() {
    std::cout << "\n=== DANCING LINKS DEMO (Knuth's example: 7 columns, 6 rows) ===\n";
    std::cout << "  Rows: A={1,4,7} B={1,4} C={4,5,7} D={3,5,6} E={2,3,6,7} F={2,7}\n";

    while (true) {
        std::cout << "\n  1. Show the matrix (column sizes)\n"
                  << "  2. Cover / uncover demo on a chosen column\n"
                  << "  3. Solve with iterative Algorithm X (uses Stack)\n"
                  << "  0. Back\n";
        int choice;
        if (!readInt("  Choice: ", 0, 3, choice)) {
            return;
        }
        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            DancingLinks dlx;
            buildKnuthExample(dlx);
            printActiveColumns(dlx);
            ColumnNode* smallest = dlx.chooseSmallestColumn();
            std::cout << "    chooseSmallestColumn() picks column " << smallest->name
                      << " (size " << smallest->size << ").\n";
        } else if (choice == 2) {
            int columnNumber;
            if (!readInt("  Column to cover (1-7): ", 1, 7, columnNumber)) {
                return;
            }
            DancingLinks dlx;
            buildKnuthExample(dlx);
            ColumnNode* column = dlx.getColumnByName(std::to_string(columnNumber));
            if (column == nullptr) {
                std::cout << "  Column not found.\n";
                continue;
            }
            std::cout << "  Before cover:\n";
            printActiveColumns(dlx);
            dlx.cover(column);
            std::cout << "  After cover(" << columnNumber << ") (column and its conflicting rows unlinked):\n";
            printActiveColumns(dlx);
            dlx.uncover(column);
            std::cout << "  After uncover(" << columnNumber << ") (everything restored in reverse order):\n";
            printActiveColumns(dlx);
        } else if (choice == 3) {
            solveKnuthIteratively();
        }
    }
}

// ============================================================================
// DEMO 3: HashTable
// ============================================================================
static const int MAX_RECORDS = 5000;

// Makes IDs like SUD0007 (prefix + 4 digits)
static std::string makeId(const std::string& prefix, int number) {
    std::string digits = std::to_string(number);
    while (digits.length() < 4) {
        digits = "0" + digits;
    }
    return prefix + digits;
}

static void printTableStats(const HashTable& table) {
    std::cout << "  Records: " << table.size()
              << "   Buckets: " << table.getBucketCount()
              << "   Load factor: " << table.getLoadFactor() << "\n";
}

static void hashTableDemo() {
    std::cout << "\n=== HASH TABLE DEMO (djb2, separate chaining, rehash at load > 0.75) ===\n";

    HashTable table(8, 0.75f);                  // small start so rehash is visible
    PuzzleRecord** records = new PuzzleRecord*[MAX_RECORDS];
    int recordCount = 0;                        // this demo owns the records

    while (true) {
        std::cout << "\n";
        printTableStats(table);
        std::cout << "  1. Insert a record\n"
                  << "  2. Search by ID\n"
                  << "  3. Delete by ID\n"
                  << "  4. Bulk insert 1,200 generated records (shows rehashing)\n"
                  << "  5. Test duplicate ID rejection\n"
                  << "  0. Back\n";
        int choice;
        if (!readInt("  Choice: ", 0, 5, choice)) {
            break;
        }
        if (choice == 0) {
            break;
        }

        if (choice == 1) {
            if (recordCount >= MAX_RECORDS) {
                std::cout << "  Demo is full (" << MAX_RECORDS << " records).\n";
                continue;
            }
            std::string id, type, data;
            if (!readNonEmpty("  Puzzle ID (e.g. SUD0001): ", id)) {
                break;
            }
            if (!readNonEmpty("  Type (SUDOKU or NQUEENS): ", type)) {
                break;
            }
            if (!readNonEmpty("  Puzzle data: ", data)) {
                break;
            }
            PuzzleRecord* record = new PuzzleRecord;
            record->id = id;
            record->type = type;
            record->difficultyOrN = "-";
            record->puzzleData = data;
            if (table.insert(id, record)) {
                records[recordCount] = record;
                recordCount++;
                std::cout << "  Inserted " << id << ".\n";
            } else {
                delete record; // duplicate: table did not take ownership
                std::cout << "  Duplicate ID rejected: " << id << " already exists.\n";
            }
        } else if (choice == 2) {
            std::string id;
            if (!readNonEmpty("  ID to search: ", id)) {
                break;
            }
            PuzzleRecord* found = table.find(id);
            if (found == nullptr) {
                std::cout << "  Not found: " << id << "\n";
            } else {
                std::cout << "  Found -> ID: " << found->id << "  Type: " << found->type
                          << "  Data: " << found->puzzleData << "\n";
            }
        } else if (choice == 3) {
            std::string id;
            if (!readNonEmpty("  ID to delete: ", id)) {
                break;
            }
            PuzzleRecord* found = table.find(id);
            if (found == nullptr) {
                std::cout << "  Not found: " << id << "\n";
            } else {
                table.remove(id);
                // free the record and fill the gap with the last one
                for (int i = 0; i < recordCount; i++) {
                    if (records[i] == found) {
                        delete records[i];
                        records[i] = records[recordCount - 1];
                        recordCount--;
                        break;
                    }
                }
                std::cout << "  Deleted " << id << ".\n";
            }
        } else if (choice == 4) {
            int added = 0;
            int oldBuckets = table.getBucketCount();
            for (int i = 1; i <= 1200 && recordCount < MAX_RECORDS; i++) {
                std::string id = makeId("SUD", i);
                if (table.find(id) != nullptr) {
                    continue; // already present (bulk insert run twice)
                }
                PuzzleRecord* record = new PuzzleRecord;
                record->id = id;
                record->type = "SUDOKU";
                record->difficultyOrN = "EASY";
                record->puzzleData = "generated";
                table.insert(id, record);
                records[recordCount] = record;
                recordCount++;
                added++;
            }
            std::cout << "  Added " << added << " records. Buckets grew from " << oldBuckets
                      << " to " << table.getBucketCount() << ".\n";
            PuzzleRecord* sample = table.find(makeId("SUD", 777));
            if (sample != nullptr) {
                std::cout << "  Spot check: SUD0777 found.\n";
            }
        } else if (choice == 5) {
            std::string id;
            if (!readNonEmpty("  Existing ID to insert again: ", id)) {
                break;
            }
            if (table.find(id) == nullptr) {
                std::cout << "  That ID does not exist yet. Insert it first (option 1).\n";
                continue;
            }
            PuzzleRecord temp;
            temp.id = id;
            if (!table.insert(id, &temp)) {
                std::cout << "  Duplicate correctly rejected.\n";
            } else {
                std::cout << "  ERROR: duplicate was accepted!\n";
                table.remove(id);
            }
        }
    }

    // Free every record this demo created (HashTable only owns its nodes)
    for (int i = 0; i < recordCount; i++) {
        delete records[i];
    }
    delete[] records;
}

// ============================================================================
// Main menu
// ============================================================================
int main() {
    std::cout << "==============================================\n"
              << "  P26: Exact Cover Puzzle Solver (Group G08)\n"
              << "  PROGRESS DEMO: Stack, Dancing Links, HashTable\n"
              << "==============================================\n";

    while (!inputEnded) {
        std::cout << "\n========== MAIN MENU ==========\n"
                  << "  1. Stack demo\n"
                  << "  2. Dancing Links demo (cover/uncover + Algorithm X)\n"
                  << "  3. Hash Table demo\n"
                  << "  0. Exit\n";
        int choice;
        if (!readInt("Choice: ", 0, 3, choice)) {
            break;
        }
        if (choice == 0) {
            break;
        }
        if (choice == 1) {
            stackDemo();
        } else if (choice == 2) {
            dancingLinksDemo();
        } else if (choice == 3) {
            hashTableDemo();
        }
    }

    std::cout << "\nGoodbye.\n";
    return 0;
}
