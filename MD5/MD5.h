#pragma once
#include <string>
#include <Windows.h>
#define N 64

class MD5
{
public:
	MD5();
	std::string getFileMD5(const char* filename);
	std::string getStringMD5(const std::string& str);
	char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn);
	char* openFile();
private:

	void init(); // ��ʼ��
	void calculateMD5(size_t* chunk); // ����512bit��chunk��
	void calculateMD5Final();
	std::string changeHex(size_t num);

	// �Ĵ������ݿ�ĺ��� 
	// F������F(x,y,z) = (x & y) | ((~x) & z)
	size_t F(size_t x, size_t y, size_t z)
	{
		return (x & y) | ((~x) & z);
	}
	// G������G(x,y,z) = (x & z) | ( y & (~z))
	size_t G(size_t x, size_t y, size_t z)
	{
		return (x & z) | (y & (~z));
	}
	// H������H(x,y,z) = x ^ y ^ z
	size_t H(size_t x, size_t y, size_t z)
	{
		return x ^ y ^ z;
	}
	// I������I(x,y,z) = y ^ (x | (~z))
	size_t I(size_t x, size_t y, size_t z)
	{
		return y ^ (x | (~z));
	}

	// ���Ʋ���:ÿ�������Ƶ�λ������i��һһ��Ӧ�ġ�
	size_t shiftLeftRotate(size_t num, size_t n)
	{
		return (num << n) | (num >> (32 - n));
	}



	size_t _a;
	size_t _b;
	size_t _c;
	size_t _d;

	size_t _k[N]; // K[i]
	size_t _sft[N]; // ѭ����λ
	unsigned char _chunk[N]; // ÿһ����Ĵ�С��64�ֽ�
	const size_t _chunkByte; // ÿһ�������Ӧ��Ĵ�С
	size_t _lastByte;
	unsigned long long _totalByte; // �ܹ������˶����ֽ�
};
