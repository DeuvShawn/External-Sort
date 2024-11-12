#include <iostream>
#include <fstream>
#include <cstdlib>  // 用于rand()和srand()
#include <ctime>    // 用于time()

using namespace std;

// 生成测试文件，包含指定数量的随机整数
void generateTestFile(const string& filename, int numIntegers) {
    ofstream outFile(filename); // 打开输出文件
    if (!outFile) {
        cerr << "无法创建文件: " << filename << endl;
        return;
    }

    srand(static_cast<unsigned int>(time(0))); // 设置随机数种子
    for (int i = 0; i < numIntegers; ++i) {
        int randomNum = rand() % 100000; // 生成0到99999之间的随机数
        outFile << randomNum << endl;    // 写入文件并换行
    }

    outFile.close(); // 关闭文件
    cout << "测试文件已生成: " << filename << "，包含" << numIntegers << "个随机整数。" << endl;
}

int main() {
    string filename = "data.txt";
    int numIntegers = 50000; // 可以根据需要调整生成的整数数量

    generateTestFile(filename, numIntegers); // 生成测试文件

    return 0;
}
