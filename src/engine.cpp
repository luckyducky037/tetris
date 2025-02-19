#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include "weightsio.h"
#include "tetris_movement.h"
#include "ai.h"

typedef std::vector<std::vector<double> > W1;
typedef std::vector<double> W2;
typedef std::pair<W1, W2> W;

std::vector<sf::Vertex> make_grid(const float& window_x, const float& window_y, const float& grid_buffer_x, const float& grid_buffer_y, const float& block_width) {
    sf::Vector2f corner1 = sf::Vector2f(grid_buffer_x, grid_buffer_y);
    sf::Vector2f corner2 = sf::Vector2f(window_x - grid_buffer_x, grid_buffer_y);
    sf::Vector2f corner3 = sf::Vector2f(window_x - grid_buffer_x, window_y - grid_buffer_y);
    sf::Vector2f corner4 = sf::Vector2f(grid_buffer_x, window_y - grid_buffer_y);
    std::vector<sf::Vertex> grid =
    {
        sf::Vertex(corner1),
        sf::Vertex(corner2),
        sf::Vertex(corner2),
        sf::Vertex(corner3),
        sf::Vertex(corner3),
        sf::Vertex(corner4),
        sf::Vertex(corner4),
        sf::Vertex(corner1),
    };

    std::vector<sf::Vertex> horizontal_gridlines;
    std::vector<sf::Vertex> vertical_gridlines;

    for (float i = 1; i < 10; i++) {
        horizontal_gridlines.push_back(sf::Vertex(sf::Vector2f(grid_buffer_x + i * block_width, grid_buffer_y)));
        horizontal_gridlines.push_back(sf::Vertex(sf::Vector2f(grid_buffer_x + i * block_width, window_y - grid_buffer_y)));
    }

    for (float i = 1; i < 20; i++) {
        vertical_gridlines.push_back(sf::Vertex(sf::Vector2f(grid_buffer_x, i * block_width + grid_buffer_y)));
        vertical_gridlines.push_back(sf::Vertex(sf::Vector2f(window_x - grid_buffer_x, i * block_width + grid_buffer_y)));
    }

    grid.insert(grid.end(), std::make_move_iterator(horizontal_gridlines.begin()), std::make_move_iterator(horizontal_gridlines.end()));
    grid.insert(grid.end(), std::make_move_iterator(vertical_gridlines.begin()), std::make_move_iterator(vertical_gridlines.end()));

    return grid;
}

void color_grid(sf::RenderWindow& window, std::map<char, sf::Color>& color_map, std::vector<std::vector<char> > pieces, const float& window_x, const float& window_y, const float& grid_buffer_x, const float& grid_buffer_y, const float& block_width) {
    sf::RectangleShape block;
    const int block_buffer = 2;
    block.setSize(sf::Vector2f(block_width - block_buffer * 2.5, block_width - block_buffer * 2.5));
    for (int row = 0; row < 20; row++) {
        for (int col = 0; col < 10; col++) {
            block.setPosition(sf::Vector2f(grid_buffer_x + col * block_width + block_buffer, grid_buffer_y + row * block_width + block_buffer));
            block.setFillColor(color_map[pieces[row+4][col]]);
            window.draw(block);
        }
    }
}

void remove_piece(const std::vector<std::pair<int, int> >& piece_location, std::vector<std::vector<char> >& pieces) {
    for (auto p : piece_location) {
        pieces[p.first][p.second] = '.';
    }
}

void add_piece(const std::vector<std::pair<int, int> >& piece_location, std::vector<std::vector<char> >& pieces, const char& current_piece) {
    for (auto p : piece_location) {
        pieces[p.first][p.second] = current_piece;
    }
}

