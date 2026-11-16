#include "HashTable.h"

// djb2 hash function using standard arithmetic (hash * 33 + c)
int HashTable::calculateHash(const std::string& key) const {
    unsigned long hashValue = 5381;
    int keyLength = key.length();

    for (int index = 0; index < keyLength; index++) {
        unsigned char currentChar = (unsigned char)key[index];
        hashValue = (hashValue * 33) + currentChar;
    }

    int bucketIndex = (int)(hashValue % (unsigned long)bucketCount);
    if (bucketIndex < 0) {
        bucketIndex = -bucketIndex;
    }
    return bucketIndex;
}

// Constructor: Allocates initial bucket pointers initialized to nullptr
HashTable::HashTable(int initialBucketCount, float loadFactor) {
    if (initialBucketCount <= 0) {
        initialBucketCount = 31;
    }
    bucketCount = initialBucketCount;
    totalElementCount = 0;
    maxLoadFactor = loadFactor;

    buckets = new HashNode*[bucketCount];
    for (int index = 0; index < bucketCount; index++) {
        buckets[index] = nullptr;
    }
}

// Clears all nodes from every bucket chain
void HashTable::clear() {
    for (int index = 0; index < bucketCount; index++) {
        HashNode* currentNode = buckets[index];
        while (currentNode != nullptr) {
            HashNode* nodeToDelete = currentNode;
            currentNode = currentNode->nextNode;
            delete nodeToDelete;
        }
        buckets[index] = nullptr;
    }
    totalElementCount = 0;
}

// Destructor: Deallocates all nodes and the bucket array
HashTable::~HashTable() {
    clear();
    if (buckets != nullptr) {
        delete[] buckets;
        buckets = nullptr;
    }
    bucketCount = 0;
}

// Rehashes the entire table by doubling bucket array and redistributing existing nodes
void HashTable::rehash(int newBucketCount) {
    HashNode** newBuckets = new HashNode*[newBucketCount];
    for (int index = 0; index < newBucketCount; index++) {
        newBuckets[index] = nullptr;
    }

    // Move existing nodes into new buckets without reallocating
    for (int index = 0; index < bucketCount; index++) {
        HashNode* currentNode = buckets[index];
        while (currentNode != nullptr) {
            HashNode* nextNode = currentNode->nextNode;

            // Recalculate bucket index for the new capacity
            unsigned long hashValue = 5381;
            int keyLength = currentNode->key.length();
            for (int charIndex = 0; charIndex < keyLength; charIndex++) {
                hashValue = (hashValue * 33) + (unsigned char)currentNode->key[charIndex];
            }
            int newBucketIndex = (int)(hashValue % (unsigned long)newBucketCount);
            if (newBucketIndex < 0) {
                newBucketIndex = -newBucketIndex;
            }

            // Prepend to new bucket chain
            currentNode->nextNode = newBuckets[newBucketIndex];
            newBuckets[newBucketIndex] = currentNode;

            currentNode = nextNode;
        }
    }

    delete[] buckets;
    buckets = newBuckets;
    bucketCount = newBucketCount;
}

// Inserts a new key and record pointer into the table
// Returns false if key already exists (rejects duplicates)
bool HashTable::insert(const std::string& key, PuzzleRecord* record) {
    int targetBucket = calculateHash(key);

    // Check for duplicate key in chain
    HashNode* currentNode = buckets[targetBucket];
    while (currentNode != nullptr) {
        if (currentNode->key == key) {
            return false; // Reject duplicate ID
        }
        currentNode = currentNode->nextNode;
    }

    // Check if rehash is needed
    float potentialLoadFactor = (float)(totalElementCount + 1) / (float)bucketCount;
    if (potentialLoadFactor > maxLoadFactor) {
        rehash(bucketCount * 2);
        targetBucket = calculateHash(key);
    }

    // Allocate and prepend new node
    HashNode* newNode = new HashNode;
    newNode->key = key;
    newNode->record = record;
    newNode->nextNode = buckets[targetBucket];
    buckets[targetBucket] = newNode;

    totalElementCount++;
    return true;
}

// Searches for a key and returns pointer to PuzzleRecord, or nullptr if not found
PuzzleRecord* HashTable::find(const std::string& key) const {
    int targetBucket = calculateHash(key);
    HashNode* currentNode = buckets[targetBucket];

    while (currentNode != nullptr) {
        if (currentNode->key == key) {
            return currentNode->record;
        }
        currentNode = currentNode->nextNode;
    }

    return nullptr;
}

// Removes a key from the table. Returns true if removed, false if not found
bool HashTable::remove(const std::string& key) {
    int targetBucket = calculateHash(key);
    HashNode* currentNode = buckets[targetBucket];
    HashNode* previousNode = nullptr;

    while (currentNode != nullptr) {
        if (currentNode->key == key) {
            if (previousNode == nullptr) {
                buckets[targetBucket] = currentNode->nextNode;
            } else {
                previousNode->nextNode = currentNode->nextNode;
            }
            delete currentNode;
            totalElementCount--;
            return true;
        }
        previousNode = currentNode;
        currentNode = currentNode->nextNode;
    }

    return false;
}

// Returns total number of stored elements
int HashTable::size() const {
    return totalElementCount;
}

// Returns current number of buckets
int HashTable::getBucketCount() const {
    return bucketCount;
}

// Returns current load factor
float HashTable::getLoadFactor() const {
    if (bucketCount == 0) {
        return 0.0f;
    }
    return (float)totalElementCount / (float)bucketCount;
}
