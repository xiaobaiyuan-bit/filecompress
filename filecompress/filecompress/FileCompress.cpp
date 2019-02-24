#define _CRT_SECURE_NO_WARNINGS
#include "FileCompress.h"
#include "HuffmanTree.hpp"
#include <iostream>
#include <string>
#include <assert.h>
using namespace std;

FileCompress::FileCompress()//���캯����ʼ���ṹ������
{
	_CharInfo.resize(256);//��ʼ��vector���������
	for (size_t i = 0; i < 256; ++i)
	{
		_CharInfo[i]._ch = i;
		_CharInfo[i]._count = 0;
	}
}

void FileCompress::CompressFile(const std::string& strFilePath)//�ļ�ѹ������
{
	//1 ͳ���ļ���ÿ���ַ��ĸ���
	FILE* Fin = fopen(strFilePath.c_str(), "rb");//��ֻ����ʽ��Դ�ļ�����һ�������ǵ���string�����c_str()������·����Ϊc�����ַ�����ʽ
	if (nullptr == Fin)
	{
		cout << "�ļ���ʧ��" << endl;
		return;
	}
	
	UNC* pReadBuff = new UNC[1024];//���϶�̬����1024���ֽڵĿռ䣬��fread()�ĵ�һ�������������洢���ļ���һ�ζ�ȡ���ַ�����

	while (1){
		size_t rdSize = fread(pReadBuff, 1, 1024, Fin);//��Դ�ļ��ж�ȡ�ַ�����������1����ȡ�����ݷŵ�����ռ䡿������2��һ���ַ�ռһ���ֽڡ�������3��һ�ζ�1024���ֽ�����������4���Ӹ�ָ��ָ����ļ��ж�ȡ�ַ���
		if (0 == rdSize)
			break;//�����ļ�ĩβ���ͻ����0���ַ�����ʱ��ͣ��������Ҫ����
		for (size_t i = 0; i < rdSize; ++i)//i < 1024��������д������Ϊ����1025���ַ���Դ�ļ�ʱ�����ڶ���ֻ��1���ַ���Ҫͳ�ƣ���ô����д�ͻ�������
			_CharInfo[pReadBuff[i]]._count++;//�ҵ����϶�ȡ���ַ���Ӧ��ASCII��ֵ��������Ϊ�����±ֱ꣬�Ӷ�Ӧ��Ӧ���ַ���ֱ��++����ͳ�Ƴ���Ӧ�ַ��Ĵ���
	}

	//2 ����Ȩֵ���ַ�����������Huffman��
	HuffmanTree<CharInfo> ht;
	ht.CreateHuffmanTree(_CharInfo, CharInfo(0));

	//3 ͨ��Huffman���ҵ�ÿ���ַ��ı���
	GetHuffmanCode(ht.GetpRoot());
	//4 ʹ�ñ������¸�дԴ�ļ�
	char ch = 0;
	char bitcount = 0;

	std::string strfilename(strFilePath.c_str());
	size_t finish = strfilename.rfind('.');//�Ӻ���ǰ���ҵ�����±�
	std::string strUNname(strfilename.substr(0, finish));//����ʼλ�ã���ȡfinish���ַ�
	strUNname += ".hzp";
	FILE* Fout = fopen(strUNname.c_str(), "wb");//����һ��ѹ���ļ�
	assert(Fout);//����һ�£�ѹ���ļ��Ƿ�������

	WriteHeadInfo(Fout, strFilePath);//��ѹ���ļ�ͷ��д���ļ�ѹ������Ϣ��Ϊ�˸���ѹ���ṩ��Ϣ
	fseek(Fin, 0, SEEK_SET);//��Ϊ֮ǰ�Ѿ�����һ��Fin�ļ�������Finָ���ļ���ĩβ�ˣ�������Ҫƫ�Ƶ��ļ��Ŀ�ͷ
	while (1)
	{
		size_t rdsize = fread(pReadBuff, 1, 1024, Fin);
		if (0 == rdsize)
			break;//��һ��Դ�ļ�
		
		for (size_t i = 0; i < rdsize; ++i)//�����ļ������е��ַ�
		{
			string& strCode = _CharInfo[pReadBuff[i]]._strCode;//�ҵ������������е�һ���ַ���huffman����
			for (size_t j = 0; j < strCode.size(); ++j)//����ÿ���ַ����ַ�������
			{
				ch <<= 1;//ÿ�λ�֮�󣬱��������ƶ�һλ������һ���ַ�����λ��
				if ('1' == strCode[j])//����ַ������ַ����ڡ�1�����Ͱ�λch|=1
					ch |= 1;//ÿһλ������0����֮�����1�����������ַ�1��Ҫ��һ�£������ַ�0�Ͳ��û�

				bitcount++;//ÿ����ch�ϴ�һλ��bitcount������1
				if (8 == bitcount)//��bitcount���ӵ�8ʱ��һ���ֽھ����ˣ�����Ҫ��ѹ���ļ���ȥд��
				{
					fputc(ch, Fout);//���յ����ֽ�д�뵽ѹ���ļ���ȥ
					ch = 0;//�����ֽ�д��ѹ���ļ�֮������洢�ֽھ�Ҫ���㣬׼����һ�ֽڵĴ洢
					bitcount = 0;//ͳ���ֽ�д��8λ��bitcountҲ��Ҫ���㣬����ͳ����һ�ֵĴ洢λ��
				}
			}
		}
	}

	//����ȡ��һ�б������û����8bitλ����ô�Ͳ���д�뵽ѹ���ļ���ȥ
	if (bitcount > 0 && bitcount < 8)//���bitcount�ոպõ���0����˵�����һ�иո���8bitλ���Ͳ���Ҫ��ѹ��һ����
	{
		ch <<= (8 - bitcount);//�ƶ���λ���Ǹ�λʣ��λ��������λ�ƶ���λ
		fputc(ch, Fout);//û����8bitλ��д��֮ǰ��Ҫ�ѵͼ�λ�������Ƶ����λ����Ϊѹ���ǴӸ�λ����λѹ����
	}

	delete[] pReadBuff;
	fclose(Fin);
	fclose(Fout);
}


