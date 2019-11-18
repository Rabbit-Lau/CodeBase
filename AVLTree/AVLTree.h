#pragma once
#include <iostream>

using namespace std;

template <class T>
struct AVLTreeNode {
	AVLTreeNode(const T& data = T())
		: _pParent(nullptr)
		, _pLeft(nullptr)
		, _pRight(nullptr)
		, _data(data)
		,_bf(0)
		{}

	AVLTreeNode<T>* _pParent;
	AVLTreeNode<T>* _pLeft;
	AVLTreeNode<T>* _pRight;
	T _data;
	int _bf;
};

template <class T>
class AVLTree {
	typedef AVLTreeNode<T> Node;
	typedef Node* pNode;
public:
	bool Insert(const T& val) {
		if (_pRoot == nullptr) {
			_pRoot = new Node(val);
			return true;
		}
		pNode cur = _pRoot;
		pNode parent = nullptr;
		// 寻找插入位置
		while (cur) {
			parent = cur;
			if (cur->_data > val) {
				cur = cur->_pLeft;
			}
			else if (cur->_data < val) {
				cur = cur->_pRight;
			}
			else {
				return false;
			}
		}

		pNode newNode = new Node(val);
		if (parent->_data > val)
			parent->_pLeft = newNode;
		else
			parent->_pRight = newNode;
		newNode->_pParent = parent;
		cur = newNode;

		// 调整，保持平衡
		while (parent) {
			// 更新当前结点的平衡因子
			if (parent->_pLeft == cur) {
				--parent->_bf;
			}
			else {
				++parent->_bf;
			}
			// 检查平衡因子
			// _bf == 0 ? 高度没有变化停止更新
			if (parent->_bf == 0) {
				break;
			}
			// 高度为 +1 || -1，更新此路径上的祖先结点的平衡因子
			if (parent->_bf == 1 || parent->_bf == -1) {
				cur = parent;
				parent = parent->_pParent;
			}
			else if (parent->_bf == 2 || parent->_bf == -2) {
				// 不平衡，需要调整
				// 左旋
				if (parent->_bf == 2 && cur->_bf == 1) {
					RotateL(parent);
				}
				// 右旋
				else if (parent->_bf == -2 && cur->_bf == -1) {
					RotateR(parent);
				}
				else if (parent->_bf == -2 && cur->_bf == 1) {
					// 左右双旋
					RotateLR(parent);
				}
				else if (parent->_bf == 2 && cur->_bf == -1) {
					// 右左双旋
					RotateRL(parent);
				}
				break;
			}
		}
		return true;
	}
	// 左单旋
	void RotateL(pNode parent) {
		pNode subR = parent->_pRight;
		pNode subRL = subR->_pLeft;
		
		// 处理subR的左子树
		parent->_pRight = subRL;
		if (subRL) {
			subRL->_pParent = parent;
		}
		// 处理subR和parent的关系
		subR->_pLeft = parent;
		pNode gParent = parent->_pParent;
		parent->_pParent = subR;
		subR->_pParent = gParent;
		if (gParent == nullptr) {
			_pRoot = subR;
			subR->_pParent = nullptr;
		}
		else {
			if (gParent->_pLeft == parent) {
				gParent->_pLeft = subR;
			}
			else {
				gParent->_pRight = subR;
			}
		}
		subR->_bf = parent->_bf = 0;
	}

	// 右单旋
	void RotateR(pNode parent) {
		pNode subL = parent->_pLeft;
		pNode subLR = subL->_pRight;
		
		parent->_pLeft = subLR;
		if (subLR) {
			subLR->_pParent = parent;
		}
		subL->_pRight = parent;
		pNode gParent = parent->_pParent;
		parent->_pParent = subL;
		subL->_pParent = gParent;
		if (gParent == nullptr) {
			_pRoot = subL;
			subL->_pParent = nullptr;
		}
		else {
			if (gParent->_pLeft == parent) {
				gParent->_pLeft = subL;
			}
			else {
				gParent->_pRight = subL;
			}
		}
		subL->_bf = parent->_bf = 0;
	}
	// 左右双旋
	void RotateLR(pNode parent) {
		pNode subL = parent->_pLeft;
		pNode subLR = subL->_pRight;
		int bf = subLR->_bf;
		RotateL(subL);
		RotateR(parent);
		if (bf == 1) {
			parent->_bf = 0;
			subL->_bf = -1;
		}
		else if (bf == -1) {
			subL->_bf = 0;
			parent->_bf = 1;
		}
	}
	// 右左双旋
	void RotateRL(pNode parent) {
		pNode subR = parent->_pRight;
		pNode subRL = parent->_pLeft;
		int bf = subRL->_bf;
		RotateR(subR);
		RotateL(parent);
		if (bf == 1) {
			subR->_bf = 0;
			parent->_bf = -1;
		}
		else if (bf == -1) {
			parent->_bf = 0;
			subR->_bf = 1;
		}
	}

	// 获取树的深度（高度）
	int Height(pNode root) {
		if (root == nullptr) {
			return 0;
		}
		int leftHeight = Height(root->_pLeft);
		int rightHeight = Height(root->_pRight);
		return leftHeight > rightHeight ? (leftHeight + 1) : (rightHeight + 1);
	}
	
	//判断是否为平衡树

	bool IsBalance() {
		return _IsBalanceTree(_pRoot);
	}

	bool _IsBalanceTree(pNode root) {
		if (root == nullptr) {
			return true;
		}
		int leftHeight = Height(root->_pLeft);
		int rightHeight = Height(root->_pRight);
		int diff = leftHeight - rightHeight;
		if ((diff != root->_bf) || (diff > 1 || diff < -1)) {
			return false;
		}
		return _IsBalanceTree(root->_pLeft) && _IsBalanceTree(root->_pRight);
	}


	void InOrder() {
		_InOrder(_pRoot);
	}


private:
	void _InOrder(pNode root) {
		if (root) {
			_InOrder(root->_pLeft);
			cout << root->_data << " ";
			_InOrder(root->_pRight);
		}
	}
	
	void _Destroy(pNode root) {
		if (root) {
			_Destroy(root->_pLeft);
			_Destroy(root->_pRight);
			_pRoot = nullptr;
		}
	}
	pNode _pRoot = nullptr;
};

void testAVL()
{
	//int arr[] = { 16, 3, 7, 11, 9, 26, 18, 14, 15 };
	int arr[] = { 4, 2, 6, 1, 3, 5, 15, 7, 16, 14 };

	AVLTree<int> avl;
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		avl.Insert(arr[i]);
		cout << arr[i] << "--->" << avl.IsBalance() << endl;
	}


	avl.InOrder();
}
