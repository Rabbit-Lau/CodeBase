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

	void init(); // 初始化
	void calculateMD5(size_t* chunk); // 计算512bit的chunk块
	void calculateMD5Final();
	std::string changeHex(size_t num);

	// 四大处理数据块的函数 
	// F函数：F(x,y,z) = (x & y) | ((~x) & z)
	size_t F(size_t x, size_t y, size_t z)
	{
		return (x & y) | ((~x) & z);
	}
	// G函数：G(x,y,z) = (x & z) | ( y & (~z))
	size_t G(size_t x, size_t y, size_t z)
	{
		return (x & z) | (y & (~z));
	}
	// H函数：H(x,y,z) = x ^ y ^ z
	size_t H(size_t x, size_t y, size_t z)
	{
		return x ^ y ^ z;
	}
	// I函数：I(x,y,z) = y ^ (x | (~z))
	size_t I(size_t x, size_t y, size_t z)
	{
		return y ^ (x | (~z));
	}

	// 左移操作:每次在左移的位数，和i是一一对应的。
	size_t shiftLeftRotate(size_t num, size_t n)
	{
		return (num << n) | (num >> (32 - n));
	}



	size_t _a;
	size_t _b;
	size_t _c;
	size_t _d;

	size_t _k[N]; // K[i]
	size_t _sft[N]; // 循环移位
	unsigned char _chunk[N]; // 每一个块的大小：64字节
	const size_t _chunkByte; // 每一次运算对应块的大小
	size_t _lastByte;
	unsigned long long _totalByte; // 总共处理了多少字节
};
