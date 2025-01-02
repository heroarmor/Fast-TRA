#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>
#include "channel_name.h"
#include "ftra.h"
#include <dirent.h>
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
void runMeshSingleThread(int mode,bool go,int w,int h,int n,int r,int index){
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
    best_placement=all_placements.at(index);
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
        if(mode & 32){
            setTurnsDirect1(G,brl,res);
        }
        else{
            if(mode & 8){
                if(mode & 16) sortAllTurns(G,bt,mode & 4,mode & 3);
                setTurnsBkwd(G,bt,pt,(int)(bt.size()>>1),0,0,r,res);
            }
            else setTurns(G,bt,pt,0,(int)(bt.size()>>1)-1,0,r,res);
        }
        //whether to clear results
        if(!go){
            res.ofv = 0; res.itc = 0; res.os.clear();
        }
    fout.flush();
    fout.close();
}

struct DataRow {
    double ofv;
    int idx;
    std::string line;
};

int getmaxindex() {
    // 打开数据文件
    std::ifstream file("./output_mesh_w4_h4_n4/st_go0_mode32_r6.txt"); // 确保文件路径正确
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    std::string header;
    // 读取标题行
    if (std::getline(file, header)) {
        // 成功读取标题行
    } else {
        std::cerr << "文件为空或无法读取标题行" << std::endl;
        return 1;
    }

    DataRow maxRow;
    maxRow.ofv = -std::numeric_limits<double>::infinity();
    maxRow.line = "";

    std::string line;
    // 读取数据行并找到最大 ofv 的行
    while (std::getline(file, line)) {
        if (line.empty()) continue; // 跳过空行

        std::istringstream iss(line);
        std::string idx_str, ofv_str;

        // 读取 idx 和 ofv 字段
        if (!(iss >> idx_str >> ofv_str)) {
            continue; // 跳过无法读取 idx 和 ofv 的行
        }

        // 尝试将 ofv 转换为 double
        double current_ofv;
        int current_idx;
        try {
            current_ofv = std::stod(ofv_str);
            current_idx = std::stoi(idx_str);
        } catch (const std::exception& e) {
            continue; // 跳过无法转换的行
        }

        // 更新最大 ofv 的行
        if (current_ofv > maxRow.ofv) {
            maxRow.ofv = current_ofv;
            maxRow.idx = current_idx;
            maxRow.line = line;
        }
    }

    file.close(); // 关闭文件

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
    runMeshSingleThread(_MODE_,_GO_,_W_,_H_,_N_,_R_,max_idx);
    return 0;
}
