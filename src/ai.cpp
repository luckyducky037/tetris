#include <iostream>
#include <vector>
#include <algorithm>
#include "tetris_movement.h"

std::vector<std::vector<char> > convert_board(std::vector<std::vector<char> > grid, std::vector<std::pair<int, int> > piece_location) {
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

std::vector<std::vector<char> > grid_and_piece(std::vector<std::vector<char> > grid, std::vector<std::pair<int, int> > piece_location) {
    for (auto& p : piece_location) {
        grid[p.first][p.second] = 'X';
    }
    return grid;
}

int count_holes(std::vector<std::vector<char> > grid) {
    int count = 0;
    for (int i = 1; i < grid.size(); i++) {
        for (int j = 0; j < grid[0].size(); j++) {
            if (grid[i-1][j] == 'X' && grid[i][j] == '.') {
                count++;
            }
        }
    }
    return count;
}

int aggregate_height(std::vector<std::vector<char> > grid) {
    int height = 0;
    for (int j = 0; j < grid[0].size(); j++) {
        int i = 0;
        while (i < grid.size()) {
            if (grid[i][j] == 'X') {
                height += grid.size() - i;
                break;
            }
        }
    }
    return height;
}

int lines_full(std::vector<std::vector<char> > grid) {
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

std::vector<std::pair<int, int> > drop(std::vector<std::vector<char> > grid, std::vector<std::pair<int, int> > piece_location) {
    std::vector<std::pair<int, int> > new_piece_location = piece_location;
    bool bottom = false;

    while (!bottom) {
        for (auto& p : new_piece_location) {
            if (grid[p.first + 1][p.second] == 'X') {
                return new_piece_location;
            }
        }

        for (auto& p : new_piece_location) {
            p.first++;
            if (p.first == grid.size() - 1) {
                bottom = true;
            }
        }
    }

    return new_piece_location;
}

double best(std::vector<double> weights, std::vector<std::vector<char> > grid) {
    return weights[0] * count_holes(grid) + weights[1] * aggregate_height(grid) + weights[2] * lines_full(grid);
}

std::vector<char> generate_best_move(std::vector<std::vector<char> > grid, std::vector<std::pair<int, int> > piece_location, char piece, std::vector<double> weights) {
    std::vector<char> moves;
    moves.push_back('D');
    std::vector<char> best_set = {'N'};
    double best_score = INT_MIN;
    for (int rot = 0; rot < 4; rot++) {
        moves.push_back('L');
        moves.push_back('L');
        moves.push_back('L');
        moves.push_back('L');
        moves.push_back('L');
        moves.push_back('L');
        for (auto& p : piece_location) {
            p.second -= 6;
        }
        for (int x = -6; x <= 6; x++) {
            double curr_score = best(weights, grid_and_piece(grid, drop(grid, piece_location)));
            if (best_score <= curr_score) {
                best_score = curr_score;
                best_set = moves;
            }

            if (x < 0) {
                moves.pop_back();
            } else {
                moves.push_back('R');
            }
            for (auto& p : piece_location) {
                p.second++;
            }
        }
        moves.push_back('U');
        piece_location = spin(piece_location, piece);
    }

    return best_set;
}

std::vector<char> ai(std::vector<std::vector<char> > board, std::vector<std::pair<int, int> > piece_location, char piece, char hold, char next) {
    std::vector<std::vector<char> > grid = convert_board(board, piece_location);
    std::vector<double> weights = {-1, -1, 1};
    std::vector<char> res = generate_best_move(grid, piece_location, piece, weights);
    return res;
}