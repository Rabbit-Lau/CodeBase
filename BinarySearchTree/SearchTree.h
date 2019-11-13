#pragma once
#include <iostream>
using namespace std;

template<class T>
struct BSTNode {
	BSTNode(const T& data = T())
		:_pLeft(nullptr)
		,_pRight(nullptr)
		,_data(data)
	{}
	BSTNode<T>* _pLeft;
	BSTNode<T>* _pRight;
	T _data;
};

template<class T>
class BSTree {
	typedef BSTNode<T> Node;
	typedef Node* PNode;

public:
	BSTree()
		:_pRoot(nullptr)
	{}

	~BSTree() {
		_Destroy(_pRoot);
	}

	// 判空
	bool IsEmpty() {
		if (_pRoot == nullptr) {
			return true;
		}
		return false;
	}

	// 查找
	PNode Find(const T& data) {
		PNode pCur = _pRoot;
		while (pCur) {
			// 看结点值是否与查找的值相匹配
			if (data == pCur->_data)
				return pCur;
			// 小了，往左找
			else if (data < pCur->_data)
				pCur = pCur->_pLeft;
			// 大了，往右找
			else
				pCur = pCur->_pRight;
		}
		// 未找到，返回空
		return nullptr;
	}

	// 插入
	bool Insert(const T& data) {
		if (nullptr == _pRoot) {
			_pRoot = new Node(data);
			return true;
		}
		// 按照搜索二叉树的性质查找插入位置
		PNode pCur = _pRoot;
		// 记录插入位置的父结点
		PNode pParent = nullptr;
		while (pCur) {
			pParent = pCur;
			if (data < pCur->_data) {
				pCur = pCur->_pLeft;
			}
			else if (data > pCur->_data) {
				pCur = pCur->_pRight;
			}
			// 存在，插入失败
			else {
				return false;
			}
		}
		// 插入元素
		pCur = new Node(data);
		if (data < pParent->_data) {
			pParent->_pLeft = pCur;
		}
		else {
			pParent->_pRight = pCur;
		}
		return true;
	}

	// 删除
	bool Erase(const T& data) {
		if (nullptr == _pRoot)
			return false;
		if (_pRoot->_data == data) {

		}
		// 查找data的位置
		PNode pCur = _pRoot; // PCur：删除结点位置
		PNode pParent = nullptr; // pParent：被删除结点的父亲结点
		// 查找删除位置
		while (pCur) {
			if (data == pCur->_data) {
				// 找到退出循环
				break;
			}
			else if (data < pCur->_data) {
				pParent = pCur;
				pCur = pCur->_pLeft;
			}
			else {
				pParent = pCur;
				pCur = pCur->_pRight;
			}
		}
		// pCur为空则无删除的值，删除失败
		if (pCur == nullptr) {
			return false;
		}
		// 被删除结点无左右子树
		if (pCur->_pLeft == nullptr && pCur->_pRight == nullptr) {
			if (pParent->_pLeft == pCur) {
				pParent->_pLeft = nullptr;
			}
			else {
				pParent->_pRight = nullptr;
			}
			delete pCur;
		}
		// 被删除结点无右子树
		else if (pCur->_pLeft && pCur->_pRight == nullptr) {
			if (pParent->_pLeft == pCur) {
				pParent->_pLeft = pCur->_pLeft;
				delete pCur;
			}
			else {
				pParent->_pRight = pCur->_pLeft;
				delete pCur;
			}
		}
		// 当被删除结点无左子树
		else if (pCur->_pRight && pCur->_pLeft == nullptr) {
			if (pParent->_pLeft == pCur) {
				pParent->_pLeft = pCur->_pRight;
				delete pCur;
			}
			else {
				pParent->_pRight = pCur->_pRight;
				delete pCur;
			}
		}
		// 被删除结点有左右子树
		else {
			// pR和pRF用来找删除结点的右子树中最小值所处的位置
			PNode pR = pCur->_pRight; 
			PNode pRF = pCur;  
			
			while (pR->_pLeft) {
				pRF = pR;
				pR = pR->_pLeft;
			}
			if (pRF == pCur) {
				pCur->_data = pR->_data;
				pRF->_pRight = pR->_pRight;

			}
			else {
				pCur->_data = pR->_data;
				pRF->_pLeft = pR->_pLeft;
			}
			delete pR;
		}
		return true;
	}

	// 中序遍历
	void InOrder() {
		_InOrder(_pRoot);
	}

	// 销毁
	void Destroy() {
		_Destroy(_pRoot);
	}
private:
	void _InOrder(PNode pRoot) {
		if (pRoot) {
			_InOrder(pRoot->_pLeft);
			cout << pRoot->_data << " ";
			_InOrder(pRoot->_pRight);
		}
	}
	void _Destroy(PNode& pRoot) {
		if (pRoot) {
			_Destroy(pRoot->_pLeft);
			_Destroy(pRoot->_pRight);
			pRoot = nullptr;
		}
	}
	
	PNode _pRoot;
};