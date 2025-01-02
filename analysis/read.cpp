#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>

struct DataRow {
    double ofv;
    std::string line;
};

int main() {
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
        try {
            current_ofv = std::stod(ofv_str);
        } catch (const std::exception& e) {
            continue; // 跳过无法转换的行
        }

        // 更新最大 ofv 的行
        if (current_ofv > maxRow.ofv) {
            maxRow.ofv = current_ofv;
            maxRow.line = line;
        }
    }

    file.close(); // 关闭文件

    if (maxRow.line.empty()) {
        std::cerr << "未找到任何有效的数据行" << std::endl;
        return 1;
    }

    // 输出标题行
    std::cout << header << std::endl;

    // 输出具有最大 ofv 的行
    std::cout << maxRow.line << std::endl;

    return 0;
}
