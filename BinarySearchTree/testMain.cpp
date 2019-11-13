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
	cout << "中序遍历: ";
	bs.InOrder();
	bs.Erase(1);
	cout << endl << "删除1之后：";
	bs.InOrder();
	bs.Erase(5);
	cout << endl << "删除根节点之后：";
	bs.InOrder();
	cout << endl << "销毁(1为成功)：";
	bs.Destroy();
	cout <<  bs.IsEmpty() ;
	return 0;
}