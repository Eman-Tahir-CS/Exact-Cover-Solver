#include <iostream>
#include <string>
#include <cstdlib>
#include "DancingLinks.h"

// Test assertion helper
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
    std::cout << "   RUNNING TEST: DancingLinks (DLX)     " << std::endl;
    std::cout << "========================================" << std::endl;

    // Test 1: Construction of DLX torus and circular header links
    DancingLinks dlx;
    ColumnNode* root = dlx.getRoot();
    assertCondition(root != nullptr, "DLX root node exists");
    assertCondition(root->left == root && root->right == root, "Initial root ring is self-linked");

    ColumnNode* colA = dlx.addColumn("A", true);
    ColumnNode* colB = dlx.addColumn("B", true);
    ColumnNode* colC = dlx.addColumn("C", true);

    assertCondition(root->right == colA, "root->right points to column A");
    assertCondition(colA->right == colB, "colA->right points to column B");
    assertCondition(colB->right == colC, "colB->right points to column C");
    assertCondition(colC->right == root, "colC->right wraps around to root");
    assertCondition(colA->left == root, "colA->left points back to root");
    assertCondition(colC->left == colB, "colC->left points to colB");

    // Test 2: Adding rows and verifying vertical sizes
    // Row 0: covers A and B
    ColumnNode* row0Cols[2] = {colA, colB};
    dlx.addRow(0, row0Cols, 2);

    // Row 1: covers B and C
    ColumnNode* row1Cols[2] = {colB, colC};
    dlx.addRow(1, row1Cols, 2);

    assertCondition(colA->size == 1, "Column A has size 1");
    assertCondition(colB->size == 2, "Column B has size 2 (present in Row 0 and Row 1)");
    assertCondition(colC->size == 1, "Column C has size 1");

    // Test 3: Minimum column heuristic (chooseSmallestColumn)
    // Between A(size 1), B(size 2), C(size 1), smallest has size 1
    ColumnNode* smallest = dlx.chooseSmallestColumn();
    assertCondition(smallest == colA || smallest == colC, "chooseSmallestColumn selects column with size 1");

    // Test 4: Knuth's cover operation on Column B
    // Covering B should:
    // 1. Remove B from the root horizontal ring (A->right will point to C)
    // 2. Unlink Row 0 (which removes node from A, reducing A's size to 0)
    // 3. Unlink Row 1 (which removes node from C, reducing C's size to 0)
    dlx.cover(colB);
    assertCondition(colA->right == colC, "Covering B unlinks B from primary ring (A->right is now C)");
    assertCondition(colC->left == colA, "Covering B updates C->left to point to A");
    assertCondition(colA->size == 0, "Column A size decremented to 0 after conflicting Row 0 was covered");
    assertCondition(colC->size == 0, "Column C size decremented to 0 after conflicting Row 1 was covered");

    // Test 5: Knuth's uncover operation (exact reverse order restoration)
    // Uncovering B must restore:
    // 1. Column sizes back to original (A=1, B=2, C=1)
    // 2. Circular horizontal links (A->right == B, B->right == C, B->left == A, C->left == B)
    dlx.uncover(colB);
    assertCondition(colA->right == colB, "Uncovering B restores A->right to B");
    assertCondition(colB->right == colC, "Uncovering B restores B->right to C");
    assertCondition(colC->left == colB, "Uncovering B restores C->left to B");
    assertCondition(colB->left == colA, "Uncovering B restores B->left to A");
    assertCondition(colA->size == 1, "Column A size restored to 1");
    assertCondition(colB->size == 2, "Column B size restored to 2");
    assertCondition(colC->size == 1, "Column C size restored to 1");

    // ----------------------------------------------------
    // Section 2: Knuth's Exact Cover Example
    // ----------------------------------------------------
    std::cout << "\n--- Testing Knuth's Exact Cover Example ---" << std::endl;
    // Matrix with 4 columns (1, 2, 3, 4)
    // Row 0: covers {1, 2}
    // Row 1: covers {3, 4}
    // Row 2: covers {2, 3}
    // Exact cover is clearly {Row 0, Row 1}.
    DancingLinks knuthDlx;
    ColumnNode* c1 = knuthDlx.addColumn("1", true);
    ColumnNode* c2 = knuthDlx.addColumn("2", true);
    ColumnNode* c3 = knuthDlx.addColumn("3", true);
    ColumnNode* c4 = knuthDlx.addColumn("4", true);

    ColumnNode* r0[2] = {c1, c2};
    knuthDlx.addRow(0, r0, 2);

    ColumnNode* r1[2] = {c3, c4};
    knuthDlx.addRow(1, r1, 2);

    ColumnNode* r2[2] = {c2, c3};
    knuthDlx.addRow(2, r2, 2);

    assertCondition(c1->size == 1 && c2->size == 2 && c3->size == 2 && c4->size == 1, 
                    "Knuth example columns initialized with correct sizes");

    // Simulate Algorithm X manual step:
    // Step 1: Select c1 (size 1). Row 0 covers c1 and c2.
    knuthDlx.cover(c1);
    for (DLXNode* n = c1->down->right; n != c1->down; n = n->right) {
        knuthDlx.cover(n->column);
    }
    // Now c1 and c2 are covered. In row 2, node in c2 was unlinked, so c3's size decremented by 1!
    assertCondition(c3->size == 1, "Conflicting Row 2 was removed, leaving c3 with size 1");

    // Step 2: Select c3 (size 1). Row 1 covers c3 and c4.
    knuthDlx.cover(c3);
    for (DLXNode* n = c3->down->right; n != c3->down; n = n->right) {
        knuthDlx.cover(n->column);
    }

    // Now root->right == root because all 4 primary columns are covered!
    assertCondition(knuthDlx.getRoot()->right == knuthDlx.getRoot(), 
                    "All primary columns covered: EXACT COVER FOUND!");

    // Backtrack and uncover in reverse order
    for (DLXNode* n = c3->down->left; n != c3->down; n = n->left) {
        knuthDlx.uncover(n->column);
    }
    knuthDlx.uncover(c3);

    for (DLXNode* n = c1->down->left; n != c1->down; n = n->left) {
        knuthDlx.uncover(n->column);
    }
    knuthDlx.uncover(c1);

    assertCondition(knuthDlx.getRoot()->right == c1, "After full uncover, root points back to column 1");
    assertCondition(c1->size == 1 && c2->size == 2 && c3->size == 2 && c4->size == 1, 
                    "All columns restored to original sizes after backtrack");

    std::cout << "\n========================================" << std::endl;
    std::cout << " ALL DANCING LINKS TESTS PASSED!        " << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
