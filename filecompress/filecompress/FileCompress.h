#pragma once 
#include "HuffmanTree.hpp"
#include <string>
#include <vector>
using namespace std;

typedef unsigned char UNC;

struct CharInfo//һ���ַ��Ľṹ�弯��
{
	CharInfo(unsigned long long count = 0)
		:_count(count)
	{}
	UNC _ch;//�洢һ���ַ�[�з��ŵ�char�����Χ��-128~127�������������ֵ�ʱ�򣬺��ֵı�����ܴ���127��һ������127���Ǹ����ˣ��Ժ���Ҫ�ô˱������±꣬�ͻ�����⣬������Ҫʹ���޷�������]
	unsigned long long _count;//ͳ�Ƹ��ַ��ĸ���
	std::string _strCode;//��ʾ���ַ����ַ�������

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
	FileCompress();//�������캯����ʼ���ṹ������
	void CompressFile(const std::string& strFilePath);//�ļ�ѹ������
	void UNCompressFile(const std::string& strFilePath);//�ļ���ѹ������

private:
	void GetHuffmanCode(HTNode<CharInfo>* pRoot);//huffman����
	void WriteHeadInfo(FILE* pf, const std::string& strFileName);//��ѹ����Ҫԭ�����ļ���Ϣ���ļ���׺���ַ��������ַ������Ӧ�Ĵ�����ѹ���ļ���
	void Getline(FILE* pf, std::string& strContent);
private:
	std::vector<CharInfo> _CharInfo;//��ʾ256���ַ��ṹ������
};