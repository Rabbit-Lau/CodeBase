#include <iostream>
#include <cassert>
#include <ctime>
#include <vector>
#include "StackAlloc.hpp"
#include "MemoryPool.hpp"

// 插入元素个数
#define ELEMS 2000000
// 重复次数
#define REPS 50

int main()
{
    clock_t start, finish;
    double duration;
    // 使用默认分配器
    StackAlloc<int, std::allocator<int>> stackDefault;
    start = clock();

    for(int i = 0; i < REPS; i++)
    {
        assert(stackDefault.empty());
        for(int j = 0; j < ELEMS; j++)
        {
            stackDefault.push(j);
        }

        for(int j = 0; j < ELEMS; j++)
        {
            stackDefault.pop();
        }
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::cout << "Default Allocator Time: ";
    std::cout << duration << " seconds." << std::endl;
    
    // 使用内存池
    StackAlloc<int, MemoryPool<int>> stackPool;
    start = clock();
    for(int i = 0; i < REPS; i++)
    {
        assert(stackPool.empty());
        for(int j = 0; j < ELEMS; j++)
        {
            stackPool.push(j);
        }
        for(int j = 0 ; j < ELEMS; j++)
        {
            stackPool.pop();
        }
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::cout << "MemoryPool Allocator Time: ";
    std::cout << duration << " seconds." << std::endl;
    
    return 0;
}

