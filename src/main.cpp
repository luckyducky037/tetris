#include "train.cpp"

int main() {
    std::cout << "1 for Player, 2 for AI, 3 for Train: ";
    int choice;
    std::cin >> choice;
    std::vector<char> piece_list = {};
    if (choice == 1) {
        mainloop(false, piece_list);
    } else if (choice == 2) {
        std::cout << "Weight file: ";
        std::string file;
        std::cin >> file;
        std::vector<W> population = retrieve_weights_vector(file);
        std::cout << "Population size: " << population.size() << std::endl;
        int num;
        std::cout << "Enter weight number: ";
        std::cin >> num;
        mainloop(true, piece_list, population[num - 1].first, population[num - 1].second);
    } else if (choice == 3) {
        std::cout << "Generation: ";
        int gen;
        std::cin >> gen;
        train(gen);
    }
}