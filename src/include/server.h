#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

int rows;         
int columns;      
int total_mines;  
int game_state;  

bool mine_map[35][35];       
bool visited_map[35][35];    
bool marked_map[35][35];    
int visit_count_global = 0;
int fail_r = -1, fail_c = -1;

int get_mine_count(int r, int c) {
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
                if (mine_map[nr][nc]) count++;
            }
        }
    }
    return count;
}

void recursive_visit(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= columns || visited_map[r][c] || mine_map[r][c]) return;
    visited_map[r][c] = true;
    visit_count_global++;
    if (get_mine_count(r, c) == 0) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                recursive_visit(r + dr, c + dc);
            }
        }
    }
}

void InitMap() {
    if (!(std::cin >> rows >> columns)) return;
    total_mines = 0;
    for (int i = 0; i < rows; ++i) {
        std::string row_str;
        std::cin >> row_str;
        for (int j = 0; j < columns; ++j) {
            mine_map[i][j] = (row_str[j] == 'X');
            visited_map[i][j] = false;
            marked_map[i][j] = false;
            if (mine_map[i][j]) total_mines++;
        }
    }
    game_state = 0;
    visit_count_global = 0;
    fail_r = -1; fail_c = -1;
}

void VisitBlock(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= columns || game_state != 0) return;
    if (visited_map[r][c] || marked_map[r][c]) return;

    if (mine_map[r][c]) {
        game_state = -1;
        fail_r = r; fail_c = c;
        visited_map[r][c] = true; 
    } else {
        recursive_visit(r, c);
        if (visit_count_global == (rows * columns - total_mines)) {
            game_state = 1;
        }
    }
}

void MarkMine(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= columns || game_state != 0) return;
    if (visited_map[r][c] || marked_map[r][c]) return;

    if (mine_map[r][c]) {
        marked_map[r][c] = true;
    } else {
        game_state = -1;
        fail_r = r; fail_c = c;
    }
}

void AutoExplore(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= columns || game_state != 0) return;
    if (!visited_map[r][c] || mine_map[r][c]) return;

    int target_count = get_mine_count(r, c);
    int marked_around = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
                if (marked_map[nr][nc]) marked_around++;
            }
        }
    }

    if (marked_around == target_count) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                int nr = r + dr;
                int nc = c + dc;
                if (nr >= 0 && nr < rows && nc >= 0 && nc < columns) {
                    if (!visited_map[nr][nc] && !marked_map[nr][nc]) {
                        if (mine_map[nr][nc]) {
                            game_state = -1;
                            fail_r = nr; fail_c = nc;
                            visited_map[nr][nc] = true;
                            return;
                        } else {
                            recursive_visit(nr, nc);
                        }
                    }
                }
            }
        }
        if (visit_count_global == (rows * columns - total_mines)) {
            game_state = 1;
        }
    }
}

void ExitGame() {
    if (game_state == 1) {
        std::cout << "YOU WIN!" << std::endl;
        std::cout << visit_count_global << " " << total_mines << std::endl;
    } else {
        std::cout << "GAME OVER!" << std::endl;
        int correct_marked = 0;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                if (marked_map[i][j] && mine_map[i][j]) correct_marked++;
            }
        }
        std::cout << visit_count_global << " " << correct_marked << std::endl;
    }
    exit(0);
}

void PrintMap() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (game_state == 1) {
                if (mine_map[i][j]) std::cout << '@';
                else std::cout << get_mine_count(i, j);
            } else {
                if (visited_map[i][j]) {
                    if (mine_map[i][j]) std::cout << 'X';
                    else std::cout << get_mine_count(i, j);
                } else if (marked_map[i][j]) {
                    if (mine_map[i][j]) std::cout << '@';
                    else std::cout << 'X';
                } else if (game_state == -1 && i == fail_r && j == fail_c && !mine_map[i][j]) {
                    std::cout << 'X';
                } else {
                    std::cout << '?';
                }
            }
        }
        std::cout << std::endl;
    }
}

#endif