void display_box(float leftmost_x, float topmost_y, sf::RenderWindow& window, std::map<char, sf::Color>& color_map, const float& window_x, const float& window_y, const float& grid_buffer_x, const float& grid_buffer_y, const float& block_width, char piece, std::map<char, std::vector<std::pair<int, int> > > start_positions, const int shift) {
    sf::Vector2f corner1 = sf::Vector2f(leftmost_x, topmost_y);
    sf::Vector2f corner2 = sf::Vector2f(leftmost_x + 6 * block_width, topmost_y);
    sf::Vector2f corner3 = sf::Vector2f(leftmost_x + 6 * block_width, topmost_y + 4 * block_width);
    sf::Vector2f corner4 = sf::Vector2f(leftmost_x, topmost_y + 4 * block_width);
    sf::Vertex frame[8] =
    {
        sf::Vertex(corner1),
        sf::Vertex(corner2),
        sf::Vertex(corner2),
        sf::Vertex(corner3),
        sf::Vertex(corner3),
        sf::Vertex(corner4),
        sf::Vertex(corner4),
        sf::Vertex(corner1),
    };

    std::vector<std::vector<char> > pieces(4, std::vector<char>(6, '.'));
    for (auto p : start_positions[piece]) {
        pieces[p.first + 1 - shift][p.second - 1] = piece;
    }

    sf::RectangleShape background;
    background.setPosition(sf::Vector2f(leftmost_x, topmost_y));
    background.setSize(sf::Vector2f(6 * block_width, 4 * block_width));
    background.setFillColor(color_map['.']);
    window.draw(background);

    sf::RectangleShape block;
    const int block_buffer = 2;
    block.setSize(sf::Vector2f(block_width - block_buffer * 2, block_width - block_buffer * 2));
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 6; col++) {
            block.setPosition(sf::Vector2f(leftmost_x + ((piece == 'I' || piece == 'O') ? col - 1 : col - 0.5) * block_width + block_buffer, topmost_y + ((piece == 'I') ? row + 0.5 : row) * block_width + block_buffer));
            block.setFillColor(color_map[pieces[row][col]]);
            window.draw(block);
        }
    }

    window.draw(frame, 8, sf::Lines);
}

