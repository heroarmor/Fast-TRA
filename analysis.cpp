#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <iomanip> // For std::setprecision

using namespace std;

//outside node is (5,5)
pair<int, int> calculate_coordinates(int value) {
    if (value == 192) {
        return {5, 5};
    }
    int x = (value / 12) % 4;
    int y = (value / 12) / 4;
    return {x, y};
}

struct Edge {
    pair<int, int> source;
    pair<int, int> destination;
    double total_c;

    Edge(pair<int, int> src, pair<int, int> dest, double c)
        : source(src), destination(dest), total_c(c) {}
};

void processFile(const string& file_path) {
    vector<Edge> edges;

    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "fail to open file: " << file_path << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        int a, b;
        double c;
        if (!(ss >> a >> b >> c)) {
            cerr << "wrong data format: " << line << endl;
            continue; 
        }

        if ((a / 12 == b / 12) && a != 192 && b != 192) {
            continue;
        }

        pair<int, int> source = calculate_coordinates(a);
        pair<int, int> destination = calculate_coordinates(b);

        bool found = false;
        for (auto& edge : edges) {
            if (edge.source == source && edge.destination == destination) {
                edge.total_c += c; 
                found = true;
                break;
            }
        }

        if (!found) {
            edges.emplace_back(source, destination, c);
        }
    }

    file.close();

    for (const auto& edge : edges) {
        cout << "(" << edge.source.first << ", " << edge.source.second << ") -> ("
             << edge.destination.first << ", " << edge.destination.second << ") Total c: "
             << fixed << setprecision(2) << edge.total_c << endl;
    }
}

int main() {
    string file_path = "./4*4mesh/simulateresult.txt";

    processFile(file_path);

    return 0;
}
