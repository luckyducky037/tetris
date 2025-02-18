#ifndef WEIGHTSIO_H
#define WEIGHTSIO_H

#include <iostream>
#include <random>
#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdio.h>

typedef std::vector<std::vector<double> > W1;
typedef std::vector<double> W2;
typedef std::pair<W1, W2> W;

std::vector<W> retrieve_weights_vector(std::string filename) {
	std::vector<W> weights_vector;
	// std::cout << "Attempting to read file " << filename << " with ifstream: " << std::endl;
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line)) {
		std::cout << line << std::endl;

		W1 weights1(5, std::vector<double>(5, 0));
		W2 weights2(5, 0);

		std::istringstream line_stream(line);
		std::string token;
		int index = 0;

		while (line_stream >> token) {
			double value = std::stod(token);
			if (index < 25) {
				weights1[index / 5][index % 5] = value;
			} else if (index < 30) {
				weights2[index - 25] = value;
			} else {
				std::cerr << "Unexpected number of tokens in line: " << line << "\n";
				assert("Unexpected number of tokens in line");
			}
			++index;
		}

		if (index != 30) {
			std::cerr << "Line does not contain 25 values: " << line << "\n";
			assert("Invalid line format");
		}

		weights_vector.emplace_back(weights1, weights2);
	}
	// std::cout << "Attempt finished.\n";

	return weights_vector;
}


W retrieve_weights(std::string filename) {
	std::ifstream weight_file(filename);
	W1 weights1(5, std::vector<double>(5, 0));
	W2 weights2(5, 0);
	if (weight_file.is_open()) {
		std::cout << "Reading weights from " << filename << "...\n";
		std::string w1;
		getline(weight_file, w1);
		std::vector<std::string> tempw;
		char delim = ' ';
		auto split = [&w1, delim]() {
			std::vector<std::string> tokens;
			size_t start = 0;
			size_t end = w1.find(delim);

			while (end != std::string::npos) {
				tokens.push_back(w1.substr(start, end - start));
				start = end + 1;
				end = w1.find(delim, start);
			}

			tokens.push_back(w1.substr(start));
			return tokens;
		};
		for (int i = 0; i < split().size(); i++) {
			std::string token = split()[i];
			std::cout << token << " ";
			if (i < 25) {
				weights1[i / 5][i % 5] = std::stod(token);
			} else {
				weights2[i - 25] = std::stod(token);
			}
		}
		std::cout << "\n";
	} else {
		std::cout << "Unable to open weights file\n";
		assert("Unable to open weights file");
	}

	return make_pair(weights1, weights2);
}

#endif //WEIGHTSIO_H
