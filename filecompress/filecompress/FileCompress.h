#pragma once 
#include "HuffmanTree.hpp"
#include <string>
#include <vector>
using namespace std;

typedef unsigned char UNC;

struct CharInfo//一个字符的结构体集合
{
	CharInfo(unsigned long long count = 0)
		:_count(count)
	{}
	UNC _ch;//存储一个字符[有符号的char型最大范围是-128~127，但是遇到汉字的时候，汉字的编码可能大于127，一旦大于127就是负数了，以后需要用此编码做下标，就会出问题，所以需要使用无符号类型]
	unsigned long long _count;//统计该字符的个数
	std::string _strCode;//表示该字符的字符串编码

	CharInfo operator+(const CharInfo& c)
	{
		return CharInfo(_count + c._count);
	}

	bool operator>(const CharInfo& c)
	{
		return _count > c._count;
	}

	bool operator!=(const CharInfo& c)const
	{
		return _count != c._count;
	}
};

class FileCompress
{
public:
	FileCompress();//声明构造函数初始化结构体数组
	void CompressFile(const std::string& strFilePath);//文件压缩函数
	void UNCompressFile(const std::string& strFilePath);//文件解压缩函数

private:
	void GetHuffmanCode(HTNode<CharInfo>* pRoot);//huffman编码
	void WriteHeadInfo(FILE* pf, const std::string& strFileName);//解压缩需要原来的文件信息【文件后缀、字符行数、字符及其对应的次数、压缩文件】
	void Getline(FILE* pf, std::string& strContent);
private:
	std::vector<CharInfo> _CharInfo;//表示256个字符结构体数组
};