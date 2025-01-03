#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>
#include "channel_name.h"
#include "ftra.h"
#include <dirent.h>
#include <unordered_map>
using namespace std;

/* user defined hyper parameters begin */
int _W_ = 0;
int _H_ = 0;
int _N_ = 0;
int _R_ = 0;
int _MODE_ = 0;
bool _GO_ = false;
/* user defined hyper parameters end */

vector<vector<int>> all_placements;
vector<int> best_placement;
vector<mydata> data_list;
unordered_map<int, unordered_map<int, vector<int>>> paths;
string output_path,file_name;
/*----------------------------------------------------------------------------------
    mode:the searching method
        0   :forward searching without presorting
        8   :backward searching without presorting
        25  :backward searching with forward 1/AD presorting
        26  :backward searching with forward AR/1 presorting
        27  :backward searching with forward AR/AD presorting
        29  :backward searching with backward 1/AD presorting
        30  :backward searching with backward AR/1 presorting
        31  :backward searching with backward AR/AD presorting
        32  :Direct-TRA method

    go:global optimization
        true:global optimal value marking
        false:partitional optimal value marking

    w:network width
    h:network height
    n:number of boundary routers
    r:maximum turn restriction number
----------------------------------------------------------------------------------*/

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

unordered_map<int, unordered_map<int, vector<int>>> read() {
    string path_file = "./4*4mesh/routetable.txt";
    ifstream inputPathFile(path_file);
    if (!inputPathFile.is_open()) {
        cerr << "Error: Unable to open file " << path_file << endl;
        exit(1);
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
    return paths;
}

vector<mydata> read_queries(){
    string query_file = "queries.txt";
    ifstream inputQueryFile(query_file);
    if (!inputQueryFile.is_open()) {
        cerr << "Error: Unable to open file " << query_file << endl;
        exit(1);
    }
    string line;
    // 处理每个查询
    while (getline(inputQueryFile, line)) {
        if (line.empty()) continue; // 跳过空行
        stringstream ss(line);
        int source, dest, size, timestamp;
        ss >> source >> dest >> size >> timestamp;
        data_list.push_back({source, dest, size, timestamp});
        if (ss.fail()) {
            cerr << "Warning: Invalid query format: " << line << endl;
            continue;
        }
    }
    inputQueryFile.close(); // 关闭查询文件
    return data_list;
}

MeshCdg MeshSingleThread(int mode,bool go,int w,int h,int n,int r,int index){
    string plc_file = "./brp/plcmt_2d_wh" + to_string(_W_) + "_n" + to_string(_N_) + ".txt";
    ifstream fin;
    string ts;
    ofstream fout;
    fout.setf(std::ios::left);
    MeshCdg init_G(w,h),G(w,h);
    vector<int> bt,pt;
    vector<int> brl;
    vector<int> plc;
    struct Results res;
    struct MeshCode code;
    fout.open(file_name);
    fin.open(plc_file);
    int cnt = 0;
    while(getline(fin,ts)){
        cnt ++;
        plc.push_back(atoi(ts.c_str()));
        if(!(cnt % _N_)){
            all_placements.push_back(plc);
            plc.clear();
        }
    }
    best_placement=all_placements.at(index-1);
    auto it = &best_placement; 
    fin.close();
    //output table header
        static int i=0;
        i++;
        G = init_G;
        brl = *it;
        G.setBoundRouters(brl);
        pt.clear();bt.clear();
        G.initBoundTurns(bt);
        setTurnsDirect(G,brl,res);
    fout.flush();
    fout.close();
    return G;
}

struct DataRow {
    double ofv;
    int idx;
    std::string line;
};

int getmaxindex() {
    std::ifstream file("./output_mesh_w4_h4_n4/st_go0_mode32_r6.txt"); // 确保文件路径正确
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }
    std::string header;
    if (std::getline(file, header)) {
    } else {
        std::cerr << "文件为空或无法读取标题行" << std::endl;
        return 1;
    }
    DataRow maxRow;
    maxRow.ofv = -std::numeric_limits<double>::infinity();
    maxRow.line = "";
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue; 
        std::istringstream iss(line);
        std::string idx_str, ofv_str;
        if (!(iss >> idx_str >> ofv_str)) {
            continue;
        }
        double current_ofv;
        int current_idx;
        try {
            current_ofv = std::stod(ofv_str);
            current_idx = std::stoi(idx_str);
        } catch (const std::exception& e) {
            continue; 
        }
        if (current_ofv > maxRow.ofv) {
            maxRow.ofv = current_ofv;
            maxRow.idx = current_idx;
            maxRow.line = line;
        }
    }
    file.close(); 
    if (maxRow.line.empty()) {
        std::cerr << "未找到任何有效的数据行" << std::endl;
        return 1;
    }
    return maxRow.idx;
}

int main(){
    cin >> _W_;
    cin >> _H_;
    cin >> _N_;
    cin >> _R_;
    cin >> _MODE_;
    cin >> _GO_;
    output_path = "./output_mesh_w" + to_string(_W_) + "_h" + 
                            to_string(_H_) + "_n" + to_string(_N_);
    file_name = output_path + "/temp.txt";
    system(("mkdir -p " + output_path).c_str());
    system(("cd "+ output_path).c_str());
    int max_idx=getmaxindex();
    data_list = read_queries();
    paths = read();
    int finaltime = data_list[data_list.size()-1].timestamp;
    MeshCdg G(_W_,_H_);
    G=MeshSingleThread(_MODE_,_GO_,_W_,_H_,_N_,_R_,max_idx); 
    vector<long long> total_edge_weights(G.EdgeSet.size(), 0);
    for(int time=0;time<=finaltime+20;time++){  
        //cout<<"time:"<<time<<endl;
        //int Sum = 0;
        //int sum = 0;
        for(size_t i=0;i<data_list.size();i++){
                int source = data_list[i].source;
                int dest = data_list[i].dest;
                int size = data_list[i].size;
                int timestamp = data_list[i].timestamp;
                int path_length = paths[source][dest].size();
                if(time>timestamp && time<=timestamp+path_length-1){
                    //Sum += size;
                    G.addEdgeweight(size,G.getEdge(paths[source][dest][time-timestamp-1],paths[source][dest][time-timestamp]));
                }
        }
        for(size_t eid = 0; eid < G.EdgeSet.size(); eid++){
            total_edge_weights[eid] += G.EdgeSet[eid].weight;
        }
        vector<Edge> edges = G.EdgeSet;
        if(time==0){
            G.makeweightzero();
        }
        else{
        //for (const auto& edge : edges) {
            //cout << edge.sv << " " << edge.dv << " " << edge.weight << endl;
            //sum += edge.weight;
        //}
            G.makeweightzero();
        }
        //cout<<"Sum:"<<Sum<<endl;
        //cout<<"sum:"<<sum<<endl;    
        
    }
        for(size_t eid = 0; eid < G.EdgeSet.size(); eid++){
        const Edge& edge = G.EdgeSet[eid];
        double avg = static_cast<double> (total_edge_weights[eid]) / (finaltime + 20);
        cout << edge.sv << " " << edge.dv << " " 
             << avg << endl;
    }
    return 0;
}

