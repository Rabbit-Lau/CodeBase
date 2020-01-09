#pragma once
#include <climits>
#include <cstddef>
#include <iostream>
template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
public:
    typedef T* pointer;

    // 定义rebind<U>::other 接口
    template <typename U> struct rebind
    {
        typedef MemoryPool<U> other;
    };
    // 构造函数，不会抛出异常
    MemoryPool() noexcept
    {
        _currentBlock = nullptr;
        _currentSlot = nullptr;
        _lastSlot = nullptr;
        _freeSlots = nullptr;
    }

    // 析构函数
    ~MemoryPool () noexcept 
    {
        // 循环销毁内存池中的内存区块
        _slot_pointer cur = _currentBlock;
        while(cur != nullptr)
        {
            _slot_pointer prev = cur->next;
            operator delete(reinterpret_cast<void*>(cur));
            cur = prev;
        }
    }
    
    // 调用构造函数
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args)
    {
        new (p) U (std::forward<Args>(args)...);
    }

    // 同一时间只能分配一个对象
    pointer allocate(size_t n = 1, const T* hint = 0)
    {
        // 如果有空闲的对象槽，那么直接将空闲的区域交出去
        if(_freeSlots != nullptr)
        {
            pointer result = reinterpret_cast<pointer>(_freeSlots);
            _freeSlots = _freeSlots->next;
            return result;
        }
        else
        {
            // 如果对象槽不够用了，则分配一个新的内存区域
            if(_currentSlot >= _lastSlot)
            {
                // 分配新的内存区块，并指向前一个内存区域
                _data_pointer newBlock = reinterpret_cast<_data_pointer>(operator new(BlockSize));
                reinterpret_cast<_slot_pointer>(newBlock)->next = _currentBlock;
                _currentBlock = reinterpret_cast<_slot_pointer>(newBlock);
                // 填补整个区块来满足元素内存区域的对齐要求
                _data_pointer body = newBlock + sizeof(_slot_pointer);
                uintptr_t result = reinterpret_cast<uintptr_t>(body);
                size_t bodyPadding = (alignof(_slot_type) - result) % alignof(_slot_type);
                _currentSlot = reinterpret_cast<_slot_pointer>(body + bodyPadding);
                _lastSlot = reinterpret_cast<_slot_pointer>(newBlock + BlockSize - sizeof(_slot_type) + 1);
            }
            return reinterpret_cast<pointer>(_currentSlot++);
        }
    }
    
    // 销毁指针指向的内存区块
    void deallocate(pointer p, size_t n = 1)
    {
        if(p != nullptr)
        {
            // 要访问next必须强制将p转成_slot_pointer
            reinterpret_cast<_slot_pointer>(p)->next = _freeSlots;
            _freeSlots = reinterpret_cast<_slot_pointer>(p);
        }
    }

    // 销毁内存池中的对象，即调用对象的析构函数
    template <typename U>
    void destroy(U* p)
    {
        p->~U();
    }

private:
    // 用于存储内存池中的对象槽
    // 要么被实例化为一个存放对象的槽
    // 要么被实例化为一个存放对象槽的槽指针
    union _Slot
    {
        T element;
        _Slot* next;
    };

    // 数据指针
    typedef char* _data_pointer;
    // 对象槽
    typedef _Slot _slot_type;
    // 对象槽指针
    typedef _Slot* _slot_pointer;

    // 指向当前内存区块
    _slot_pointer _currentBlock;
    // 指向当前内存区块的一个对象槽
    _slot_pointer _currentSlot;
    // 指向当前内存区块的最后一个对象槽
    _slot_pointer _lastSlot;
    // 指向当前内存区块中的空闲对象槽
    _slot_pointer _freeSlots;

    // 检查定义的内存池大小是否过小
    static_assert(BlockSize >= 2 * sizeof(_slot_type), "BlockSize too small.");
};

