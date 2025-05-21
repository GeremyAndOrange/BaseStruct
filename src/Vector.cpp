#include <Linear/Vector.hpp>
#include <vector>
#include <stdexcept>
#include <Windows.h>
#include <psapi.h>
#include <chrono>

// 自定义测试宏
#define CHECK(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "\033[31m[FAIL]\033[0m Line " << __LINE__ << ": " << message << "\n"; \
            return false; \
        } else { \
            std::cout << "\033[32m[PASS]\033[0m " << message << "\n"; \
        } \
    } while(0)

// ------------------------- 测试用例 -------------------------

// 测试 1：基础插入和遍历
bool testPushAndIterate() {
    Linear::Vector<int> vec;
    CHECK(vec.empty(), "Vector should be empty initially");

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    CHECK(vec.size() == 3, "Size should be 3 after three pushes");
    CHECK(vec[0] == 1 && vec[1] == 2 && vec[2] == 3, "Elements should be 1,2,3");

    // 测试迭代器
    std::vector<int> collected;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        collected.push_back(*it);
    }
    CHECK((collected == std::vector<int>{1, 2, 3}), "Iterator traversal");

    return true;
}

// 测试 2：删除操作
bool testEraseAndClear() {
    Linear::Vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    vec.erase(1); // 删除索引1
    CHECK(vec.size() == 3, "Size after erase");
    CHECK(vec[0] == 1 && vec[1] == 3 && vec[2] == 4, "Elements after erase");

    vec.clear();
    CHECK(vec.empty(), "Vector should be empty after clear");

    return true;
}

// 测试 3：边界条件
bool testEdgeCases() {
    Linear::Vector<int> vec;
    // 测试初始插入扩容
    vec.push_back(1);
    CHECK(vec.capacity() == 1, "Initial capacity");
    vec.push_back(2);
    CHECK(vec.capacity() == 2, "Double capacity");
    vec.push_back(3);
    CHECK(vec.capacity() == 4, "Double capacity again");

    // 测试头部插入
    Linear::Vector<int> vec2;
    vec2.push_back(2);
    vec2.insert(0, 1); // 插入到头部
    CHECK(vec2.size() == 2, "Size after head insert");
    CHECK(vec2[0] == 1 && vec2[1] == 2, "Head insert elements");

    return true;
}

// 测试 4：拷贝构造和赋值
bool testCopyAndAssignment() {
    Linear::Vector<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);

    // 拷贝构造
    Linear::Vector<int> vec2(vec1);
    CHECK(vec2.size() == 2, "Copy constructor size");
    CHECK(vec2[0] == 1 && vec2[1] == 2, "Copy constructor elements");

    // 修改原对象不影响拷贝
    vec1[0] = 99;
    CHECK(vec2[0] == 1, "Deep copy verification");

    // 赋值运算符
    Linear::Vector<int> vec3;
    vec3 = vec2;
    CHECK(vec3.size() == 2, "Assignment size");
    CHECK(vec3[0] == 1 && vec3[1] == 2, "Assignment elements");

    // 自赋值测试
    vec3 = vec3;
    CHECK(vec3.size() == 2, "Self-assignment");

    return true;
}

// 测试 5：异常处理
bool testExceptions() {
    Linear::Vector<int> vec;
    bool caught = false;

    // 测试空 Vector 访问
    try {
        vec.at(0);
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught, "at() on empty vector");

    // 测试越界访问
    vec.push_back(10);
    caught = false;
    try {
        vec.at(1);
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught, "at() out of range");

    return true;
}

// 测试 6：迭代器功能
bool testIterators() {
    Linear::Vector<std::string> vec;
    vec.push_back("first");
    vec.push_back("second");
    vec.push_back("third");

    // 前后缀++
    auto it = vec.begin();
    CHECK(*it == "first", "Begin iterator");
    ++it;
    CHECK(*it == "second", "Prefix ++");
    ++it;
    CHECK(*it == "third", "Prefix ++");
    ++it;
    CHECK(it == vec.end(), "End iterator");

    // 反向遍历（Vector 本身不支持反向迭代器，但可以手动 --）
    Linear::Vector<int> vec2;
    vec2.push_back(1);
    vec2.push_back(2);
    auto it2 = vec2.end();
    --it2;
    CHECK(*it2 == 2, "Reverse traversal");

    return true;
}

// ------------------------- 性能测试工具函数 -------------------------
size_t getMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return pmc.WorkingSetSize / 1024; // 返回 KB
}

template <typename VecType>
void testPerformance(const std::string& name, int count = 1000000) {
    VecType vec;
    auto start = std::chrono::high_resolution_clock::now();
    size_t startMem = getMemoryUsage();

    for (int i = 0; i < count; ++i) {
        vec.push_back(i);
    }

    auto end = std::chrono::high_resolution_clock::now();
    size_t endMem = getMemoryUsage();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[" << name << "]\n"
              << "Time: " << duration << " ms\n"
              << "Memory: " << (endMem - startMem) << " KB\n";
}

// ------------------------- 主函数 -------------------------
int main() {
    bool allPassed = true;

    std::cout << "=== Running Vector tests ===\n";
    allPassed &= testPushAndIterate();
    allPassed &= testEraseAndClear();
    allPassed &= testEdgeCases();
    allPassed &= testCopyAndAssignment();
    allPassed &= testExceptions();
    allPassed &= testIterators();

    if (allPassed) {
        std::cout << "\n\033[32mAll tests passed!\033[0m\n\n";
    } else {
        std::cout << "\n\033[31mSome tests failed!\033[0m\n\n";
    }

    std::cout << "=== Performance Comparison ===\n";
    std::cout << "-- Custom Vector --\n";
    testPerformance<Linear::Vector<int>>("Linear::Vector");

    std::cout << "\n-- std::vector --\n";
    testPerformance<std::vector<int>>("std::vector");

    return 0;
}