#include <iostream>
#include <vector>
#include <limits>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstdio>

using namespace std;

//败者树类
class LoserTree {
private:
    int k;
    vector<int> losers;       // 记录败者的数组
    vector<int> keys;         // 记录各个元素的值

public:
    // 初始化 K 路败者树
    LoserTree(int k) : k(k), losers(k, -1), keys(k, INT_MAX) {}

    // 设置初始的各路元素值并构建败者树
    void initialize(const vector<int>& initialKeys) {
        // 设置初始键值
        keys = initialKeys;

        // 构建初始的败者树
        for (int i = 0; i < k; ++i) {
            adjust(i);
        }
    }

    // 获取当前的胜者索引
    int winner() const {
        return losers[0];
    }

    // 更新败者树中指定索引的值
    void update(int index, int newKey) {
        keys[index] = newKey;
        adjust(index);
    }

    // 获取指定索引的键值
    int getKey(int index) const {
        return keys[index];
    }

private:
    // 调整败者树，将指定索引 s 的元素插入到正确的位置
    void adjust(int s) {
        int parent = (s + k) / 2;
        int temp = s;

        while (parent > 0) {
            if (temp != -1 && (losers[parent] == -1 || keys[temp] > keys[losers[parent]])) {
                swap(temp, losers[parent]);
            }
            parent /= 2;
        }
        losers[0] = temp;
    }
};

// k路归并文件
void mergeFiles(const vector<string>& inputFiles, const string& outputFile) {
    int k = inputFiles.size();
    vector<ifstream> inputs(k);
    ofstream output(outputFile);

    // 打开所有输入文件
    for (int i = 0; i < k; ++i) {
        inputs[i].open(inputFiles[i]);
        if (!inputs[i].is_open()) {
            cerr << "无法打开输入文件进行归并！" << endl;
            return;
        }
    }

    vector<int> values(k, numeric_limits<int>::max());
    vector<bool> hasValue(k, false);

    // 初始化每个输入文件的首个值
    for (int i = 0; i < k; ++i) {
        if (inputs[i] >> values[i]) {
            hasValue[i] = true;
        }
    }

    // 初始化败者树
    LoserTree lt(k);
    lt.initialize(values);

    // 开始归并
    while (true) {
        int winnerIndex = lt.winner();
        int winnerValue = lt.getKey(winnerIndex);
        if (winnerValue == numeric_limits<int>::max()) {
            // 所有文件已经归并完成
            break;
        }

        output << winnerValue << endl;
        

        // 从获胜的文件读取下一个值
        if (inputs[winnerIndex] >> values[winnerIndex]) {
            lt.update(winnerIndex, values[winnerIndex]);
        } else {
            lt.update(winnerIndex, numeric_limits<int>::max());
            hasValue[winnerIndex] = false;
        }
    }

    // 关闭文件
    for (int i = 0; i < k; ++i) {
        inputs[i].close();
    }
    output.close();

    //合并完成后删除原始文件
    for (const auto& file : inputFiles) {
        remove(file.c_str());
    }
}


// 读取并排序一个数据块
void readAndSortBlock(ifstream& input, vector<int>& buffer, int bufferSize) {
    buffer.clear();
    buffer.reserve(bufferSize);

    int num;
    for (int i = 0; i < bufferSize; ++i) {
        if (input >> num) {
            buffer.push_back(num);
        } else {
            break;
        }
    }
    sort(buffer.begin(), buffer.end());
}

// 外部排序实现，使用k路归并败者树
void externalSort(const string& inputFile, const string& outputFile, int bufferSize, int k) {
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

    // Step 2: k路归并多个有序文件，使用败者树
    while (tempFiles.size() > 1) {
        vector<string> newTempFiles;

        // 每次合并k个文件
        for (size_t i = 0; i < tempFiles.size(); i += k) {
            vector<string> filesToMerge;
            for (size_t j = 0; j < k && i + j < tempFiles.size(); ++j) {
                filesToMerge.push_back(tempFiles[i + j]);
            }

            if (filesToMerge.size() > 1) {
                string outputTempFile = "temp_merge_" + to_string(chunkIndex++) + ".txt";
                mergeFiles(filesToMerge, outputTempFile);
                newTempFiles.push_back(outputTempFile);
            } else {
                // 剩余文件数少于k，直接加入
                newTempFiles.push_back(filesToMerge[0]);
            }
        }
        tempFiles = newTempFiles;
    }

    // 最后的文件即为排序结果，重命名为输出文件
    if (!tempFiles.empty()) {
        rename(tempFiles[0].c_str(), outputFile.c_str());
        tempFiles.clear();
    }
}

int main() {
    const string inputFile = "result/data.txt";
    const string outputFile = "result/out.txt";
    int bufferSize = 100;  // 缓冲区大小
    int k = 4;              // k 路归并

    // 记录开始时间
    auto start = chrono::steady_clock::now();
    externalSort(inputFile, outputFile, bufferSize, k);
    
    // 记录结束时间
    auto end = chrono::steady_clock::now();
    
    // 计算运行时间
    chrono::duration<double> duration = end - start;

    // 输出运行时间
    cout << duration.count() << " s" << endl;
    return 0;
}
