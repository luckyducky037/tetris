#include "engine.cpp"
#include "weightsio.h"
#include <random>
#include <sstream>
#include <time.h>
#include <map>

typedef std::vector<std::vector<double> > W1;
typedef std::vector<double> W2;
typedef std::pair<W1, W2> W;

W generate() {
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_real_distribution<double> distribution(-1, 1);

	W weights;
	for (int i = 0; i < 5; i++) {
        weights.first.push_back(std::vector<double>(5, 0));
		for (int j = 0; j < 5; j++) {
			weights.first[i][j] = distribution(generator);
		}
	}
    weights.second = std::vector<double>(5, 0);
	for (int i = 0; i < 5; i++) {
		weights.second[i] = distribution(generator);
	}
	return weights;
}

std::vector<W> generate_population(int n) {
    std::vector<W> population(n);
    for (int i = 0; i < n; i++) {
        population[i] = generate();
    }
    return population;
}

void write_population(std::vector<W> population, std::string filename) {
    std::ofstream population_file;
    population_file.open(filename);
    for (int i = 0; i < population.size(); i++) {
        for (int j = 0; j < population[i].first.size(); j++) {
            for (int k = 0; k < population[i].first[j].size(); k++) {
                population_file << population[i].first[j][k] << " ";
            }
        }
        for (int j = 0; j < population[i].second.size(); j++) {
            population_file << population[i].second[j] << " ";
        }
        population_file << std::endl;
    }
    population_file.close();
}

long long fitness(W weights) {
    std::vector<char> piece_list;
    std::ifstream read_file("piece_list.txt");
    std::string line;
    int pieces = 0;
    while (std::getline(read_file, line)) {
        piece_list.push_back(line[0]);
        pieces++;
    }
    read_file.close();
    int score = mainloop(true, piece_list, weights.first, weights.second);
    if (pieces >= piece_list.size()) {
        return score;
    }
    std::ofstream write_file("piece_list.txt");
    for (char c : piece_list) {
        write_file << c << "\n";
    }
    write_file.close();
    return score;
}

W mutate(W weights, double lr) {
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_real_distribution<double> distribution(-lr, lr);

    for (auto& vec : weights.first) {
		for (auto& i : vec) {
        	i += distribution(generator);
		}
    }
    for (auto& i : weights.second) {
        i += distribution(generator);
    }

    return weights;
}

W crossover(W weights1, W weights2) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0, 1);

    W weights = std::make_pair(std::vector(5, std::vector<double>(5, 0)), std::vector<double>(5, 0));

    for (int i = 0; i < weights1.first.size(); i++) {
        for (int j = 0; j < weights1.first[0].size(); j++) {
            double d = distribution(generator);
            weights.first[i][j] = d * weights1.first[i][j] + (1 - d) * weights2.first[i][j];
        }
    }
    for (int i = 0; i < weights1.second.size(); i++) {
        double d = distribution(generator);
        weights.second[i] = d * weights1.second[i] + (1 - d) * weights2.second[i];
    }

    return weights;
}

void print_weights(W weights) {
    std::cout << "W1: ";
    for (int j = 0; j < weights.first.size(); j++) {
        for (double k : weights.first[j]) {
            std::cout << k << " ";
        }
        std::cout << "| ";
    }
    std::cout << std::endl;
    std::cout << "W2: ";
    for (double j : weights.second) {
        std::cout << j << " ";
    }
    std::cout << std::endl;
}

void print_population(std::vector<W> population) {
    std::cout << "Population size: " << population.size() << std::endl;
    for (int i = 0; i < population.size(); i++) {
        std::cout << "#" << i + 1 << std::endl;
        W weights = population[i];
        print_weights(weights);
        std::cout << "----------\n";
    }
}

void train(int gen) {
    std::vector<W> population;
    int total;
    if (gen == 1) {
        char reinit;
        std::cout << "Reinitialize? (Y/N): ";
        std::cin >> reinit;
        if (reinit == 'N') {
            population = retrieve_weights_vector("weights/w0.txt");
            total = population.size();
        } else if (reinit == 'Y') {
            std::cout << "Population size: ";
            std::cin >> total;
            population = generate_population(total);
            std::cout << "Initializing weights to w0.txt" << std::endl;
            write_population(population, "weights/w0.txt");
        }
    } else {
        std::string filename = "weights/w" + std::to_string(gen - 1) + ".txt";
		population = retrieve_weights_vector(filename);
        total = population.size();
    }
	std::map<W, long long> fitnesses;
    std::ofstream write_file("piece_list.txt");
    write_file.close();
    for (W weight : population) {
        long long score = fitness(weight);
        fitnesses[weight] = score;
    }
    std::multimap<long long, W> fitness_rank;
    for (auto& p : fitnesses) {
        fitness_rank.insert(std::make_pair(p.second, p.first));
    }
    std::vector<W> fitness_vec;
    int rank = total;
    for (auto& p : fitness_rank) {
        std::cout << "#" << rank << std::endl;
        print_weights(p.second);
        std::cout << "Score: " << p.first << "\n";
        rank--;
        fitness_vec.push_back(p.second);
    }


    std::vector<W> new_population;
    int it = 0;
    int bad_cut = total * 0.4;
    int min_keep = total * 0.9;
    double lr = 1.0 / static_cast<double>(gen);
    int num = 0;
    for (auto& p : fitness_rank) {
        if (it < bad_cut) {
            it++;
            continue;
        }
        if (it < min_keep) {
            new_population.push_back(mutate(p.second, lr));
            num++;
        } else {
            new_population.push_back(p.second);
            new_population.push_back(mutate(p.second, lr));
            new_population.push_back(mutate(p.second, lr));
            num += 3;
        }
        it++;
    }
    srand(time(0));
    while (num < total) {
        int i = rand() % (total / 10);
        int j = i;
        while (i == j) {
            j = rand() % (total / 10);
        }
        new_population.push_back(crossover(fitness_vec[total - 1 - i], fitness_vec[total - 1 - j]));
        num++;
    }
    print_population(new_population);
    std::string filename = "weights/w" + std::to_string(gen) + ".txt";
    std::ofstream outfile (filename);
    outfile.close();
    std::cout << "Writing weights to " << filename << std::endl;
    write_population(new_population, filename);
}