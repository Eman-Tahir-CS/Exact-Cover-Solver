#include "DancingLinks.h"

// DLXNode Constructor: Initializes 4-way circular pointers to self
DLXNode::DLXNode() {
    left = this;
    right = this;
    up = this;
    down = this;
    column = nullptr;
    rowId = -1;
}

// DLXNode Destructor
DLXNode::~DLXNode() {
}

// ColumnNode Constructor: Initializes as column header
ColumnNode::ColumnNode(const std::string& colName, bool primary) : DLXNode() {
    size = 0;
    name = colName;
    isPrimary = primary;
    column = this;
}

// ColumnNode Destructor
ColumnNode::~ColumnNode() {
}

// Resizes internal dynamic array tracking allocated nodes
void DancingLinks::resizeNodeTracker(int newCapacity) {
    DLXNode** newArray = new DLXNode*[newCapacity];
    for (int index = 0; index < totalNodeCount; index++) {
        newArray[index] = allAllocatedNodes[index];
    }
    delete[] allAllocatedNodes;
    allAllocatedNodes = newArray;
    allocatedNodeCapacity = newCapacity;
}

// Tracks an allocated node for safe single-pass deletion in destructor
void DancingLinks::trackNode(DLXNode* node) {
    if (totalNodeCount == allocatedNodeCapacity) {
        resizeNodeTracker(allocatedNodeCapacity * 2);
    }
    allAllocatedNodes[totalNodeCount] = node;
    totalNodeCount++;
}

// Constructor: Allocates node tracker and root column header
DancingLinks::DancingLinks(int initialCapacity) {
    if (initialCapacity <= 0) {
        initialCapacity = 512;
    }
    allocatedNodeCapacity = initialCapacity;
    totalNodeCount = 0;
    allAllocatedNodes = new DLXNode*[allocatedNodeCapacity];

    root = new ColumnNode("root", true);
    trackNode(root);
}

// Destructor: Safely frees all tracked nodes exactly once
DancingLinks::~DancingLinks() {
    for (int index = 0; index < totalNodeCount; index++) {
        if (allAllocatedNodes[index] != nullptr) {
            delete allAllocatedNodes[index];
            allAllocatedNodes[index] = nullptr;
        }
    }
    delete[] allAllocatedNodes;
    allAllocatedNodes = nullptr;
    root = nullptr;
    totalNodeCount = 0;
    allocatedNodeCapacity = 0;
}

// Adds a new column header to the matrix
ColumnNode* DancingLinks::addColumn(const std::string& name, bool isPrimary) {
    ColumnNode* newColumn = new ColumnNode(name, isPrimary);
    trackNode(newColumn);

    if (isPrimary) {
        // Link horizontally before root (at the end of primary column ring)
        newColumn->right = root;
        newColumn->left = root->left;
        root->left->right = newColumn;
        root->left = newColumn;
    } else {
        // Secondary columns are self-linked horizontally (not in root ring)
        newColumn->left = newColumn;
        newColumn->right = newColumn;
    }

    return newColumn;
}

// Adds a matrix row containing 1s at the specified columns
DLXNode* DancingLinks::addRow(int rowId, ColumnNode** columnsInRow, int count) {
    if (count <= 0 || columnsInRow == nullptr) {
        return nullptr;
    }

    DLXNode* firstNodeInRow = nullptr;

    for (int colIndex = 0; colIndex < count; colIndex++) {
        ColumnNode* targetCol = columnsInRow[colIndex];
        if (targetCol == nullptr) {
            continue;
        }

        DLXNode* newNode = new DLXNode();
        newNode->rowId = rowId;
        newNode->column = targetCol;
        trackNode(newNode);

        // Link vertically at the bottom of targetCol
        newNode->down = targetCol;
        newNode->up = targetCol->up;
        targetCol->up->down = newNode;
        targetCol->up = newNode;
        targetCol->size++;

        // Link horizontally into row circle
        if (firstNodeInRow == nullptr) {
            firstNodeInRow = newNode;
            newNode->left = newNode;
            newNode->right = newNode;
        } else {
            newNode->right = firstNodeInRow;
            newNode->left = firstNodeInRow->left;
            firstNodeInRow->left->right = newNode;
            firstNodeInRow->left = newNode;
        }
    }

    return firstNodeInRow;
}

// Covers a column (Knuth's Algorithm X cover operation)
void DancingLinks::cover(ColumnNode* column) {
    if (column == nullptr) {
        return;
    }

    // Unlink column header horizontally from primary list
    if (column->isPrimary) {
        column->right->left = column->left;
        column->left->right = column->right;
    }

    // Unlink all conflicting rows vertically
    for (DLXNode* rowNode = column->down; rowNode != column; rowNode = rowNode->down) {
        for (DLXNode* otherNode = rowNode->right; otherNode != rowNode; otherNode = otherNode->right) {
            otherNode->down->up = otherNode->up;
            otherNode->up->down = otherNode->down;
            otherNode->column->size--;
        }
    }
}

// Uncovers a column in exact reverse order (restores nodes without reallocating)
void DancingLinks::uncover(ColumnNode* column) {
    if (column == nullptr) {
        return;
    }

    // Restore all conflicting rows in exact reverse order (upward, leftward)
    for (DLXNode* rowNode = column->up; rowNode != column; rowNode = rowNode->up) {
        for (DLXNode* otherNode = rowNode->left; otherNode != rowNode; otherNode = otherNode->left) {
            otherNode->column->size++;
            otherNode->down->up = otherNode;
            otherNode->up->down = otherNode;
        }
    }

    // Relink column header horizontally into primary list
    if (column->isPrimary) {
        column->right->left = column;
        column->left->right = column;
    }
}

// Chooses primary column with fewest 1s (Knuth's S-heuristic)
ColumnNode* DancingLinks::chooseSmallestColumn() const {
    ColumnNode* bestColumn = nullptr;
    int minSize = 2147483647; // Maximum integer

    for (DLXNode* current = root->right; current != root; current = current->right) {
        ColumnNode* candidateCol = (ColumnNode*)current;
        if (candidateCol->size < minSize) {
            minSize = candidateCol->size;
            bestColumn = candidateCol;
            if (minSize == 0) {
                return bestColumn; // Immediate prune
            }
        }
    }

    return bestColumn;
}

// Returns root header node
ColumnNode* DancingLinks::getRoot() const {
    return root;
}

// Finds column by name
ColumnNode* DancingLinks::getColumnByName(const std::string& name) const {
    for (DLXNode* current = root->right; current != root; current = current->right) {
        ColumnNode* candidateCol = (ColumnNode*)current;
        if (candidateCol->name == name) {
            return candidateCol;
        }
    }
    return nullptr;
}