long long mainloop(bool ai_playing, std::vector<char>& piece_list, W1 weights1 = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {0, 0, 0, 0}},
    W2 weights2 = {-0.0015, -0.000001, 1, -0.000015, 0}) {
    const float window_x = 780;
    const float grid_buffer_x = 240;
    const float block_width = (window_x - 2 * grid_buffer_x) / 10;
    const float grid_buffer_y = grid_buffer_x / 4;
    const float window_y = 20 * block_width + 2 * grid_buffer_y;

    sf::RenderWindow window(sf::VideoMode(window_x, window_y), "Tetris");

    std::vector<sf::Vertex> vec_grid = make_grid(static_cast<float>(window_x), static_cast<float>(window_y), grid_buffer_x, grid_buffer_y, block_width);

    std::vector<char> piece_types = {'O', 'T', 'J', 'L', 'Z', 'S', 'I'};
    std::map<char, std::vector<std::pair<int, int> > > start_positions;
    const int shift = 0;
    start_positions['O'] = {
        std::make_pair(0+shift, 4),
        std::make_pair(0+shift, 5),
        std::make_pair(1+shift, 4),
        std::make_pair(1+shift, 5)};
    start_positions['T'] = {
        std::make_pair(1+shift, 4),
        std::make_pair(0+shift, 4),
        std::make_pair(1+shift, 3),
        std::make_pair(1+shift, 5)};
    start_positions['J'] = {
        std::make_pair(1+shift, 4),
        std::make_pair(1+shift, 5),
        std::make_pair(1+shift, 3),
        std::make_pair(0+shift, 3)};
    start_positions['L'] = {
        std::make_pair(1+shift, 4),
        std::make_pair(1+shift, 3),
        std::make_pair(1+shift, 5),
        std::make_pair(0+shift, 5)};
    start_positions['Z'] = {
        std::make_pair(1+shift, 4),
        std::make_pair(0+shift, 4),
        std::make_pair(0+shift, 3),
        std::make_pair(1+shift, 5)};
    start_positions['S'] = {
        std::make_pair(1+shift, 4),
        std::make_pair(0+shift, 4),
        std::make_pair(1+shift, 3),
        std::make_pair(0+shift, 5)};
    start_positions['I'] = {
        std::make_pair(0+shift, 4),
        std::make_pair(0+shift, 5),
        std::make_pair(0+shift, 3),
        std::make_pair(0+shift, 6)};
    std::map<char, sf::Color> color_map;
    color_map['.'] = sf::Color(0, 0, 0);
    color_map['O'] = sf::Color(255, 255, 0);
    color_map['T'] = sf::Color(128, 0, 128);
    color_map['J'] = sf::Color(0, 0, 255);
    color_map['L'] = sf::Color(255, 127, 0);
    color_map['Z'] = sf::Color(255, 0, 0);
    color_map['S'] = sf::Color(0, 255, 0);
    color_map['I'] = sf::Color(0, 255, 255);

    std::vector<std::vector<char> > pieces(24, std::vector<char>(10, '.'));

    char current_piece = '.';
    std::vector<std::pair<int, int> > piece_location(4, std::make_pair(-1, -1));

    sf::Clock Clock;
    sf::Clock KeyClock;
    sf::Time Time = Clock.getElapsedTime();
    sf::Time KeyBuffer = KeyClock.getElapsedTime();
    float UpdateBuffer = 1000;
    sf::Time MinBuffer = sf::milliseconds(50);
    Clock.restart();

    srand(time(0));

    bool game_over = false;
    int piece_number = 1;
    char next_piece;
    if (piece_list.size() != 0) {
        next_piece = piece_list[0];
    } else {
        next_piece = piece_types[rand() % 7];
        piece_list.push_back(next_piece);
    }
    char hold_piece = '.';
    bool recent_switch = false;
    char last_key_pressed = '.';
    bool no_press = true;
    long long score = 0;
    bool last_tet = false;
    long long lines = 0;
    // bool ai_playing = true;
    bool next_piece_on_board = false;
    std::vector<char> moves;

    while (window.isOpen())
    {
        Time = Clock.getElapsedTime();
        KeyBuffer = KeyClock.getElapsedTime();

        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                // std::cout << "Key pressed: " << event.key.code << std::endl;
                no_press = false;
                if (ai_playing) {
                    ;
                } else if (event.key.code == sf::Keyboard::Left) {
                    last_key_pressed = 'L';
                } else if (event.key.code == sf::Keyboard::Right) {
                    last_key_pressed = 'R';
                } else if (event.key.code == sf::Keyboard::Up) {
                    last_key_pressed = 'U';
                } else if (event.key.code == sf::Keyboard::Down) {
                    last_key_pressed = 'D';
                } else if (event.key.code == sf::Keyboard::Space) {
                    last_key_pressed = ' ';
                } else if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift || event.key.code == sf::Keyboard::C) {
                    last_key_pressed = 'S';
                }
            } else {
                no_press = true;
            }
        }

        if (Time >= sf::milliseconds(UpdateBuffer)) {
            if (ai_playing) {
                no_press = true;
            }
            if (current_piece == '.') {
                next_piece_on_board = true;
                current_piece = next_piece;
                if (piece_list.size() <= piece_number) {
                    next_piece = piece_types[rand() % 7];
                    piece_list.push_back(next_piece);
                } else {
                    next_piece = piece_list[piece_number];
                }
                piece_number++;

                piece_location = start_positions[current_piece];
                for (auto p : piece_location) {
                    if (pieces[p.first][p.second] != '.') {
                        // std::cout << "Top is reached; game is over" << std::endl;
                        game_over = true;
                        break;
                    }
                    pieces[p.first][p.second] = current_piece;
                }
                recent_switch = false;

                if (game_over) {
                    window.close();
                    std::cout << "Game Over. Your final score is " << score << " with " << lines << " lines cleared.\n";
                    break;
                }
            } else if (bottom(piece_location, pieces)) {
                // std::cout << "Reached bottom" << std::endl;
                score += 1; // This is used to encourage optimal stacking
                // in the early stages of training
                // even if no lines were cleared.
                int i = 0;
                while (clear_line(pieces)) {i++;}
                if (i == 1) {
                    score += 100;
                } else if (i == 2) {
                    score += 300;
                } else if (i == 3) {
                    score += 500;
                } else if (i == 4) {
                    score += 800 * (last_tet ? 1.5 : 1);
                    last_tet = true;
                }
                if (i != 4) {last_tet = false;}
                lines += i;
                if (ai_playing) {
                    UpdateBuffer = 100;
                } else {
                    UpdateBuffer *= 0.99;
                }

                current_piece = '.';
            } else {
                // std::cout << "Falling down" << std::endl;
                remove_piece(piece_location, pieces);
                for (auto& p : piece_location) {
                    p.first += 1;
                }
                add_piece(piece_location, pieces, current_piece);
            }

            Clock.restart();
        }

        if (KeyBuffer >= MinBuffer && current_piece != '.' && no_press) {
            if (ai_playing && !moves.empty()) {
                next_piece_on_board = false;
                last_key_pressed = moves[0];
                moves.erase(moves.begin());
            } else if (ai_playing && moves.empty() && next_piece_on_board) {
                moves = ai(pieces, piece_location, current_piece, hold_piece, next_piece, weights1, weights2);
                std::cout << "Moves to play: ";
                for (auto m : moves) {
                    std::cout << m << " ";
                }
                std::cout << "\n";
            }
            if (last_key_pressed == 'L'
                || (ai_playing ? false : sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                ) {
                // std::cout << "Left pressed" << std::endl;
                last_key_pressed = '.';
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
                if (!at_edge) {
                    // std::cout << "Moving left" << std::endl;
                    remove_piece(piece_location, pieces);
                    for (auto& p : piece_location) {
                        p.second -= 1;
                    }
                    add_piece(piece_location, pieces, current_piece);
                }
            } else if (last_key_pressed == 'R'
                || (ai_playing ? false : sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                ) {
                // std::cout << "Right pressed" << std::endl;
                last_key_pressed = '.';
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
                if (!at_edge) {
                    // std::cout << "Moving right" << std::endl;
                    remove_piece(piece_location, pieces);
                    for (auto& p : piece_location) {
                        p.second += 1;
                    }
                    add_piece(piece_location, pieces, current_piece);
                }
            } else if (last_key_pressed == 'U'
                || (ai_playing ? false : sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                ) {
                last_key_pressed = '.';
                // std::cout << "Up pressed" << std::endl;
                remove_piece(piece_location, pieces);
                piece_location = rotate(piece_location, pieces, current_piece);
                add_piece(piece_location, pieces, current_piece);
            } else if (last_key_pressed == 'D'
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
                ) {
                // std::cout << "Down pressed" << std::endl;
                last_key_pressed = '.';
                if (!bottom(piece_location, pieces)) {
                    remove_piece(piece_location, pieces);
                    for (auto& p : piece_location) {
                        p.first += 1;
                    }
                    add_piece(piece_location, pieces, current_piece);
                }
            } else if (last_key_pressed == ' '
                || (ai_playing ? false : sf::Keyboard::isKeyPressed((sf::Keyboard::Space)))
                ) {
                // std::cout << "Space pressed" << std::endl;
                last_key_pressed = '.';
                while (!bottom(piece_location, pieces)) {
                    remove_piece(piece_location, pieces);
                    for (auto& p : piece_location) {
                        p.first += 1;
                    }
                    add_piece(piece_location, pieces, current_piece);
                }
            } else if ((last_key_pressed == 'S'
                || (ai_playing ? false : sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                || (ai_playing ? false : sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
                || (ai_playing ? false : sf::Keyboard::isKeyPressed(sf::Keyboard::C))
                ) && !recent_switch) {
                // std::cout << "Hold key pressed" << std::endl;
                last_key_pressed = '.';
                recent_switch = true;
                remove_piece(piece_location, pieces);
                if (hold_piece == '.') {
                    hold_piece = current_piece;
                    current_piece = '.';
                } else {
                    char temp = current_piece;
                    current_piece = hold_piece;
                    hold_piece = temp;
                    piece_location = start_positions[current_piece];
                    for (auto p : piece_location) {
                        if (pieces[p.first][p.second] != '.') {
                            std::cout << "Top is reached; game is over" << std::endl;
                            game_over = true;
                            break;
                        }
                        pieces[p.first][p.second] = current_piece;
                    }
                    if (game_over) {
                        window.close();
                        break;
                    }
                }
            }

            KeyClock.restart();
        }

        window.draw(vec_grid.data(), vec_grid.size(), sf::Lines);

        display_box(window_x - grid_buffer_x + block_width, grid_buffer_y, window, color_map, window_x, window_y, grid_buffer_x, grid_buffer_y, block_width, next_piece, start_positions, shift);
        display_box(block_width, grid_buffer_y, window, color_map, window_x, window_y, grid_buffer_x, grid_buffer_y, block_width, hold_piece, start_positions, shift);
        color_grid(window, color_map, pieces, window_x, window_y, grid_buffer_x, grid_buffer_y, block_width);

        sf::Font font;
        font.loadFromFile("pixel.ttf");

        sf::Text next_text;
        next_text.setString("Next Piece");
        next_text.setFont(font);
        next_text.setCharacterSize(24);
        next_text.setFillColor(sf::Color::White);
        next_text.setPosition(window_x - grid_buffer_x + 2 * block_width, grid_buffer_y);
        window.draw(next_text);

        sf::Text hold_text;
        hold_text.setString("Hold Piece");
        hold_text.setFont(font);
        hold_text.setCharacterSize(24);
        hold_text.setFillColor(sf::Color::White);
        hold_text.setPosition(block_width * 2, grid_buffer_y);
        window.draw(hold_text);

        sf::Text score_header;
        score_header.setString("Score");
        score_header.setFont(font);
        score_header.setCharacterSize(24);
        score_header.setFillColor(sf::Color::White);
        score_header.setPosition(window_x / 2 - score_header.getGlobalBounds().width / 2, grid_buffer_y / 2 - block_width * 1.1);
        window.draw(score_header);

        sf::RectangleShape clear;
        clear.setFillColor(sf::Color::Black);
        clear.setSize(sf::Vector2f(block_width * 4, block_width));
        clear.setOrigin(clear.getSize().x / 2, 0);
        clear.setPosition(window_x / 2, grid_buffer_y / 2 - block_width * 0.1);
        window.draw(clear);

        sf::Text score_text;
        score_text.setString(std::to_string(score));
        score_text.setFont(font);
        score_text.setCharacterSize(24);
        score_text.setFillColor(sf::Color::White);
        score_text.setPosition(window_x / 2 - score_text.getGlobalBounds().width / 2, grid_buffer_y / 2 - block_width * 0.1);
        window.draw(score_text);

        window.display();
    }

    return score;
}