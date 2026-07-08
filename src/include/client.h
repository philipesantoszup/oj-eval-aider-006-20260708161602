#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>
#include <string>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

// Global state for the client
int current_map[35][35]; // -1: unvisited, -2: marked, 0-8: mine count
bool visited[35][35];
bool marked[35][35];

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // Initialize global variables
  for(int i=0; i<35; ++i) {
    for(int j=0; j<35; ++j) {
      visited[i][j] = false;
      marked[i][j] = false;
      current_map[i][j] = -1;
    }
  }
  int first_row, first_column;
  if (!(std::cin >> first_row >> first_column)) return;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 */
void ReadMap() {
  for (int i = 0; i < rows; ++i) {
    std::string line;
    if (!(std::cin >> line)) break;
    for (int j = 0; j < columns; ++j) {
      char c = line[j];
      if (c == '?') {
        visited[i][j] = false;
        marked[i][j] = false;
        current_map[i][j] = -1;
      } else if (c == '@') {
        visited[i][j] = false;
        marked[i][j] = true;
        current_map[i][j] = -2;
      } else if (c == 'X') {
        // This could be a visited mine or a wrongly marked non-mine
        visited[i][j] = true; 
        marked[i][j] = false;
        current_map[i][j] = 9; // Special value for mine
      } else {
        visited[i][j] = true;
        marked[i][j] = false;
        current_map[i][j] = c - '0';
      }
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role.
 */
void Decide() {
  // 1. Try to mark obvious mines
  // If a visited non-mine grid has k unknown neighbors and its number is k, mark all neighbors as mines.
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (visited[r][c] && current_map[r][c] >= 0 && current_map[r][c] <= 8) {
        int unvisited_neighbors = 0;
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns && !visited[nr][nc] && !marked[nr][nc]) {
              unvisited_neighbors++;
            }
          }
        }
        if (unvisited_neighbors > 0 && unvisited_neighbors == current_map[r][c]) {
          for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
              if (dr == 0 && dc == 0) continue;
              int nr = r + dr, nc = c + dc;
              if (nr >= 0 && nr < rows && nc >= 0 && nc < columns && !visited[nr][nc] && !marked[nr][nc]) {
                Execute(nr, nc, 1);
                return;
              }
            }
          }
        }
      }
    }
  }

  // 2. Try to auto-explore safe grids
  // If the number of marked mines around the target equals its mine count, all non-mine grids among the 8 surrounding grids are visited.
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (visited[r][c] && current_map[r][c] >= 0 && current_map[r][c] <= 8) {
        int marked_neighbors = 0;
        for (int dr = -1; dr <= 1; ++dr) {
          for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns && marked[nr][nc]) {
              marked_neighbors++;
            }
          }
        }
        if (marked_neighbors == current_map[r][c]) {
          Execute(r, c, 2);
          return;
        }
      }
    }
  }

  // 3. Fallback: Visit a random unvisited/unmarked cell
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < columns; ++c) {
      if (!visited[r][c] && !marked[r][c]) {
        Execute(r, c, 0);
        return;
      }
    }
  }
}

#endif
