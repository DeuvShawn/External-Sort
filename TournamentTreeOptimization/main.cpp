#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>

using namespace std;

// 败者树类
class LoserTree {
private:
    int losers[2]; // 对于二路归并，只有两个叶子节点
    int keys[2];   // 存储两个元素的值

public:
    // 初始化败者树
    void initialize(int key0, int key1) {
        keys[0] = key0;
        keys[1] = key1;

        // 比较两个元素，构建败者树
        if (keys[0] < keys[1]) {
            losers[0] = 1; // 1 号元素是失败者
            losers[1] = 0; // 0 号元素是胜者
        } else {
            losers[0] = 0;
            losers[1] = 1;
        }
    }

    // 返回当前的胜者索引
    int winner() const {
        return losers[1];
    }

    // 更新败者树
    void update(int index, int newKey) {
        keys[index] = newKey;

        // 重新比较，更新败者和胜者
        if (keys[0] < keys[1]) {
            losers[0] = 1;
            losers[1] = 0;
        } else {
            losers[0] = 0;
            losers[1] = 1;
        }
    }

    // 获取指定索引的键值
    int getKey(int index) const {
        return keys[index];
    }
};

// 辅助函数：读取一个块的数据并排序
void readAndSortBlock(ifstream& input, vector<int>& buffer, int bufferSize) {
    buffer.clear();
    int num;
    while (buffer.size() < bufferSize && input >> num) {
        buffer.push_back(num);
    }
    sort(buffer.begin(), buffer.end());
}

// 二路归并两个有序文件，使用败者树优化
void mergeFiles(const string& file1, const string& file2, const string& outputFile) {
    ifstream input1(file1);
    ifstream input2(file2);
    ofstream output(outputFile);

    if (!input1.is_open() || !input2.is_open()) {
        cerr << "无法打开输入文件进行归并！" << endl;
        return;
    }

    int value1, value2;
    bool hasValue1 = bool(input1 >> value1);
    bool hasValue2 = bool(input2 >> value2);

    LoserTree lt;
    if (hasValue1 && hasValue2) {
        lt.initialize(value1, value2);
    } else if (hasValue1) {
        lt.initialize(value1, INT_MAX); // 如果其中一个文件读完，用最大值填充
        hasValue2 = false;
    } else if (hasValue2) {
        lt.initialize(INT_MAX, value2);
        hasValue1 = false;
    } else {
        // 两个文件都为空
        return;
    }

    while (hasValue1 || hasValue2) {
        int winnerIndex = lt.winner();
        int winnerValue = lt.getKey(winnerIndex);
        output << winnerValue << endl;

        // 从对应的输入流中读取下一个值
        if (winnerIndex == 0 && hasValue1) {
            if (input1 >> value1) {
                lt.update(0, value1);
            } else {
                lt.update(0, INT_MAX);
                hasValue1 = false;
            }
        } else if (winnerIndex == 1 && hasValue2) {
            if (input2 >> value2) {
                lt.update(1, value2);
            } else {
                lt.update(1, INT_MAX);
                hasValue2 = false;
            }
        }
    }

    input1.close();
    input2.close();
    output.close();

    // 合并后可以删除原始文件
    remove(file1.c_str());
    remove(file2.c_str());
}

// 外部排序实现，使用二路归并和败者树
void externalSort(const string& inputFile, const string& outputFile, int bufferSize) {
    ifstream input(inputFile);
    if (!input.is_open()) {
        cerr << "无法打开输入文件！" << endl;
        return;
    }

    vector<string> tempFiles;  // 存储临时文件名
    vector<int> buffer;

    // Step 1: 分块读取并排序
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

    // Step 2: 二路归并多个有序文件，使用败者树
    while (tempFiles.size() > 1) {
        vector<string> newTempFiles;

        // 每次合并两个文件
        for (size_t i = 0; i < tempFiles.size(); i += 2) {
            if (i + 1 < tempFiles.size()) {
                string outputTempFile = "result/temp_merge_" + to_string(chunkIndex++) + ".txt";
                mergeFiles(tempFiles[i], tempFiles[i + 1], outputTempFile);
                newTempFiles.push_back(outputTempFile);
            } else {
                // 奇数个文件，最后一个文件直接加入
                newTempFiles.push_back(tempFiles[i]);
            }
        }
        tempFiles = newTempFiles;
    }

    // 最后的文件即为排序结果，重命名为输出文件
    if (!tempFiles.empty()) {
        rename(tempFiles[0].c_str(), outputFile.c_str());
        tempFiles.clear();
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
