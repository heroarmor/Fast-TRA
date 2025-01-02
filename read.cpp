#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip> // 用于设置输出精度

using namespace std;

// 定义嵌套的 unordered_map 结构
// paths[source][destination] = vector of nodes representing the path
unordered_map<int, unordered_map<int, vector<int>>> paths;

// 函数：解析 "path from X to Y" 行，返回起点和终点
pair<int, int> parsePathHeader(const string& line) {
    // 假设格式始终为 "path from X to Y"
    size_t pos_from = line.find("path from ");
    if (pos_from == string::npos) {
        return {-1, -1}; // 无效格式
    }
    pos_from += string("path from ").length();
    size_t pos_to = line.find(" to ", pos_from);
    if (pos_to == string::npos) {
        return {-1, -1}; // 无效格式
    }
    string start_str = line.substr(pos_from, pos_to - pos_from);
    size_t pos_end = pos_to + string(" to ").length();
    string end_str = line.substr(pos_end);

    int start = stoi(start_str);
    int end = stoi(end_str);
    return {start, end};
}

int main() {
    // 文件名
    string path_file = "temp.txt";
    string query_file = "queries.txt";

    // 打开路径文件
    ifstream inputPathFile(path_file);
    if (!inputPathFile.is_open()) {
        cerr << "Error: Unable to open file " << path_file << endl;
        return 1;
    }

    string line;
    while (getline(inputPathFile, line)) {
        if (line.empty()) continue; // 跳过空行
        if (line.find("path from") != string::npos) {
            // 解析路径头部
            pair<int, int> nodes = parsePathHeader(line);
            int start = nodes.first;
            int end = nodes.second;

            if (start == -1 && end == -1) {
                cerr << "Warning: Invalid path header format: " << line << endl;
                continue;
            }

            // 读取下一行作为路径节点
            if (getline(inputPathFile, line)) {
                stringstream ss(line);
                vector<int> path;
                int node;
                while (ss >> node) {
                    path.push_back(node);
                }
                // 存储路径
                paths[start][end] = path;
            } else {
                cerr << "Warning: Expected path nodes after \"" << line << "\"" << endl;
                break;
            }
        }
    }

    inputPathFile.close(); // 关闭路径文件

    // 打开查询文件
    ifstream inputQueryFile(query_file);
    if (!inputQueryFile.is_open()) {
        cerr << "Error: Unable to open file " << query_file << endl;
        return 1;
    }

    // 定义统计变量
    long long total_hops = 0;
    long long total_delay = 0; // 单位：纳秒
    long long successful_queries = 0;

    // 处理每个查询
    while (getline(inputQueryFile, line)) {
        if (line.empty()) continue; // 跳过空行
        stringstream ss(line);
        int source, dest;
        ss >> source >> dest;
        if (ss.fail()) {
            cerr << "Warning: Invalid query format: " << line << endl;
            continue;
        }

        // 查找路径
        if (paths.find(source) != paths.end() && paths[source].find(dest) != paths[source].end()) {
            const vector<int>& path = paths[source][dest];
            // 计算跳数
            int hop_count = path.size() - 1;
            // 计算总延迟
            int total_path_delay = hop_count * 10; // 每条边延迟为10纳秒

            // 输出路径、跳数和延迟
            cout << "Path from " << source << " to " << dest << ": ";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << path[i];
                if (i != path.size() - 1) cout << " ";
            }
            cout << " | Hops: " << hop_count;
            cout << " | Delay: " << total_path_delay << " ns" << endl << endl; // 空一行

            // 累加统计
            total_hops += hop_count;
            total_delay += total_path_delay;
            successful_queries++;
        } else {
            cout << "No path from " << source << " to " << dest << endl << endl; // 空一行
        }
    }

    inputQueryFile.close(); // 关闭查询文件

    // 计算并输出平均跳数和平均延迟
    if (successful_queries > 0) {
        double average_hops = static_cast<double>(total_hops) / successful_queries;
        double average_delay = static_cast<double>(total_delay) / successful_queries;
        cout << fixed << setprecision(2); // 设置小数点精度为两位
        cout << "Average Hops: " << average_hops << endl;
        cout << "Average Delay: " << average_delay << " ns" << endl;
    } else {
        cout << "No successful queries to calculate averages." << endl;
    }

    return 0;
}
