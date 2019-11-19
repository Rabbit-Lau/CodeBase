#pragma once
#include <iostream>
#include <time.h>
using namespace std;

enum Color {Red, Black};

template <class K, class V>
struct RBTreeNode {
	pair<K, V> _kv;
	Color _color = Red;

	RBTreeNode<K, V>* _pLeft = nullptr;
	RBTreeNode<K, V>* _pRight = nullptr;
	RBTreeNode<K, V>* _pParent = nullptr;
};

template <class K, class V>
class RBTree {
	typedef RBTreeNode<K, V> Node;
	typedef Node* pNode;
public:
	RBTree()
		:_header(new Node)
	{
		_header->_pLeft = _header;
		_header->_pRight = _header;
	}

	bool Insert(const pair<K, V>& kv) {
		if (_header->_pParent == nullptr) {
			pNode root = new Node;
			root->_kv = kv;
			root->_color = Black;
			root->_pParent = _header;
			_header->_pLeft = root;
			_header->_pRight = root;
			_header->_pParent = root;
			return true;
		}

		pNode cur = _header->_pParent; // _header->_pParent:�����
		pNode parent = nullptr;
		// 1. Ѱ�Ҳ���λ��
		while (cur) {
			if (cur->_kv.first > kv.first) {
				parent = cur;
				cur = cur->_pLeft;
			}
			else if (cur->_kv.first < kv.first) {
				parent = cur;
				cur = cur->_pRight;
			}
			else {
				return false;
			}
		}

		cur = new Node;
		cur->_kv = kv;
		if (parent->_kv.first > cur->_kv.first) {
			parent->_pLeft = cur;
		}
		else {
			parent->_pRight = cur;
		}
		cur->_pParent = parent;

		// 2. ����,��ɫ
		while (cur != _header->_pParent && 
			   cur->_pParent->_color == Red) {
			pNode parent = cur->_pParent;
			pNode gParent = parent->_pParent;
			if (gParent->_pLeft == parent) {
				pNode uncle = gParent->_pRight;
				// u������Ϊ��ɫ
				if (uncle && uncle->_color == Red) {
					parent->_color = uncle->_color = Black;
					gParent->_color = Red;
					cur = gParent;
				}
				// u�����ڻ��ߴ�����Ϊ��ɫ
				else {
					if (cur == parent->_pRight) {
						RotateL(parent);
						swap(parent, cur);
					}
					RotateR(gParent);
					parent->_color = Black;
					gParent->_color = Red;
					break;
				}
			}
			else
			{
				pNode uncle = gParent->_pLeft;
				if (uncle && uncle->_color == Red) {
					parent->_color = uncle->_color = Black;
					gParent->_color = Red;
					cur = gParent;
				}
				else {
					if (parent->_pLeft == cur) {
						RotateR(parent);
						swap(cur, parent);
					}
					RotateL(gParent);
					gParent->_color = Red;
					parent->_color = Black;
					break;
				}
			}
		}
		_header->_pParent->_color = Black; // ʹ��������ɫΪ��ɫ
		_header->_pLeft = leftMost(); 
		_header->_pRight = rightMost();
		return true;
	}

	pNode leftMost() {
		pNode cur = _header->_pParent;
		while (cur && cur->_pLeft) {
			cur = cur->_pLeft;
		}
		return cur;
	}

	pNode rightMost() {
		pNode cur = _header->_pParent;
		while (cur && cur->_pRight) {
			cur = cur->_pRight;
		}
		return cur;
	}

	void RotateL(pNode parent) {
		pNode subR = parent->_pRight;
		pNode subRL = subR->_pLeft;
		subR->_pLeft = parent;
		parent->_pRight = subRL;
		if (subRL) {
			subRL->_pParent = parent;
		}
		if (parent != _header->_pParent) {
			pNode gParent = parent->_pParent;
			if (gParent->_pLeft == parent) {
				gParent->_pLeft = subR;
			}
			else {
				gParent->_pRight = subR;
			}
			subR->_pParent = gParent;
		}
		else {
			subR->_pParent = nullptr;
			_header->_pParent = subR;
		}
	}

	void RotateR(pNode parent) {
		pNode subL = parent->_pLeft;
		pNode subLR = parent->_pRight;
		subL->_pRight = parent;
		parent->_pLeft = subLR;
		if (subLR) {
			subLR->_pParent = parent;
		}
		if (parent != _header->_pParent) {
			pNode gParent = parent->_pParent;
			if (gParent->_pLeft == parent) {
				gParent->_pLeft = subL;
			}
			else {
				gParent->_pRight = subL;
			}
			subL->_pParent = gParent;
		}
		else {
			subL->_pParent = nullptr;
			_header->_pParent = subL;
		}
		parent->_pParent = subL;
	}

	bool IsValidRBTree(){
		pNode root = _header->_pParent;
		// �����Ϊ������ �����
		if (root == nullptr) {
			return true;
		}
		if (root->_color != Black) {
			cout << "Υ�����ʶ�����������Ϊ��ɫ" << endl;
			return false;
		}

		size_t blackCount = 0;
		pNode pCur = root;
		// ��ȡ����һ��·���к�ɫ��������
		while (pCur) {
			if (pCur->_color == Black) {
				blackCount++;
			}
			pCur = pCur->_pLeft;
		}
		// ����Ƿ���������������
		size_t k = 0;
		return _IsValidRBTree(root, k, blackCount);
	}

	bool _IsValidRBTree(pNode root, size_t k, const size_t blackCount) {
		if (nullptr == root) {
			if (k != blackCount) {
				cout << "Υ�������ģ�ÿ��·���к�ɫ�ĸ���������ͬ" << endl;
			}
			return true;
		}

		if (Black == root->_color) {
			k++;
		}
		pNode pParent = root->_pParent;
		if (pParent && Red == pParent->_color && Red == root->_color) {
			cout << "Υ��������������������һ��ĺ���" << endl;
			return false;
		}
		return _IsValidRBTree(root->_pLeft,  k,  blackCount) &&
			_IsValidRBTree(root->_pRight, k, blackCount);
	}
	void Inorder()
	{
		_Inorder(_header->_pParent);
		cout << endl;
	}

	void _Inorder(pNode root)
	{
		if (root)
		{
			_Inorder(root->_pLeft);
			cout << root->_kv.first << " ";
			_Inorder(root->_pRight);
		}
	}

private:
	pNode _header;
};