//��ѹ��
void FileCompress::UNCompressFile(const std::string& strFilePath)
{
	//�ж��ļ���׺�Ƿ���ȷ
	std::string postFix = strFilePath.substr(strFilePath.rfind('.'));
	if (".hzp" != postFix)
	{
		cout << "ѹ���ļ���ʽ����" << endl;
		return;
	}

	FILE* fIn = fopen(strFilePath.c_str(), "rb");
	if (nullptr == fIn)
	{
		cout << "ѹ���ļ���ʧ��" << endl;
		return;
	}

	//��ȡѹ���ļ�ͷ����Ϣ��һ�С���׺����
	postFix = "";
	Getline(fIn, postFix);

	//ͨ��ѹ���ļ��ڶ�����Ч���������õ���Ч�ַ�����Ȩֵ
	std::string strContent;
	Getline(fIn, strContent);
	size_t LineCount = 0;
    LineCount = atoi(strContent.c_str());//���ַ�ת��Ϊʮ������

	unsigned long long charCount = 0;
	for (size_t i = 0; i < LineCount; ++i)
	{
		strContent = "";//��������ַ���
		Getline(fIn, strContent);//��ȡ��Ч���������µĵ�һ��
		if ("" == strContent)//�����з�����Ϊ�ոն�ȡ�����з���ʱ��Getline����ֱ�ӷ��أ�û��д���ļ�
		{
			strContent += "\n";
			Getline(fIn, strContent);//ע�⣺�״�㣬�ǵ�Ҫ�����з�
		}

		charCount = atoi(strContent.c_str() + 2);//[A,1]
		_CharInfo[(UNC)strContent[0]]._count = charCount;//�״�㣬��Ҫ����ǿת�����������±�Խ��
	}

	//�ؽ�huffman��
	HuffmanTree<CharInfo> ht;//���ʵ����
	ht.CreateHuffmanTree(_CharInfo, CharInfo(0));

	//��ѹ��
	std::string strUNFileName("2");//��ѹ�������ļ� 
	strUNFileName += postFix;//�ļ�������Դ�ļ���׺��
	FILE* fOut = fopen(strUNFileName.c_str(), "wb");//�򿪽�ѹ��������ݵ��ļ�
	if (nullptr == fOut)
	{
		cout << "���ļ�ʧ��" << endl;
		return;
	}
	char* rdBuff = new char[1024];//�洢������
	int pos = 7;
	HTNode<CharInfo>* pCur = ht.GetpRoot();
	unsigned long long fileSize = pCur->_weight._count;

	while (1)
	{
		size_t rdSize = fread(rdBuff, 1, 1024, fIn);//��ѹ���ļ��ж�ȡ������������
		if (0 == rdSize)
			break;

		for (size_t i = 0; i < rdSize; ++i)//ѭ��rdSize���ֽ�
		{
			pos = 7;
			for (size_t j = 0; j < 8; ++j)//һ���ֽ�8λ[10010110 11011111 11111100 00000]
			{
				if (rdBuff[i] & (1 << pos))
					pCur = pCur->_pRight;//�����λ��1�����������ƶ�
				else
					pCur = pCur->_pLeft;//��������������ƶ�

				if (nullptr == pCur->_pLeft && nullptr == pCur->_pRight)//������Ҷ�ӽڵ�ʱ
				{
					fputc(pCur->_weight._ch, fOut);//�Ϳ��԰�Ҷ�ӽڵ���ַ����뵽��ѹ���ļ���ȥ
					pCur = ht.GetpRoot();//��pCur�ص����ڵ�
					--fileSize;//��ѹһ���ַ����ڵ���������һ��
					if (0 == fileSize)//�����ڵ��ַ�Ȩֵ�������ٵ�0ʱ��ѹ���������
						break;
				}

				pos--;
				if (pos < 0)//����һ���ֽڽ���ʱ������Ҫ����8λѭ������ʼ��һ���ֽڵ�ѭ��
					break;
			}
		}
	}

	delete[] rdBuff;
	fclose(fIn);
	fclose(fOut);
}


