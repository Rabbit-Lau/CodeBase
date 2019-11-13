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

	// �п�
	bool IsEmpty() {
		if (_pRoot == nullptr) {
			return true;
		}
		return false;
	}

	// ����
	PNode Find(const T& data) {
		PNode pCur = _pRoot;
		while (pCur) {
			// �����ֵ�Ƿ�����ҵ�ֵ��ƥ��
			if (data == pCur->_data)
				return pCur;
			// С�ˣ�������
			else if (data < pCur->_data)
				pCur = pCur->_pLeft;
			// ���ˣ�������
			else
				pCur = pCur->_pRight;
		}
		// δ�ҵ������ؿ�
		return nullptr;
	}

	// ����
	bool Insert(const T& data) {
		if (nullptr == _pRoot) {
			_pRoot = new Node(data);
			return true;
		}
		// �������������������ʲ��Ҳ���λ��
		PNode pCur = _pRoot;
		// ��¼����λ�õĸ����
		PNode pParent = nullptr;
		while (pCur) {
			pParent = pCur;
			if (data < pCur->_data) {
				pCur = pCur->_pLeft;
			}
			else if (data > pCur->_data) {
				pCur = pCur->_pRight;
			}
			// ���ڣ�����ʧ��
			else {
				return false;
			}
		}
		// ����Ԫ��
		pCur = new Node(data);
		if (data < pParent->_data) {
			pParent->_pLeft = pCur;
		}
		else {
			pParent->_pRight = pCur;
		}
		return true;
	}

	// ɾ��
	bool Erase(const T& data) {
		if (nullptr == _pRoot)
			return false;
		if (_pRoot->_data == data) {

		}
		// ����data��λ��
		PNode pCur = _pRoot; // PCur��ɾ�����λ��
		PNode pParent = nullptr; // pParent����ɾ�����ĸ��׽��
		// ����ɾ��λ��
		while (pCur) {
			if (data == pCur->_data) {
				// �ҵ��˳�ѭ��
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
		// pCurΪ������ɾ����ֵ��ɾ��ʧ��
		if (pCur == nullptr) {
			return false;
		}
		// ��ɾ���������������
		if (pCur->_pLeft == nullptr && pCur->_pRight == nullptr) {
			if (pParent->_pLeft == pCur) {
				pParent->_pLeft = nullptr;
			}
			else {
				pParent->_pRight = nullptr;
			}
			delete pCur;
		}
		// ��ɾ�������������
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
		// ����ɾ�������������
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
		// ��ɾ���������������
		else {
			// pR��pRF������ɾ����������������Сֵ������λ��
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

	// �������
	void InOrder() {
		_InOrder(_pRoot);
	}

	// ����
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