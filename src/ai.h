#ifndef AI_H
#define AI_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
// #include <algorithm>
#include "tetris_movement.h"

std::vector<std::vector<char> > convert_board(const std::vector<std::vector<char> >& grid, const std::vector<std::pair<int, int> >& piece_location) {
    std::vector<std::vector<char> > new_board(grid.size(), std::vector<char>(grid[0].size(), 0));
    for (int i = 0; i < grid.size(); i++) {
      	for (int j = 0; j < grid[0].size(); j++) {
        	if (std::find(piece_location.begin(), piece_location.end(), std::make_pair(i, j)) == piece_location.end()) {
          		new_board[i][j] = (grid[i][j] != '.') ? 'X' : '.';
        	} else {
                new_board[i][j] = '.';
            }
        }
    }
    return new_board;
}

std::vector<std::vector<char> > grid_and_piece(const std::vector<std::vector<char> >& grid, const std::vector<std::pair<int, int> >& piece_location) {
    std::vector<std::vector<char> > new_grid(grid.size(), std::vector<char>(grid[0].size(), '.'));
    for (int row = 0; row < grid.size(); row++) {
        for (int col = 0; col < grid[0].size(); col++) {
            new_grid[row][col] = grid[row][col];
        }
    }
    for (auto& p : piece_location) {
        new_grid[p.first][p.second] = 'X';
    }
    return new_grid;
}

int count_holes(const std::vector<std::vector<char> >& grid) {
    int count = 0;
    for (int j = 0; j < grid[0].size(); j++) {
        int empty = 0;
        for (int i = grid.size() - 1; i >= 0; i--) {
            if (grid[i][j] == 'X') {
                count += empty;
                empty = 0;
            } else {
                empty++;
            }
        }
    }
    return count;
}

int aggregate_height(const std::vector<std::vector<char> >& grid) {
    int height = 0;
    for (int j = 0; j < grid[0].size(); j++) {
        int i = 0;
        while (i < grid.size()) {
            if (grid[i][j] == 'X') {
                height += grid.size() - i;
                break;
            }
            i++;
        }
    }
    return height;
}

int bumpiness(const std::vector<std::vector<char> >& grid) {
    int score = 0;
    std::vector<int> heights(grid[0].size(), 0);
    for (int j = 0; j < grid[0].size(); j++) {
        int i = 0;
        while (i < grid.size()) {
            if (grid[i][j] == 'X') {
                heights[j] = grid.size() - i;
                break;
            }
            i++;
        }
    }
    for (int i = 0; i < heights.size() - 1; i++) {
        score += pow(heights[i] - heights[i+1], 2);
    }
    return score;
}

int lines_full(const std::vector<std::vector<char> >& grid) {
    int lines = 0;
    for (auto& row : grid) {
        bool full = true;
        for (char i : row) {
            if (i == '.') {
                full = false;
                break;
            }
        }
        if (full) {
            lines++;
        }
    }

    return lines;
}

std::vector<std::pair<int, int> > drop(const std::vector<std::vector<char> >& grid, const std::vector<std::pair<int, int> >& piece_location) {
    std::vector<std::pair<int, int> > new_piece_location = piece_location;
    bool bottom = false;

    while (!bottom) {
        // std::cout << "Testing drop ";
        for (auto& p : new_piece_location) {
            if (grid[p.first + 1][p.second] == 'X') {
                // std::cout << "Finished testing drop!" << std::endl;
                return new_piece_location;
            }
        }

        for (int i = 0; i < 4; i++) {
            new_piece_location[i].first++;
            if (new_piece_location[i].first == grid.size() - 1) {
                bottom = true;
            }
        }
    }
    // std::cout << "Finished testing drop!" << std::endl;
    return new_piece_location;
}

double best(const std::vector<double>& weights, const std::vector<std::vector<char> >& grid);

double MLP(const std::vector<std::vector<double> >& W1, const std::vector<double>& W2, const std::vector<std::vector<char> >& grid);

std::pair<std::vector<char>, double> generate_move(const std::vector<std::vector<char> >& grid, std::vector<std::pair<int, int> > piece_location, char piece, std::vector<std::vector<double> > weights1, std::vector<double> weights2) {
    std::vector<char> moves = {'D'};
    std::vector<char> best_set = {'N'};
    double best_score = -10000000.0;
    std::vector<std::pair<int, int> > new_piece_location = piece_location;
    for (auto& p : new_piece_location) {
        p.first++;
    }
    for (int rot = 0; rot < 4; rot++) {
        moves = {'D'};
        new_piece_location = piece_location;
        for (auto& p : new_piece_location) {
            p.first++;
        }
        for (int i = 0; i < rot; i++) {
            moves.push_back('U');
            // std::cout << "Spinning... ";
            new_piece_location = spin(new_piece_location, piece);
            // std::cout << "Spin successful\n";
        }
        int max_left = 0;
        for (; max_left < 6; max_left++) {
            bool at_edge = false;
            for (auto& p : new_piece_location) {
                if (p.second == 0) {
                    at_edge = true;
                    break;
                }
                if (grid[p.first][p.second - 1] == 'X') {
                    at_edge = true;
                    break;
                }
            }

            if (!at_edge) {
                moves.push_back('L');
                for (auto& p : new_piece_location) {
                    p.second--;
                }
            } else {
                break;
            }
        }
        for (int x = -1 * max_left; x <= 6; x++) {
            // std::cout << "Calculating best score for ";
            // for (char r : moves) {std::cout << r << " ";}
            // std::cout << "... ";
            double curr_score = MLP(weights1, weights2, grid_and_piece(grid, drop(grid, new_piece_location)));
            // std::cout << "score is " << curr_score << "\n";
            if (best_score <= curr_score) {
                best_score = curr_score;
                best_set = moves;
            }

            if (x < 0) {
                moves.pop_back();
            } else {
                moves.push_back('R');
            }
            bool at_edge = false;
            for (auto& p : new_piece_location) {
                if (p.second == 9) {
                    at_edge = true;
                    break;
                }
                if (grid[p.first][p.second + 1] == 'X') {
                    at_edge = true;
                    break;
                }
            }
            if (!at_edge) {
                for (auto& p : new_piece_location) {
                    p.second++;
                }
            } else {
                break;
            }
        }
    }
    // std::cout << "Found best move!\n";
    return std::make_pair(best_set, best_score);
}

