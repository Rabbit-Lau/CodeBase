#pragma once
#include "MD5.h"
#include <string.h>
#include <math.h>
#include <fstream>
#include <Windows.h>
#include <Commdlg.h>
#include <stdio.h>

OPENFILENAME ofn;
char szFile[300];


MD5::MD5()
	:_chunkByte(N)
{
	init();
	memset(_chunk, 0, _chunkByte);
	_totalByte = _lastByte = 0;
}


// 初始化输入的A、B、C、D、_sft、_k[i]
void MD5::init()
{
	// 初始化a、b、c、d
	_a = 0x67452301;
	_b = 0xefcdab89;
	_c = 0x98badcfe;
	_d = 0x10325476;
	// 初始化 _sft
	size_t s[] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
				   12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 
		           20, 5, 9, 14, 20,4, 11, 16, 23, 4, 11, 16, 23, 4, 
		           11, 16, 23, 4, 11, 16, 23, 6, 10,15, 21, 6, 10, 
		           15,21, 6, 10, 15, 21, 6, 10, 15, 21 };
	memcpy(_sft, s, sizeof(s));

	// 初始化_k[i]
	for (int i = 0; i < 64; ++i)
	{
		_k[i] = pow(2.0, 32) * abs(sin(i + 1.0));
	}

}

std::string MD5::getFileMD5(const char* filename)
{
	std::ifstream fin(filename, std::ifstream::binary);
	if (fin.is_open())
	{
		while (!fin.eof())
		{
			fin.read((char*)_chunk, _chunkByte);
			if (_chunkByte != fin.gcount())
				break;
			_totalByte += _chunkByte;
			calculateMD5((size_t*)_chunk);
		}

		// 最后一块数据
		_lastByte = fin.gcount(); // 最后一块数据大小
		_totalByte += _lastByte; // 整个文档的大小
		calculateMD5Final();
	}
	return  changeHex(_a) + changeHex(_b) + changeHex(_c) + changeHex(_d);
}

std::string MD5::getStringMD5(const std::string& str)
{
	if (str.empty())
		return "";
	else
	{
		unsigned char* pstr = (unsigned char*)str.c_str();
		size_t numChunk = str.size() / _chunkByte;
		for (size_t i = 0; i < numChunk; ++i)
		{
			_totalByte += _chunkByte;
			calculateMD5((size_t*)(pstr + i * _chunkByte));
		}
		_lastByte = str.size() % _chunkByte;
		memcpy(_chunk, pstr + _totalByte, _lastByte);
		calculateMD5Final();
		return  changeHex(_a) + changeHex(_b) + changeHex(_c) + changeHex(_d);
	}
}


std::string MD5::changeHex(size_t num)
{
	std::string ret;
	std::string byteStr;
	static std::string strMap = "0123456789abcdef";
	for (int i = 0; i < 4; i++)
	{
		byteStr = "";
		size_t b = (num >> (i * 8)) & 0xff;
		for (int j = 0; j < 2; ++j)
		{
			byteStr.insert(0, 1, strMap[b % 16]);
			b /= 16;
		}
		ret += byteStr;
	}
	return ret;
}

//计算完整块的MD5(512bit) 
void MD5::calculateMD5(size_t* chunk)
{
	size_t a = _a;
	size_t b = _b;
	size_t c = _c;
	size_t d = _d;
	size_t f, g; //chunk[g] f: 哈希函数的返回值

	for (size_t i = 0; i < 64; ++i)
	{
		if (0 <= i && i < 16)
		{
			f = F(b, c, d);
			g = i;
		}
		else if (16 <= i && i < 32)
		{
			f = G(b, c, d);
			g = (5 * i + 1) % 16;
		}
		else if (32 <= i && i < 48)
		{
			f = H(b, c, d);
			g = (3 * i + 5) % 16;
		}
		else
		{
			f = I(b, c, d);
			g = (7 * i) % 16;
		}
		size_t tmp = d;
		d = c;
		c = b;
		b = b + shiftLeftRotate((a + f + _k[i] + chunk[g]), _sft[i]);
		a = tmp;
	}
	_a += a;
	_b += b;
	_c += c;
	_d += d;
}

// 计算最后一个块(不足512bit)
void MD5::calculateMD5Final()
{
	// _lastByte < 64byte ,最后一块字节大小
	unsigned char* p = _chunk + _lastByte;
	// 填充前8位填充：1000 0000 -> 0x80
	*p++ = 0x80;
	// 最后需要填的字节
	size_t remainFillByte = _chunkByte - _lastByte - 1;
	if (remainFillByte < 8)
	{
		memset(p, 0, remainFillByte);
		calculateMD5((size_t*)_chunk);
		memset(_chunk, 0, _chunkByte);
	}
	 // 大于等于8个字节 最后的64bit存放原始文档的bit长度，中间填充0
	else
	{
		memset(p, 0, remainFillByte);
	}
	// 最后的64bit存放原始文档的bit长度
	// _chunk[448] = _totalByte * 8 不可以 
	// _chunk是char类型最大为256 会造成溢出
	((unsigned long long*)_chunk)[7] = _totalByte * 8;
	calculateMD5((size_t*)_chunk);
}



char* MD5::ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn)
{
	LPSTR pszOut = NULL;
	try
	{
		if (lpwszStrIn != NULL)
		{
			int nInputStrLen = wcslen(lpwszStrIn);

			// Double NULL Termination  
			int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
			pszOut = new char[nOutputStrLen];

			if (pszOut)
			{
				memset(pszOut, 0x00, nOutputStrLen);
				WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
			}
		}
	}
	catch (std::exception e)
	{
	}

	return pszOut;
}



char* MD5::openFile()
{
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		return ConvertLPWSTRToLPSTR(ofn.lpstrFile);
	}
	else
	{
		printf("user cancelled\n");
	}
	
}