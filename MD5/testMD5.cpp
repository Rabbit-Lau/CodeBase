#include <iostream>
#include <stdio.h>
#include "MD5.h"


void Menu()
{
	printf("\t **********************************\t\n");
	printf("\t *           MD5У�鹤��          *\t\n");
	printf("\t *                                *\t\n");
	printf("\t *              ѡ��              *\t\n");
	printf("\t *       1. �鿴�ļ���MD5         *\t\n");
	printf("\t *       2. �鿴�ַ�����MD5       *\t\n");
	printf("\t *       3. �˳�                  *\t\n");
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
		std::cout << "������ѡ�->";
		std::cin >> options;
		switch (options)
		{
		case 1: 
			std::cout << "1. �鿴�ļ�MD5" << std::endl;
			std::cout << "���ļ��� MD5 �ǣ�" << md5.getFileMD5(md5.openFile()) << std::endl;
			std::cout << std::endl << std::endl;
		
			break;
		case 2:
			std::cout << "2. �鿴�ַ���MD5" << std::endl;
			std::cout << "���������ݣ�";
			std::cin >> str;
			std::cout << "���ַ����� MD5 �ǣ�" << md5.getStringMD5(str) << std::endl;
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