std::vector<char> generate_best_move(const std::vector<std::vector<char> >& grid, std::vector<std::pair<int, int> > piece_location, char piece, std::vector<std::vector<double> > weights1, std::vector<double> weights2, const char hold, const char next, std::map<char, std::vector<std::pair<int, int> > > start_positions) {
    std::pair<std::vector<char>, double> no_switch_res = generate_move(grid, piece_location, piece, weights1, weights2);
    std::pair<std::vector<char>, double> switch_res;
    std::vector<std::pair<int, int> > new_piece_location;
    if (hold == '.') {
        new_piece_location = start_positions[next];
        switch_res = generate_move(grid, new_piece_location, next, weights1, weights2);
    } else {
        new_piece_location = start_positions[hold];
        switch_res = generate_move(grid, new_piece_location, hold, weights1, weights2);
    }
    if (no_switch_res.second > switch_res.second) {
        return no_switch_res.first;
    }
    std::vector<char> res = {'S'};
    for (char move : switch_res.first) {
        res.push_back(move);
    }
    return res;
}

double best(const std::vector<double>& weights, const std::vector<std::vector<char> >& grid) {
    return weights[0] * count_holes(grid) + weights[1] * aggregate_height(grid) + weights[2] * lines_full(grid) + weights[3] * bumpiness(grid);
}

double tanh_func(double val) {
    return (exp(val) - exp(-val)) / (exp(val) + exp(-val));
}

double MLP(const std::vector<std::vector<double> >& W1, const std::vector<double>& W2, const std::vector<std::vector<char> >& grid) {
    double lines = lines_full(grid);
    lines = 2 * lines / 4 - 1;
    std::vector<std::vector<char> > new_grid = grid;
    while (clear_line(new_grid)) {;}
    double holes = count_holes(new_grid);
    double max_holes = 30; // reasonably. The theoretical maximum is 150
    double min_holes = 0;
    holes = 2 * (holes - min_holes) / (max_holes - min_holes) - 1;
    double aggregate = aggregate_height(new_grid);
    aggregate = 2 * aggregate / 200 - 1;
    double bump = bumpiness(new_grid);
    double max_bump = 500; // reasonably. The theoretical maximum is 3600
    bump = 2 * bump / max_bump - 1;
    std::vector<double> hidden_layer(5, 0);
    double score = 0;
    for (int i = 0; i < W2.size(); i++) {
        hidden_layer[i] = tanh_func(W1[i][0] * holes + W1[i][1] * aggregate + W1[i][2] * lines + W1[i][3] * bump + W1[i][4]);
        score += W2[i] * hidden_layer[i];
    }
    return tanh_func(score);
}

std::vector<char> ai(
    std::vector<std::vector<char> > board,
    std::vector<std::pair<int, int> > piece_location,
    char piece, char hold, char next,
    std::map<char, std::vector<std::pair<int, int> > > start_positions,
    std::vector<std::vector<double> > weights1 = {{1, 0, 0, 0, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 0, 0}},
    std::vector<double> weights2 = {-0.0015, -0.000001, 1, -0.000015, 0}) {
    assert(weights1.size() == 5 && weights2.size() == 5);
    assert(weights1[0].size() == 5);

    /*
    std::cout << "Preparing inputs... \n";

    std::cout << "board: \n";
    for (auto& row : board) {
        for (char c : row) {
            std::cout << c << " ";
        }
        std::cout << "\n";
    }

    std::cout << "piece: " << piece << std::endl;
    std::cout << "piece loc: ";
    for (auto& p : piece_location) {
        std::cout << "(" << p.first << ", " << p.second << "),  ";
    }
    std::cout << "\n";
    */

    std::vector<std::vector<char> > grid = convert_board(board, piece_location);
    /*
    std::cout << "Convert successful\n";
    for (auto& row : grid) {
        for (char c : row) {
            std::cout << c << " ";
        }
        std::cout << "\n";
    }
    */
    // std::vector<std::vector<double> > weights1(5, std::vector<double>(4, 0));
    // weights1 = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}};
    // std::vector<double> weights2(5, 0);
    // weights2 = {-1500, -1, 1000000, -15, 0};

    // std::cout << "Generating best move... ";
    std::vector<char> res = generate_best_move(grid, piece_location, piece, weights1, weights2, hold, next, start_positions);
    // std::cout << "Generate best move successful: ";
    // for (char r : res) {std::cout << r << " ";}
    // std::cout << "\n";
    res.push_back(' ');
    return res;
}

#endif //AI_H
