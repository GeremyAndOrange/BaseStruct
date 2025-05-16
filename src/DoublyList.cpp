#include <Linear/DoublyList.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>

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

// 辅助函数：将链表转换为 vector 方便比较
template <typename T>
std::vector<T> listToVector(Linear::DoublyList<T>& list) {
    std::vector<T> result;
    for (auto val : list) {
        result.push_back(val);
    }
    return result;
}

// ------------------------- 测试用例 -------------------------

// 测试 1：基础插入和遍历
bool testPushAndIterate() {
    Linear::DoublyList<int> list;
    
    // 插入元素
    list.push_back(1);
    list.push_back(2);
    list.push_front(0);
    
    // 验证元素和顺序
    CHECK(list.size() == 3, "Size should be 3");
    CHECK((listToVector(list) == std::vector<int>{0, 1, 2}), "Elements should be 0,1,2");
    
    return true;
}

// 测试 2：删除操作
bool testPopAndErase() {
    Linear::DoublyList<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    // 删除头尾
    list.pop_front();
    list.pop_back();
    CHECK(listToVector(list) == std::vector<int>{2}, "After pop, elements should be 2");
    
    // 指定位置删除
    auto it = list.begin();
    list.erase(it);
    CHECK(list.empty(), "List should be empty after erase");
    
    return true;
}

// 测试 3：插入和删除边界条件
bool testEdgeCases() {
    Linear::DoublyList<int> list;
    
    // 空链表删除
    list.pop_front();  // 不应崩溃
    list.pop_back();   // 不应崩溃
    
    // 插入头尾
    list.insert(10, list.end());  // 插入到 end() 前
    list.insert(20, list.begin());
    CHECK((listToVector(list) == std::vector<int>{20, 10}), "Insert at edge positions");
    
    return true;
}

// 测试 4：合并链表
bool testSpliceAndMerge() {
    Linear::DoublyList<int> list1, list2;
    list1.push_back(1);
    list1.push_back(3);
    list2.push_back(2);
    list2.push_back(4);
    
    // 合并链表
    list1.splice(list2);
    CHECK((listToVector(list1) == std::vector<int>{1, 3, 2, 4}), "After splice");
    
    // 排序合并
    Linear::DoublyList<int> list3, list4;
    list3.push_back(1);
    list3.push_back(3);
    list4.push_back(2);
    list4.push_back(4);
    list3.merge(list4);
    CHECK((listToVector(list3) == std::vector<int>{1, 2, 3, 4}), "After merge");
    
    return true;
}

// 测试 5：去重和反转
bool testUniqueAndReverse() {
    Linear::DoublyList<int> list;
    list.push_back(1);
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(3);
    
    list.unique();
    CHECK((listToVector(list) == std::vector<int>{1, 2, 3}), "After unique");
    
    list.reverse();
    CHECK((listToVector(list) == std::vector<int>{3, 2, 1}), "After reverse");
    
    return true;
}

// 测试 6：排序功能
bool testSort() {
    // 测试默认升序排序
    Linear::DoublyList<int> list1;
    list1.push_back(3);
    list1.push_back(1);
    list1.push_back(4);
    list1.push_back(2);
    list1.sort();

    CHECK((listToVector(list1) == std::vector<int>{1, 2, 3, 4}), "Default sort (ascending) passed");

    // 测试自定义降序排序
    Linear::DoublyList<int> list2;
    list2.push_back(3);
    list2.push_back(1);
    list2.push_back(4);
    list2.push_back(2);
    list2.sort([](const int& a, const int& b) { return a > b; });

    CHECK((listToVector(list2) == std::vector<int>{4, 3, 2, 1}), "Custom sort (descending) passed");

    return true;
}

// 测试 7：异常处理
bool testExceptions() {
    Linear::DoublyList<int> list;
    bool caught = false;
    
    // 访问空链表的 front/back
    try {
        list.front();
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught, "Should throw out_of_range for front() on empty list");
    
    return true;
}

// 测试 8：性能对比
#include <Windows.h>
#include <psapi.h>
#include <chrono>
#include <list>
size_t getMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return pmc.WorkingSetSize / 1024;
}

template <typename ListType>
void testMassiveInsert(const std::string& containerName, int count = 100000) {
    ListType list;
    auto startTime = std::chrono::high_resolution_clock::now();
    size_t startMem = getMemoryUsage();

    for (int i = 0; i < count; ++i) {
        list.push_front(i);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    size_t endMem = getMemoryUsage();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    size_t memoryUsed = endMem - startMem;

    std::cout << "[" << containerName << "] Insert " << count << " elements: " << duration << " ms, Memory: " << memoryUsed << " KB\n";
}

// ------------------------- 主函数 -------------------------
int main() {
    bool allPassed = true;
    
    // 运行所有测试
    allPassed &= testPushAndIterate();
    allPassed &= testPopAndErase();
    allPassed &= testEdgeCases();
    allPassed &= testSpliceAndMerge();
    allPassed &= testUniqueAndReverse();
    allPassed &= testSort();
    allPassed &= testExceptions();
    
    // 输出最终结果
    if (allPassed) {
        std::cout << "\033[32mAll tests passed!\033[0m\n";
    } else {
        std::cout << "\033[31mSome tests failed!\033[0m\n";
    }
    
    std::cout << "=== Testing Linear::DoublyList ===\n";
    testMassiveInsert<Linear::DoublyList<int>>("DoublyList");
    
    std::cout << "\n=== Testing std::list ===\n";
    testMassiveInsert<std::list<int>>("std::list");

    return 0;
}