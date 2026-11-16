// ============================================================================
// File: tests/test_hashtable.cpp
// Purpose: Standalone tests for the custom HashTable (djb2, separate chaining).
// Covers: insert, search, delete, duplicate rejection, collision handling,
//         rehashing, load factor, clear, and re-insert after delete.
// Build:  g++ -std=c++17 -Wall -Iinclude tests/test_hashtable.cpp src/HashTable.cpp -o test_hashtable
// ============================================================================
#include <iostream>
#include <string>
#include <cstdlib>

#include "HashTable.h"
#include "PuzzleRecord.h"

static int testsRun = 0;
static int testsPassed = 0;

void check(bool condition, const std::string& testName) {
    testsRun++;
    if (condition) {
        testsPassed++;
        std::cout << " [PASS " << testsPassed << "] " << testName << std::endl;
    } else {
        std::cout << " [FAIL] " << testName << std::endl;
        std::exit(1);
    }
}

// Builds an ID like "KEY0007" from a number
std::string makeKey(int number) {
    std::string digits = std::to_string(number);
    while (digits.length() < 4) {
        digits = "0" + digits;
    }
    return "KEY" + digits;
}

int main() {
    std::cout << "=== HashTable Test Suite ===\n";

    // ----------------------------------------------------
    // Test 1: Empty table behaviour
    // ----------------------------------------------------
    std::cout << "\n--- Empty table ---\n";
    {
        HashTable table(8, 0.75f);
        check(table.size() == 0, "New table has size 0");
        check(table.find("ANY") == nullptr, "Find on empty table returns nullptr");
        check(!table.remove("ANY"), "Remove on empty table returns false");
        check(table.getLoadFactor() == 0.0f, "Load factor of empty table is 0");
    }

    // ----------------------------------------------------
    // Test 2: Insert, search, duplicate rejection
    // ----------------------------------------------------
    std::cout << "\n--- Insert / search / duplicates ---\n";
    {
        HashTable table(8, 0.75f);
        PuzzleRecord recA = {"SUD0001", "SUDOKU", "EASY", "dataA"};
        PuzzleRecord recB = {"NQ0001", "NQUEENS", "8", "NONE"};

        check(table.insert("SUD0001", &recA), "Insert first key");
        check(table.insert("NQ0001", &recB), "Insert second key");
        check(table.size() == 2, "Size is 2 after two inserts");
        check(table.find("SUD0001") == &recA, "Find returns correct record for first key");
        check(table.find("NQ0001") == &recB, "Find returns correct record for second key");
        check(!table.insert("SUD0001", &recB), "Duplicate key is rejected");
        check(table.size() == 2, "Size unchanged after duplicate rejection");
        check(table.find("SUD0001") == &recA, "Original record kept after duplicate attempt");
        check(table.find("MISSING") == nullptr, "Missing key returns nullptr");
    }

    // ----------------------------------------------------
    // Test 3: Collision handling (1 bucket forces every key into one chain)
    // ----------------------------------------------------
    std::cout << "\n--- Collisions (single long chain) ---\n";
    {
        // Load factor is set very high so no rehash happens: all keys collide.
        HashTable table(1, 1000.0f);
        PuzzleRecord records[5];
        for (int i = 0; i < 5; i++) {
            records[i].id = makeKey(i);
            check(table.insert(records[i].id, &records[i]), "Insert colliding key " + records[i].id);
        }
        check(table.getBucketCount() == 1, "Table stayed at 1 bucket (no rehash)");
        check(table.size() == 5, "All 5 colliding keys stored");
        bool allFound = true;
        for (int i = 0; i < 5; i++) {
            if (table.find(records[i].id) != &records[i]) {
                allFound = false;
            }
        }
        check(allFound, "All 5 colliding keys can be found");

        // Remove head, middle and tail of the chain (insertion prepends, so
        // the last inserted key is the head of the chain)
        check(table.remove(makeKey(4)), "Remove chain head");
        check(table.remove(makeKey(2)), "Remove chain middle");
        check(table.remove(makeKey(0)), "Remove chain tail");
        check(table.size() == 2, "Size is 2 after three removals");
        check(table.find(makeKey(1)) == &records[1], "Remaining key 1 still found");
        check(table.find(makeKey(3)) == &records[3], "Remaining key 3 still found");
        check(table.find(makeKey(2)) == nullptr, "Removed key 2 no longer found");
    }

    // ----------------------------------------------------
    // Test 4: Delete behaviour
    // ----------------------------------------------------
    std::cout << "\n--- Delete ---\n";
    {
        HashTable table(8, 0.75f);
        PuzzleRecord rec = {"K1", "SUDOKU", "EASY", "d"};
        table.insert("K1", &rec);
        check(table.remove("K1"), "Remove existing key returns true");
        check(table.find("K1") == nullptr, "Key not found after removal");
        check(!table.remove("K1"), "Removing the same key twice returns false");
        check(table.size() == 0, "Size is 0 after removing the only key");
        check(table.insert("K1", &rec), "Same key can be inserted again after removal");
        check(table.find("K1") == &rec, "Re-inserted key is found");
    }

    // ----------------------------------------------------
    // Test 5: Rehashing with 1,200 records
    // ----------------------------------------------------
    std::cout << "\n--- Rehash with 1,200 records ---\n";
    {
        HashTable table(4, 0.75f);
        const int total = 1200;
        PuzzleRecord* records = new PuzzleRecord[total];
        int initialBuckets = table.getBucketCount();

        bool insertsOk = true;
        for (int i = 0; i < total; i++) {
            records[i].id = makeKey(i);
            if (!table.insert(records[i].id, &records[i])) {
                insertsOk = false;
            }
        }
        check(insertsOk, "All 1,200 inserts succeeded");
        check(table.size() == total, "Size is 1200");
        check(table.getBucketCount() > initialBuckets, "Bucket count grew (rehash happened)");
        check(table.getLoadFactor() <= 0.75f, "Load factor stays at or below 0.75");

        bool allFound = true;
        for (int i = 0; i < total; i++) {
            if (table.find(records[i].id) != &records[i]) {
                allFound = false;
            }
        }
        check(allFound, "All 1,200 records still found after rehashing");

        bool removalsOk = true;
        for (int i = 0; i < total; i += 2) {
            if (!table.remove(records[i].id)) {
                removalsOk = false;
            }
        }
        check(removalsOk, "Removed every even-numbered record");
        check(table.size() == total / 2, "Size is 600 after removing half");
        check(table.find(makeKey(0)) == nullptr, "Removed record 0 not found");
        check(table.find(makeKey(1)) == &records[1], "Kept record 1 still found");

        delete[] records;
    }

    // ----------------------------------------------------
    // Test 6: Clear
    // ----------------------------------------------------
    std::cout << "\n--- Clear ---\n";
    {
        HashTable table(8, 0.75f);
        PuzzleRecord a = {"A", "SUDOKU", "EASY", "d"};
        PuzzleRecord b = {"B", "SUDOKU", "EASY", "d"};
        table.insert("A", &a);
        table.insert("B", &b);
        table.clear();
        check(table.size() == 0, "Size is 0 after clear");
        check(table.find("A") == nullptr, "Key A gone after clear");
        check(table.insert("A", &a), "Table is usable again after clear");
    }

    std::cout << "\n=== ALL " << testsPassed << " / " << testsRun << " HASHTABLE TESTS PASSED ===\n";
    return 0;
}