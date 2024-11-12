#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

// 辅助函数：读取一个块的数据并排序
void readAndSortBlock(ifstream& input, vector<int>& buffer, int bufferSize) {
    buffer.clear();
    int num;
    while (buffer.size() < bufferSize && input >> num) {
        buffer.push_back(num);
    }
    sort(buffer.begin(), buffer.end());
}

// 归并两个有序文件
void mergeTwoFiles(const string& inputFile1, const string& inputFile2, const string& outputFile) {
    ifstream input1(inputFile1);
    ifstream input2(inputFile2);
    ofstream output(outputFile);

    if (!input1.is_open() || !input2.is_open() || !output.is_open()) {
        cerr << "无法打开输入文件进行归并！" << endl;
        return;
    }

    int val1, val2;
    bool hasVal1 = bool(input1 >> val1);
    bool hasVal2 = bool(input2 >> val2);

    while (hasVal1 && hasVal2) {
        if (val1 <= val2) {
            output << val1 << endl;
            hasVal1 = bool(input1 >> val1);
        } else {
            output << val2 << endl;
            hasVal2 = bool(input2 >> val2);
        }
    }

    // 将剩余的元素输出
    while (hasVal1) {
        output << val1 << endl;
        hasVal1 = static_cast<bool>(input1 >> val1);
    }
    while (hasVal2) {
        output << val2 << endl;
        hasVal2 = static_cast<bool>(input2 >> val2);
    }

    input1.close();
    input2.close();
    output.close();
}

// 外部排序实现
void externalSort(const string& inputFile, const string& outputFile, int bufferSize) {
    ifstream input(inputFile);
    if (!input.is_open()) {
        cerr << "无法打开输入文件！" << endl;
        return;
    }

    vector<string> tempFiles;  // 存储临时文件名
    vector<int> buffer;

    // 分块读取并排序
    int chunkIndex = 0;
    while (true) {
        readAndSortBlock(input, buffer, bufferSize);
        if (buffer.empty()) break;

        // 写入到临时文件
        string tempFileName = "result/temp_" + to_string(chunkIndex++) + ".txt";
        tempFiles.push_back(tempFileName);
        ofstream tempFile(tempFileName);
        for (int num : buffer) {
            tempFile << num << endl;
        }
        tempFile.close();
    }
    input.close();

    // 更新 chunkIndex，防止文件名冲突
    chunkIndex = tempFiles.size();

    // 二路归并
    while (tempFiles.size() > 1) {
        vector<string> newTempFiles;
        for (size_t i = 0; i < tempFiles.size(); i += 2) {
            if (i + 1 < tempFiles.size()) {
                // 有两个文件，进行归并
                string mergedFile = "result/temp_merge_" + to_string(chunkIndex++) + ".txt";
                mergeTwoFiles(tempFiles[i], tempFiles[i + 1], mergedFile);
                newTempFiles.push_back(mergedFile);

                // 删除已经归并的临时文件
                remove(tempFiles[i].c_str());
                remove(tempFiles[i + 1].c_str());
            } else {
                // 如果是奇数个文件，最后一个文件直接加入下一轮
                newTempFiles.push_back(tempFiles[i]);
            }
        }
        tempFiles.swap(newTempFiles);
    }

    // 最后的文件即为排序结果，重命名为输出文件并删除临时文件
    if (!tempFiles.empty()) {
        rename(tempFiles[0].c_str(), outputFile.c_str());
        remove(tempFiles[0].c_str()); // 删除临时文件
    }

    cout << "外部排序完成，结果已输出到 " << outputFile << endl;
}

int main() {
    const string inputFile = "result/data.txt";
    const string outputFile = "result/out.txt";
    const int bufferSize = 100; // 假设内存缓冲区大小为 100 个整数

    // 记录开始时间
    auto start = chrono::high_resolution_clock::now();

    externalSort(inputFile, outputFile, bufferSize);

    // 记录结束时间
    auto end = chrono::high_resolution_clock::now();

    // 计算运行时间
    chrono::duration<double> duration = end - start;

    // 输出运行时间
    cout << duration.count() << " s" << endl;
    return 0;
}
