#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

using namespace std;

int main() {
    // Define the sets of possible values for a and b
    vector<int> a_values = {192};
    vector<int> b_values = {192};

    // Generate a-values of the form 12k + 8, for k = 0 to 15
    for (int k = 0; k <= 15; ++k) {
        a_values.push_back(12 * k + 8);
    }

    // Generate b-values of the form 12k + 9, for k = 0 to 15
    for (int k = 0; k <= 15; ++k) {
        b_values.push_back(12 * k + 9);
    }

    // Set up random number generators
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> a_dist(0, a_values.size() - 1);
    uniform_int_distribution<> b_dist(0, b_values.size() - 1);
    uniform_int_distribution<> size_dist(1, 10); // size is between 1 and 10
    bernoulli_distribution timestamp_increment_dist(0.001); // 0.1% chance of 1, 99.9% chance of 0

    // Define how many query pairs to generate
    int num_queries;
    cout << "Please enter the number of query pairs to generate: ";
    cin >> num_queries;

    // Open the output file
    ofstream outfile("queries.txt");
    if (!outfile.is_open()) {
        cerr << "Error: Unable to open file queries.txt for writing." << endl;
        return 1;
    }

    // Initialize timestamp
    int timestamp = 0;

    // Generate and write query pairs
    for (int i = 0; i < num_queries; ++i) {
        int a = a_values[a_dist(gen)];
        int b = b_values[b_dist(gen)];

        // If a == 192 and b == 192, regenerate b
        while (a == 192 && b == 192) {
            b = b_values[b_dist(gen)];
        }

        // Ensure a != b
        while (a == b) {
            b = b_values[b_dist(gen)];
        }

        // Update timestamp
        timestamp += timestamp_increment_dist(gen);

        // Write to file
        outfile << a << " " << b << " " << size_dist(gen) << " " << timestamp << "\n";
    }

    outfile.close();
    cout << "Successfully generated " << num_queries << " query pairs and saved them to queries.txt." << endl;

    return 0;
}