void FileCompress::GetHuffmanCode(HTNode<CharInfo>* pRoot)//��ȡhuffman����
{
	if (nullptr == pRoot)
		return;

	GetHuffmanCode(pRoot->_pLeft);//�ݹ������ڵ�
	GetHuffmanCode(pRoot->_pRight);//�ݹ�����ҽڵ�

	if (nullptr == pRoot->_pLeft && nullptr == pRoot->_pRight)//��ڵ���ҽڵ�ͬʱΪ�գ�����Ҷ�ӽڵ�
	{
		HTNode<CharInfo>* pCur = pRoot;//Ҷ�ӽڵ㶨ΪpCur
		HTNode<CharInfo>* pParent = pCur->_pParent;//��Ҷ�ӽڵ��ΪpParent

		std::string& strCode = _CharInfo[pCur->_weight._ch]._strCode;
		while (pParent)//һֱ���Ŵ�Ҷ�ӽڵ㵽���ڵ���б���,ֱ�����ڵ�Ϊ��
		{
			if (pCur == pParent->_pLeft)//���pCur�ڵ���˫�׽ڵ������������0
				strCode += '0';
			else//���pCur�ڵ���˫�׽ڵ������������1
				strCode += '1';

			pCur = pParent;
			pParent = pCur->_pParent;
		}
		reverse(strCode.begin(), strCode.end());//�����Ǵ�Ҷ�ӽڵ㵽���ڵ㵹�������ַ�������ģ�������Ҫ����ָ��Ӹ��ڵ㵽Ҷ�ӽڵ�ı�������
	}
}

//ѹ���ļ���ͷ����Ϣ������ԭ����ѹ����Ϣ
void FileCompress::WriteHeadInfo(FILE* pf, const std::string& strFileName)
{
	//��ȡԴ�ļ���׺
	std::string postFix = strFileName.substr(strFileName.rfind('.'));
	//��Ч���������
	size_t LineCount = 0;
	//��Ч�ַ�������ֵĴ���
	std::string strCharCount;
	char buff[1024] = { 0 };
	for (size_t i = 0; i < 256; ++i)//ͨ�����forѭ�����Ϳ��Դӽṹ�������еõ���Ч�ַ���Ϣ
	{
		if (0 != _CharInfo[i]._count)
		{
			strCharCount += _CharInfo[i]._ch;
			strCharCount += ',';
			memset(buff, 0, 1024);
			_itoa(_CharInfo[i]._count, buff ,10);
			strCharCount += buff;
			strCharCount += "\n";
			LineCount++;
		}
	}

	//������������׼���ú󣬿�ʼ��������������
	std::string strHeadInfo;
	strHeadInfo += postFix;
	strHeadInfo += "\n";

	memset(buff, 0, 1024);
	_itoa(LineCount, buff, 10);
	strHeadInfo += buff;
	strHeadInfo += "\n";

	strHeadInfo += strCharCount;
	
	//������Ϣ��Ϻ�д�뵽ѹ���ļ���ȥ
	fwrite(strHeadInfo.c_str(), 1, strHeadInfo.size(), pf);
}


//���л�ȡѹ���ļ��ײ���Ϣ
void FileCompress::Getline(FILE* pf, std::string& strContent)
{
	while (!feof(pf))//û�е����ļ���β
	{
		char ch = fgetc(pf);//����ַ��Ķ�ȡ
		if ('\n' == ch)//�������з���˵��һ�ж�����
			break;

		strContent += ch;//���ַ�����������ַ�
	}
}