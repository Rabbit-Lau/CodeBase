#pragma once
#include <memory>

template<typename T>
struct _StackNode
{
    T data;
    _StackNode* prev;
};

// T为存储的对象类型，Alloc为使用的分配器，
// 并默认使用std::allocator作为对象的分配器
template <typename T, typename Alloc = std::allocator<T>>
class StackAlloc
{
public:
    typedef _StackNode<T> Node;
    typedef typename Alloc::template rebind<Node>::other allocator;
    // 构造
    StackAlloc() { _head = nullptr;}
    // 析构
    ~StackAlloc() { clear(); }
    
    // 判空，栈元素为空返回true
    bool empty() { return (_head == nullptr); }
    
    // 释放栈元素
    void clear()
    {
        Node* cur = _head;
        while(cur != nullptr)
        {
            // 记录下一个将要释放的元素
            Node* tmp = cur->prev;
            // 先析构，在回收内存
            _allocator.destroy(cur);
            _allocator.deallocate(cur, 1);
            cur = tmp;
        }
        _head = nullptr;
    }

    // 入栈
    void push(T element)
    {
        // 为新节点分配内存
        Node* newNode = _allocator.allocate(1);
        // 调用结点的构造函数
        _allocator.construct(newNode, Node());
        // 入栈操作
        newNode->data = element;
        newNode->prev = _head;
        _head = newNode;
    }

    // 出栈
    T pop()
    {
        // 出栈操作，返回当前出栈的元素值
        T result = _head->data; // 保存当前栈顶元素值
        Node* tmp = _head->prev; //保存下一个元素
        // 删除元素，并释放内存
        _allocator.destroy(_head); 
        _allocator.deallocate(_head, 1);
        _head = tmp; // 更新栈顶元素
        return result;
    }

    // 返回栈顶元素
    T top(){ return (_head->data); }

private:
    allocator _allocator;
    Node* _head; // 栈顶
};

