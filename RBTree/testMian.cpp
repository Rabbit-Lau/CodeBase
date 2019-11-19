#pragma once
#include "RBTree.h"

int main()

{
	RBTree<int, int> rbt;
	srand(time(nullptr));
	/*int arr[] = { 5, 7, 9, 1, 3, 4, 10, 6, 8 };
	for (const auto& e : arr)
	{
	rbt.Insert(make_pair(e, e));
	}*/

	int n;
	cin >> n;
	while (n--)
	{
		int num = rand();
		cout << num << " ";
		rbt.Insert(make_pair(num, n));
		/*if (!rbt.IsValidRBTree())
		{
			cout << "插入节点异常：" << num << endl;
		}*/
	}
	cout << endl;
	/*rbt.Inorder();*/
	cout <<"IsValidRBTree: "<< rbt.IsValidRBTree() << endl;
	return 0;
}