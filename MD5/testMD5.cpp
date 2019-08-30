#include <iostream>
#include <stdio.h>
#include "MD5.h"


void Menu()
{
	printf("\t **********************************\t\n");
	printf("\t *           MD5校验工具          *\t\n");
	printf("\t *                                *\t\n");
	printf("\t *              选项              *\t\n");
	printf("\t *       1. 查看文件的MD5         *\t\n");
	printf("\t *       2. 查看字符串的MD5       *\t\n");
	printf("\t *       3. 退出                  *\t\n");
	printf("\t *********************************\t\n");



}

int main()
{
	int options = 0;
	std::string str;
	MD5 md5;
	while (true)
	{
		Menu();
		std::cout << "请输入选项：->";
		std::cin >> options;
		switch (options)
		{
		case 1: 
			std::cout << "1. 查看文件MD5" << std::endl;
			std::cout << "该文件的 MD5 是：" << md5.getFileMD5(md5.openFile()) << std::endl;
			std::cout << std::endl << std::endl;
		
			break;
		case 2:
			std::cout << "2. 查看字符串MD5" << std::endl;
			std::cout << "请输入内容：";
			std::cin >> str;
			std::cout << "该字符串的 MD5 是：" << md5.getStringMD5(str) << std::endl;
			std::cout << std::endl << std::endl;
			break;
		case 3: return 0;
			break;
		default:
			break;
		}

	}
	return 0;
}



