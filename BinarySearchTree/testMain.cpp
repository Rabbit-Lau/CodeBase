#include "SearchTree.h"


int main() {
	
	BSTree<int> bs;
	bs.Insert(5);
	bs.Insert(6);
	bs.Insert(7);
	bs.Insert(8);
	bs.Insert(1);
	bs.Insert(2);
	bs.Insert(3);
	cout << "�������: ";
	bs.InOrder();
	bs.Erase(1);
	cout << endl << "ɾ��1֮��";
	bs.InOrder();
	bs.Erase(5);
	cout << endl << "ɾ�����ڵ�֮��";
	bs.InOrder();
	cout << endl << "����(1Ϊ�ɹ�)��";
	bs.Destroy();
	cout <<  bs.IsEmpty() ;
	return 0;
}