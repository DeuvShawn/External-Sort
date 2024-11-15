# **外部排序实现及其优化**

本文是大三的数据结构与算法实践作业，简单实现了三种不同阶段的外部排序，每个阶段针对不同的归并方式进行了优化：简单的二路归并、使用败者树优化的二路归并，以及基于败者树的 K 路归并。外部排序适合处理大规模数据集，在处理大型数据集时，单机的内存容量可能无法支持一次性将所有数据加载到内存中进行排序，这时就需要用到外部排序，通过将数据分成小块逐步加载到内存，进行排序并合并成最终的有序文件。本实验模拟在内存有限的情况下对大量数据的排序。

---

## **项目结构说明**

`test_data.cpp`用来生成测试数据

`BasicExternalSort`文件夹内是简单二路排序的实现代码，包含CMakeList和程序代码，下同

`TournamentTreeOptimization`文件夹内是二叉败者树优化的实现代码

`K-wayTreeOptimization`文件夹内是K叉败者树优化的实现代码

---

## **代码 1：二路外部排序**

### **功能概述**

第一份代码实现了经典的二路外部排序，使用固定大小的缓冲区读取数据块，先排序再写入多个临时文件，最后通过二路归并合并这些有序文件。

### **主要模块**

1. **分块读取与排序**

- 读取一定量数据（由缓冲区大小决定），对其排序后存储到临时文件。
- 函数：`readAndSortBlock(ifstream& input, vector<int>& buffer, int bufferSize)`

2. **二路归并**

- 从两个文件中读取数据进行归并，输出到新的文件。
- 函数：`mergeTwoFiles(const string& inputFile1, const string& inputFile2, const string& outputFile)`

3. **主逻辑**

- 执行分块读取排序生成临时文件。
- 使用二路归并逐步合并，直到只剩下一个有序文件。
- 函数：`externalSort(const string& inputFile, const string& outputFile, int bufferSize)`

---

## **代码 2：二路归并（败者树优化）**

### **功能概述**

第二份代码在二路外部排序的基础上进行了优化，使用败者树加速归并过程。败者树可以快速确定当前胜者，并调整归并顺序。

### **主要模块**

1. **败者树类**

- 实现了一个简单的败者树，用于加速二路归并。
- 主要方法：

  - `initialize(int key0, int key1)`: 初始化败者树。

  - `update(int index, int newKey)`: 更新指定索引的值并调整树。

  - `winner()`: 返回当前胜者索引。

2. **二路归并**

- 使用败者树代替直接比较来选择当前最小元素。
- 函数：`mergeFiles(const string& file1, const string& file2, const string& outputFile)`

3. **外部排序主逻辑**

- 分块排序生成临时文件。
- 二路归并的实现使用了败者树进行优化。
- 函数：`externalSort(const string& inputFile, const string& outputFile, int bufferSize)`

---

## **代码 3：K 路归并（基于败者树）**

### **功能概述**

第三份代码实现了 K 路归并排序，通过一个 K 路败者树高效合并多个有序文件。适合处理更大规模的数据。

### **主要模块**

1. **K 路败者树类**

- 支持动态调整，用于同时处理多个输入流。
- 主要方法：

  - `initialize(const vector<int>& initialKeys)`: 初始化败者树。

  - `update(int index, int newKey)`: 更新指定索引的值并调整树。

  - `winner()`: 返回当前胜者索引。

2. **K 路归并**

- 使用 K 路败者树归并多个文件，确保每次从多个有序文件中选出最小值。
- 函数：`mergeFiles(const vector<string>& inputFiles, const string& outputFile)`

3. **外部排序主逻辑**

- 分块读取与排序，生成多个临时文件。
- 每轮合并 K 个文件，直到最终得到一个有序文件。
- 函数：`externalSort(const string& inputFile, const string& outputFile, int bufferSize, int k)`

---

## **性能分析**

| 功能 | 二路排序 | 二路排序（败者树） | K 路排序（败者树） |
| ----------------------- | ------------------- | ----------------- | ------------------ |
| 时间复杂度           | O(n log n)         | O(n log n)        | O(n log k)         |

---

## **使用方法**

1. **运行环境**

- C++ 17 或更新版本。
- 使用CMake进行编译。

2. **输入**

- 数据文件路径 (默认`result/data.txt`)。
- 缓冲区大小（默认 100）。
- 对于 K 路归并，需指定归并路数（默认 4）。

3. **输出**

- 排序结果输出到指定文件路径 (默认`result/out.txt`)。

4. **运行示例**

```bash
# 在放置代码的目录下进行操作
mkdir result  # 运行前需要在result目录下放置data.txt
mkdir build
cd build
cmake ..
make
./external_sort  # 根据CMakeList.txt进行修改
```

# 部分性能测试表格

 | 测试功能 | 输入文件大小 | 缓冲区大小 (行) | K值 (路数) | 运行时间 (秒) |
 | -------- | ------------ | --------------- | ---------- | ------------- |
 | 二路排序 | 5 KB         | 100             | 2          | 0.01517 s     |
 | 二路排序（败者树） | 5 KB         | 100             | 2          | 0.01348 s     |
 | K 路排序（败者树） | 5 KB         | 100             | 4          | 0.00924 s     |

### 测试环境：

- 操作系统：MacOS
- 内存：16 GB
- 硬盘：SSD

### 性能趋势分析：

- 随着输入文件大小的增加，**分块读取和排序**时间和**K路归并**时间都会呈现上升趋势，尤其在大文件和较高K值下（更多的并行文件归并）。

- **缓冲区大小**对性能有较大影响，较大的缓冲区可以减少磁盘I/O操作，但会占用更多的内存资源。

- **K值**增加时，**K路归并时间**增长较为明显，但这也能加速归并过程，因为可以并行处理更多文件。
