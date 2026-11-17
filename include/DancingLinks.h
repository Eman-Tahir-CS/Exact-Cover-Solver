#ifndef DANCING_LINKS_H
#define DANCING_LINKS_H

#include <iostream>
#include <string>

// Forward declaration
class ColumnNode;

// ============================================================================
// Structure: DLXNode
// Description: Basic 4-way circular doubly linked node used in Dancing Links.
//              Points left, right, up, down, and references its column header.
// Invariant: Pointers always form circular rings; unlinking only modifies
//            neighbor pointers, preserving the node's internal state.
// ============================================================================
struct DLXNode {
    DLXNode* left;
    DLXNode* right;
    DLXNode* up;
    DLXNode* down;
    ColumnNode* column;
    int rowId;

    DLXNode();
    virtual ~DLXNode();
};

// ============================================================================
// Structure: ColumnNode (inherits DLXNode)
// Description: Column header node representing an exact cover constraint.
//              Tracks the number of 1s in this column (size) and column name.
// ============================================================================
struct ColumnNode : public DLXNode {
    int size;             // Number of active 1s in this column
    std::string name;     // Descriptive name of constraint
    bool isPrimary;       // True if primary (must be covered), false if secondary

    ColumnNode(const std::string& colName = "", bool primary = true);
    virtual ~ColumnNode();
};

// ============================================================================
// Class: DancingLinks
// Description: Implements Donald Knuth's Dancing Links (DLX) circular 2D
//              doubly linked list structure. Supports exact cover via cover()
//              and uncover() operations which only relink pointers, never delete.
// Ownership: Destructor safely deletes every allocated node exactly once.
// ============================================================================
class DancingLinks {
private:
    ColumnNode* root;               // Root header node of the DLX torus

    // Dynamic array tracking all allocated nodes to guarantee single-pass deletion
    DLXNode** allAllocatedNodes;
    int totalNodeCount;
    int allocatedNodeCapacity;

    // Resizes internal node tracking array when capacity is reached
    void resizeNodeTracker(int newCapacity);

    // Records an allocated node in the internal tracker
    void trackNode(DLXNode* node);

    // Disabled copy constructor to prevent accidental shallow copies
    DancingLinks(const DancingLinks& other);

    // Disabled assignment operator
    DancingLinks& operator=(const DancingLinks& other);

public:
    // Constructor: Allocates root node
    DancingLinks(int initialCapacity = 512);

    // Destructor: Deallocates all nodes tracked in allAllocatedNodes
    ~DancingLinks();

    // Adds a column header (primary or secondary)
    ColumnNode* addColumn(const std::string& name, bool isPrimary = true);

    // Adds a row with 1s in the specified columns
    DLXNode* addRow(int rowId, ColumnNode** columnsInRow, int count);

    // Covers a column: removes it from the header list and unlinks all conflicting rows
    void cover(ColumnNode* column);

    // Uncovers a column in exact reverse order: restores the column and all rows
    void uncover(ColumnNode* column);

    // Chooses the primary column with the minimum size (Knuth's S-heuristic)
    ColumnNode* chooseSmallestColumn() const;

    // Returns the root header node
    ColumnNode* getRoot() const;

    // Finds a column node by name (linear search from root)
    ColumnNode* getColumnByName(const std::string& name) const;
};

#endif // DANCING_LINKS_H
