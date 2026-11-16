#ifndef PUZZLE_RECORD_H
#define PUZZLE_RECORD_H

#include <string>

// ============================================================================
// Structure: PuzzleRecord
// Description: Stores metadata and puzzle payload data for a single puzzle.
// Format in file: <ID>|<TYPE>|<DIFFICULTY_OR_N>|<DATA>
// Examples:
//   SUD0001|SUDOKU|EASY|530070000600195000098000060800060003400803001700020006060000280000419005000080079
//   NQ0001|NQUEENS|8|NONE
// ============================================================================
struct PuzzleRecord {
    std::string id;             // Unique identifier, e.g. "SUD0001" or "NQ0001"
    std::string type;           // Puzzle type: "SUDOKU" or "NQUEENS"
    std::string difficultyOrN;  // Sudoku difficulty ("EASY", "MEDIUM", "HARD") or N dimension ("8", "10")
    std::string puzzleData;     // 81-char string for Sudoku; fixed queens string (or "NONE") for N-Queens
};

#endif // PUZZLE_RECORD_H
