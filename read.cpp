#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip> // output format

using namespace std;
unordered_map<int, unordered_map<int, vector<int>>> paths;

pair<int, int> parsePathHeader(const string& line) {
    size_t pos_from = line.find("path from ");
    if (pos_from == string::npos) {
        return {-1, -1}; 
    }
    pos_from += string("path from ").length();
    size_t pos_to = line.find(" to ", pos_from);
    if (pos_to == string::npos) {
        return {-1, -1}; 
    }
    string start_str = line.substr(pos_from, pos_to - pos_from);
    size_t pos_end = pos_to + string(" to ").length();
    string end_str = line.substr(pos_end);

    int start = stoi(start_str);
    int end = stoi(end_str);
    return {start, end};
}

int main() {
    string path_file = "./4*4mesh/routetable.txt";
    string query_file = "queries.txt";
    ifstream inputPathFile(path_file);
    if (!inputPathFile.is_open()) {
        cerr << "Error: Unable to open file " << path_file << endl;
        return 1;
    }
    string line;
    while (getline(inputPathFile, line)) {
        if (line.empty()) continue; 
        if (line.find("path from") != string::npos) {
            pair<int, int> nodes = parsePathHeader(line);
            int start = nodes.first;
            int end = nodes.second;
            if (start == -1 && end == -1) {
                cerr << "Warning: Invalid path header format: " << line << endl;
                continue;
            }
            if (getline(inputPathFile, line)) {
                stringstream ss(line);
                vector<int> path;
                int node;
                while (ss >> node) {
                    path.push_back(node);
                }
                paths[start][end] = path;
            } else {
                cerr << "Warning: Expected path nodes after \"" << line << "\"" << endl;
                break;
            }
        }
    }

    inputPathFile.close();
    ifstream inputQueryFile(query_file);
    if (!inputQueryFile.is_open()) {
        cerr << "Error: Unable to open file " << query_file << endl;
        return 1;
    }

    // define variables for statistics
    long long total_hops = 0;
    long long total_delay = 0; // in nanoseconds
    long long successful_queries = 0;

    // deal with each query
    while (getline(inputQueryFile, line)) {
        if (line.empty()) continue; // skip empty lines
        stringstream ss(line);
        int source, dest;
        ss >> source >> dest;
        if (ss.fail()) {
            cerr << "Warning: Invalid query format: " << line << endl;
            continue;
        }

        // find the path
        if (paths.find(source) != paths.end() && paths[source].find(dest) != paths[source].end()) {
            const vector<int>& path = paths[source][dest];
            // calculate hop count
            int hop_count = path.size() - 1;
            // calculate total delay
            int total_path_delay = hop_count * 10; // 10 ns per hop

            // output the path
            cout << "Path from " << source << " to " << dest << ": ";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << path[i];
                if (i != path.size() - 1) cout << " ";
            }
            cout << " | Hops: " << hop_count;
            cout << " | Delay: " << total_path_delay << " ns" << endl << endl; // empty line

            // update statistics
            total_hops += hop_count;
            total_delay += total_path_delay;
            successful_queries++;
        } else {
            cout << "No path from " << source << " to " << dest << endl << endl; // empty line
        }
    }

    inputQueryFile.close(); // close the query file

    // output statistics
    if (successful_queries > 0) {
        double average_hops = static_cast<double>(total_hops) / successful_queries;
        double average_delay = static_cast<double>(total_delay) / successful_queries;
        cout << fixed << setprecision(2); // set output format
        cout << "Average Hops: " << average_hops << endl;
        cout << "Average Delay: " << average_delay << " ns" << endl;
    } else {
        cout << "No successful queries to calculate averages." << endl;
    }

    return 0;
}
