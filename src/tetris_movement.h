//
// Created by Leonardo Zhou on 2025-01-11.
//

#ifndef TETRIS_MOVEMENT_H
#define TETRIS_MOVEMENT_H

// #include <iostream>
#include <vector>

bool overlapping(const std::vector<std::pair<int, int> > & piece_location, const std::vector<std::pair<int, int> > & new_piece_location, const std::vector<std::vector<char> >& pieces) {
    for (auto p : new_piece_location) {
        if (!(0 <= p.first && p.first < 24) || !(0 <= p.second && p.second < 10)) {
            return true;
        }
        if (pieces[p.first][p.second] != '.') {
            bool is_self = false;
            for (auto p2 : piece_location) {
                if (p == p2) {
                    is_self = true;
                }
            }
            if (!is_self) {
                return true;
            }
        }
    }

    return false;
}

bool bottom(const std::vector<std::pair<int, int> > & piece_location, const std::vector<std::vector<char> >& pieces) {
    std::vector<std::pair<int, int> > new_piece_location;

    for (auto p : piece_location) {
        if (p.first == pieces.size() - 1) {
            return true;
        }
        new_piece_location.push_back(std::make_pair(p.first + 1, p.second));
    }

    return overlapping(piece_location, new_piece_location, pieces);
}

bool left(const std::vector<std::pair<int, int> > & piece_location, const std::vector<std::vector<char> >& pieces) {
    bool at_edge = false;
    for (auto& p : piece_location) {
        if (p.second == 0) {
            at_edge = true;
        }
        if (pieces[p.first][p.second - 1] != '.') {
            if ((piece_location[0].first != p.first || piece_location[0].second != p.second - 1) &&
                (piece_location[1].first != p.first || piece_location[1].second != p.second - 1) &&
                (piece_location[2].first != p.first || piece_location[2].second != p.second - 1) &&
                (piece_location[3].first != p.first || piece_location[3].second != p.second - 1))
                at_edge = true;
        }
    }
    return at_edge;

    std::vector<std::pair<int, int> > new_piece_location;

    for (auto p : piece_location) {
        if (p.second == 0) {
            return true;
        }
        new_piece_location.push_back(std::make_pair(p.first, p.second - 1));
    }

    return overlapping(piece_location, new_piece_location, pieces);
}

bool right(const std::vector<std::pair<int, int> > & piece_location, const std::vector<std::vector<char> >& pieces) {
    bool at_edge = false;
    for (auto& p : piece_location) {
        if (p.second == 9) {
            at_edge = true;
        }
        if (pieces[p.first][p.second + 1] != '.') {
            if ((piece_location[0].first != p.first || piece_location[0].second != p.second + 1) &&
                (piece_location[1].first != p.first || piece_location[1].second != p.second + 1) &&
                (piece_location[2].first != p.first || piece_location[2].second != p.second + 1) &&
                (piece_location[3].first != p.first || piece_location[3].second != p.second + 1))
                at_edge = true;
        }
    }

    return at_edge;

    std::vector<std::pair<int, int> > new_piece_location;

    for (auto p : piece_location) {
        if (p.second == 9) {
            return true;
        }
        new_piece_location.push_back(std::make_pair(p.first, p.second + 1));
    }

    return overlapping(piece_location, new_piece_location, pieces);
}

std::vector<std::pair<int, int> > spin(const std::vector<std::pair<int, int> > & piece_location, const char& current_piece) {
    std::vector<std::pair<int, int> > new_piece_location;
    int center_row = piece_location[0].first;
    int center_col = piece_location[0].second;
    int dir_row = piece_location[1].first;
    int dir_col = piece_location[1].second;
    int dx = dir_row - center_row;
    int dy = dir_col - center_col;
    int new_dir_row, new_dir_col;

    if (current_piece == 'I') {
        new_piece_location.push_back(piece_location[1]);
        if (dx == 1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row, dir_col - 1));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col + 1));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col - 2));
        } else if (dx == -1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row, dir_col + 1));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col - 1));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col + 2));
        } else if (dx == 0 && dy == 1) {
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row + 2, dir_col));
        } else if (dx == 0 && dy == -1) {
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row - 2, dir_col));
        }

        return new_piece_location;
    }

    new_piece_location.push_back(piece_location[0]);

    if (dx == 1 && dy == 0) {
        new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col - 1));
    } else if (dx == -1 && dy == 0) {
        new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col + 1));
    } else if (dx == 0 && dy == 1) {
        new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col - 1));
    } else if (dx == 0 && dy == -1) {
        new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col + 1));
    }

    if (current_piece == 'O') {
        return piece_location;
    }
    if (current_piece == 'T') {

        if (dx == 1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row - 2, dir_col));
        } else if (dx == -1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row + 2, dir_col));
        } else if (dx == 0 && dy == 1) {
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col - 2));
        } else if (dx == 0 && dy == -1) {
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col + 2));
        }
    }
    if (current_piece == 'L') {
        if (dx == 1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col + 1));
            new_piece_location.push_back(std::make_pair(dir_row - 2, dir_col + 1));
        } else if (dx == -1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col - 1));
            new_piece_location.push_back(std::make_pair(dir_row + 2, dir_col - 1));
        } else if (dx == 0 && dy == 1) {
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col - 1));
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col - 2));
        } else if (dx == 0 && dy == -1) {
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col + 1));
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col + 2));
        }
    }
    if (current_piece == 'J') {
        if (dx == 1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col + 1));
            new_piece_location.push_back(std::make_pair(dir_row - 0, dir_col + 1));
        } else if (dx == -1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col - 1));
            new_piece_location.push_back(std::make_pair(dir_row + 0, dir_col - 1));
        } else if (dx == 0 && dy == 1) {
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col - 1));
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col - 0));
        } else if (dx == 0 && dy == -1) {
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col + 1));
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col + 0));
        }
    }
    if (current_piece == 'Z') {
        if (dx == 1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row - 2, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col - 1));
        } else if (dx == -1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row + 2, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col + 1));
        } else if (dx == 0 && dy == 1) {
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col - 2));
        } else if (dx == 0 && dy == -1) {
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col + 2));
        }
    }
    if (current_piece == 'S') {
        if (dx == 1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row - 2, dir_col - 1));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
        } else if (dx == -1 && dy == 0) {
            new_piece_location.push_back(std::make_pair(dir_row + 2, dir_col + 1));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
        } else if (dx == 0 && dy == 1) {
            new_piece_location.push_back(std::make_pair(dir_row + 1, dir_col - 2));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
        } else if (dx == 0 && dy == -1) {
            new_piece_location.push_back(std::make_pair(dir_row - 1, dir_col + 2));
            new_piece_location.push_back(std::make_pair(dir_row, dir_col));
        }
    }

    return new_piece_location;
}

std::vector<std::pair<int, int> > rotate(const std::vector<std::pair<int, int> > & piece_location, const std::vector<std::vector<char> >& pieces, const char& current_piece) {
    std::vector<std::pair<int, int> > new_piece_location = spin(piece_location, current_piece);

    if (overlapping(piece_location, new_piece_location, pieces)) {
        return piece_location;
    }

    return new_piece_location;
}

bool clear_line(std::vector<std::vector<char> >& pieces) {
    std::vector<char> empty_row(10, '.');
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (std::find(it->begin(), it->end(), '.') == it->end()) {
            pieces.erase(it);
            pieces.insert(pieces.begin(), empty_row);
            return true;
        }
    }
    return false;
}

#endif //TETRIS_MOVEMENT_H