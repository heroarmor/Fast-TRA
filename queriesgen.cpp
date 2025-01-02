#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <ctime>

using namespace std;

int main() {
    // 定义 a 和 b 的取值集合
    vector<int> a_values = {192};
    vector<int> b_values = {192};
    
    // 生成 12k + 8 形式的 a 值，k = 0 到 15
    for(int k = 0; k <= 15; ++k){
        a_values.push_back(12 * k + 8);
    }

    // 生成 12k + 9 形式的 b 值，k = 0 到 15
    for(int k = 0; k <= 15; ++k){
        b_values.push_back(12 * k + 9);
    }

    // 设置随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> a_dist(0, a_values.size() - 1);
    uniform_int_distribution<> b_dist(0, b_values.size() - 1);

    // 定义要生成的查询对数量
    int num_queries;
    cout << "请输入要生成的查询对数量: ";
    cin >> num_queries;

    // 打开输出文件
    ofstream outfile("queries.txt");
    if(!outfile.is_open()){
        cerr << "Error: Unable to open file queries.txt for writing." << endl;
        return 1;
    }

    // 生成并写入查询对
    for(int i = 0; i < num_queries; ++i){
        int a = a_values[a_dist(gen)];
        int b = b_values[b_dist(gen)];
        
        // 如果 a == b == 192，则重新生成 b
        if(a == 192 && b == 192){
            // 重新生成 b，确保 b != 192
            do{
                b = b_values[b_dist(gen)];
            } while(b == 192);
        }

        // 确保 a != b
        if(a != b){
            outfile << a << " " << b << "\n";
        }
        else{
            // 如果 a == b （除了 192），允许继续
            // 根据您的要求，这种情况可以存在，但a != b已经满足
            outfile << a << " " << b << "\n";
        }
    }

    outfile.close();
    cout << "成功生成 " << num_queries << " 个查询对并保存到 queries.txt 文件中。" << endl;

    return 0;
}
