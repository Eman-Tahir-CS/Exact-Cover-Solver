#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <iostream>
#include <string>
#include "PuzzleRecord.h"

// Node structure for separate chaining in the hash table
struct HashNode {
    std::string key;          // Unique puzzle ID (e.g. "SUD0001")
    PuzzleRecord* record;     // Pointer to the record owned by PuzzleManager
    HashNode* nextNode;       // Next node in collision chain
};

// ============================================================================
// Class: HashTable
// Description: Custom Hash Table implementation using separate chaining and
//              the djb2 hashing algorithm (written without bitwise operators).
//              Automatically rehashes when load factor exceeds 0.75.
// Ownership: HashTable owns and frees its HashNode nodes.
// ============================================================================
class HashTable {
private:
    HashNode** buckets;       // Dynamic array of head pointers for each bucket chain
    int bucketCount;          // Total number of buckets
    int totalElementCount;    // Current number of key-value pairs stored
    float maxLoadFactor;      // Rehash threshold (default 0.75)

    // djb2 hash function implementation using arithmetic multiplication by 33
    int calculateHash(const std::string& key) const;

    // Resizes table when load factor exceeds maxLoadFactor, doubling bucket count
    void rehash(int newBucketCount);

    // Disabled copy constructor to prevent accidental shallow copies
    HashTable(const HashTable& other);

    // Disabled assignment operator
    HashTable& operator=(const HashTable& other);

public:
    // Constructor: Allocates initial bucket array
    HashTable(int initialBucketCount = 31, float loadFactor = 0.75f);

    // Destructor: Deallocates all HashNode objects and the bucket pointer array
    ~HashTable();

    // Inserts a key-record pair. Rejects duplicate keys and returns false.
    // Automatically triggers rehash if load factor exceeds threshold.
    bool insert(const std::string& key, PuzzleRecord* record);

    // Looks up a key and returns pointer to PuzzleRecord, or nullptr if not found
    PuzzleRecord* find(const std::string& key) const;

    // Removes a key from the table. Returns true if found and deleted, false otherwise
    bool remove(const std::string& key);

    // Returns total number of stored elements
    int size() const;

    // Returns current number of buckets
    int getBucketCount() const;

    // Returns current load factor (totalElementCount / bucketCount)
    float getLoadFactor() const;

    // Clears all nodes from all buckets
    void clear();
};

#endif // HASH_TABLE_H
