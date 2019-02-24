#define _CRT_SECURE_NO_WARNINGS
#include "FileCompress.h"
#include "HuffmanTree.hpp"
#include <iostream>
#include <string>
#include <assert.h>
using namespace std;

FileCompress::FileCompress()//构造函数初始化结构体数组
{
	_CharInfo.resize(256);//初始化vector数组的容量
	for (size_t i = 0; i < 256; ++i)
	{
		_CharInfo[i]._ch = i;
		_CharInfo[i]._count = 0;
	}
}

void FileCompress::CompressFile(const std::string& strFilePath)//文件压缩函数
{
	//1 统计文件的每个字符的个数
	FILE* Fin = fopen(strFilePath.c_str(), "rb");//以只读方式打开源文件，第一个参数是调用string类调用c_str()函数将路径改为c风格的字符串形式
	if (nullptr == Fin)
	{
		cout << "文件打开失败" << endl;
		return;
	}
	
	UNC* pReadBuff = new UNC[1024];//堆上动态开辟1024个字节的空间，做fread()的第一个参数，用来存储从文件中一次读取的字符个数

	while (1){
		size_t rdSize = fread(pReadBuff, 1, 1024, Fin);//从源文件中读取字符个数，参数1【读取的内容放到这个空间】，参数2【一个字符占一个字节】，参数3【一次读1024个字节数】，参数4【从该指针指向的文件中读取字符】
		if (0 == rdSize)
			break;//读到文件末尾，就会读到0个字符，此时就停下来不需要读了
		for (size_t i = 0; i < rdSize; ++i)//i < 1024不能这样写，是因为当有1025个字符的源文件时，读第二次只有1个字符需要统计，那么这样写就会有问题
			_CharInfo[pReadBuff[i]]._count++;//找到堆上读取的字符对应的ASCII码值，以其作为数组下标，直接对应相应的字符，直接++即可统计出相应字符的次数
	}

	//2 利用权值【字符个数】创建Huffman树
	HuffmanTree<CharInfo> ht;
	ht.CreateHuffmanTree(_CharInfo, CharInfo(0));

	//3 通过Huffman树找到每个字符的编码
	GetHuffmanCode(ht.GetpRoot());
	//4 使用编码重新改写源文件
	char ch = 0;
	char bitcount = 0;

	std::string strfilename(strFilePath.c_str());
	size_t finish = strfilename.rfind('.');//从后往前，找到点的下标
	std::string strUNname(strfilename.substr(0, finish));//从起始位置，截取finish个字符
	strUNname += ".hzp";
	FILE* Fout = fopen(strUNname.c_str(), "wb");//创建一个压缩文件
	assert(Fout);//断言一下，压缩文件是否正常打开

	WriteHeadInfo(Fout, strFilePath);//在压缩文件头部写入文件压缩的信息，为了给解压缩提供信息
	fseek(Fin, 0, SEEK_SET);//因为之前已经读过一遍Fin文件，现在Fin指向文件的末尾了，所以需要偏移到文件的开头
	while (1)
	{
		size_t rdsize = fread(pReadBuff, 1, 1024, Fin);
		if (0 == rdsize)
			break;//读一遍源文件
		
		for (size_t i = 0; i < rdsize; ++i)//遍历文件中所有的字符
		{
			string& strCode = _CharInfo[pReadBuff[i]]._strCode;//找到缓冲区数组中第一个字符的huffman编码
			for (size_t j = 0; j < strCode.size(); ++j)//遍历每个字符的字符串编码
			{
				ch <<= 1;//每次或之后，必须向左移动一位，给下一个字符留下位置
				if ('1' == strCode[j])//如果字符串的字符等于‘1’，就按位ch|=1
					ch |= 1;//每一位本来是0，或之后就是1，所以遇到字符1需要或一下，遇到字符0就不用或

				bitcount++;//每次在ch上存一位，bitcount就自增1
				if (8 == bitcount)//当bitcount增加到8时，一个字节就满了，就需要往压缩文件中去写了
				{
					fputc(ch, Fout);//按照单个字节写入到压缩文件中去
					ch = 0;//单个字节写入压缩文件之后，这个存储字节就要清零，准备下一字节的存储
					bitcount = 0;//统计字节写够8位的bitcount也需要清零，重新统计下一轮的存储位数
				}
			}
		}
	}

	//最后读取的一行编码如果没有满8bit位，那么就不会写入到压缩文件中去
	if (bitcount > 0 && bitcount < 8)//如果bitcount刚刚好等于0，则说明最后一行刚刚满8bit位，就不需要再压缩一遍了
	{
		ch <<= (8 - bitcount);//移动的位数是高位剩几位，就往高位移动几位
		fputc(ch, Fout);//没有满8bit位，写入之前需要把低几位整体左移到最高位，因为压缩是从高位往低位压缩的
	}

	delete[] pReadBuff;
	fclose(Fin);
	fclose(Fout);
}


//解压缩
void FileCompress::UNCompressFile(const std::string& strFilePath)
{
	//判断文件后缀是否正确
	std::string postFix = strFilePath.substr(strFilePath.rfind('.'));
	if (".hzp" != postFix)
	{
		cout << "压缩文件格式不对" << endl;
		return;
	}

	FILE* fIn = fopen(strFilePath.c_str(), "rb");
	if (nullptr == fIn)
	{
		cout << "压缩文件打开失败" << endl;
		return;
	}

	//提取压缩文件头部信息第一行【后缀名】
	postFix = "";
	Getline(fIn, postFix);

	//通过压缩文件第二行有效编码行数得到有效字符个数权值
	std::string strContent;
	Getline(fIn, strContent);
	size_t LineCount = 0;
    LineCount = atoi(strContent.c_str());//把字符转换为十进制数

	unsigned long long charCount = 0;
	for (size_t i = 0; i < LineCount; ++i)
	{
		strContent = "";//首先清空字符串
		Getline(fIn, strContent);//获取有效编码行数下的第一行
		if ("" == strContent)//处理换行符，因为刚刚读取到换行符的时候，Getline函数直接返回，没有写入文件
		{
			strContent += "\n";
			Getline(fIn, strContent);//注意：易错点，记得要处理换行符
		}

		charCount = atoi(strContent.c_str() + 2);//[A,1]
		_CharInfo[(UNC)strContent[0]]._count = charCount;//易错点，需要类型强转，否则数组下标越界
	}

	//重建huffman树
	HuffmanTree<CharInfo> ht;//类的实例化
	ht.CreateHuffmanTree(_CharInfo, CharInfo(0));

	//解压缩
	std::string strUNFileName("2");//解压缩到该文件 
	strUNFileName += postFix;//文件名加上源文件后缀名
	FILE* fOut = fopen(strUNFileName.c_str(), "wb");//打开解压缩后放内容的文件
	if (nullptr == fOut)
	{
		cout << "打开文件失败" << endl;
		return;
	}
	char* rdBuff = new char[1024];//存储缓冲区
	int pos = 7;
	HTNode<CharInfo>* pCur = ht.GetpRoot();
	unsigned long long fileSize = pCur->_weight._count;

	while (1)
	{
		size_t rdSize = fread(rdBuff, 1, 1024, fIn);//从压缩文件中读取内容至缓冲区
		if (0 == rdSize)
			break;

		for (size_t i = 0; i < rdSize; ++i)//循环rdSize个字节
		{
			pos = 7;
			for (size_t j = 0; j < 8; ++j)//一个字节8位[10010110 11011111 11111100 00000]
			{
				if (rdBuff[i] & (1 << pos))
					pCur = pCur->_pRight;//如果该位是1就往右子树移动
				else
					pCur = pCur->_pLeft;//否则就往左子树移动

				if (nullptr == pCur->_pLeft && nullptr == pCur->_pRight)//当遇到叶子节点时
				{
					fputc(pCur->_weight._ch, fOut);//就可以把叶子节点的字符插入到解压缩文件中去
					pCur = ht.GetpRoot();//让pCur回到根节点
					--fileSize;//解压一个字符根节点总数就少一个
					if (0 == fileSize)//当根节点字符权值总数减少到0时，压缩就完毕了
						break;
				}

				pos--;
				if (pos < 0)//当第一个字节结束时，就需要跳出8位循环，开始下一个字节的循环
					break;
			}
		}
	}

	delete[] rdBuff;
	fclose(fIn);
	fclose(fOut);
}


void FileCompress::GetHuffmanCode(HTNode<CharInfo>* pRoot)//获取huffman编码
{
	if (nullptr == pRoot)
		return;

	GetHuffmanCode(pRoot->_pLeft);//递归遍历左节点
	GetHuffmanCode(pRoot->_pRight);//递归遍历右节点

	if (nullptr == pRoot->_pLeft && nullptr == pRoot->_pRight)//左节点和右节点同时为空，就是叶子节点
	{
		HTNode<CharInfo>* pCur = pRoot;//叶子节点定为pCur
		HTNode<CharInfo>* pParent = pCur->_pParent;//该叶子节点称为pParent

		std::string& strCode = _CharInfo[pCur->_weight._ch]._strCode;
		while (pParent)//一直倒着从叶子节点到根节点进行编码,直到跟节点为空
		{
			if (pCur == pParent->_pLeft)//如果pCur节点是双亲节点的左子树，置0
				strCode += '0';
			else//如果pCur节点是双亲节点的右子树，置1
				strCode += '1';

			pCur = pParent;
			pParent = pCur->_pParent;
		}
		reverse(strCode.begin(), strCode.end());//我们是从叶子节点到根节点倒序排列字符串编码的，现在需要逆序恢复从根节点到叶子节点的编码排序
	}
}

//压缩文件的头部信息保留着原来的压缩信息
void FileCompress::WriteHeadInfo(FILE* pf, const std::string& strFileName)
{
	//获取源文件后缀
	std::string postFix = strFileName.substr(strFileName.rfind('.'));
	//有效编码的行数
	size_t LineCount = 0;
	//有效字符及其出现的次数
	std::string strCharCount;
	char buff[1024] = { 0 };
	for (size_t i = 0; i < 256; ++i)//通过这个for循环，就可以从结构体数组中得到有效字符信息
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

	//把三部分内容准备好后，开始整合三部分内容
	std::string strHeadInfo;
	strHeadInfo += postFix;
	strHeadInfo += "\n";

	memset(buff, 0, 1024);
	_itoa(LineCount, buff, 10);
	strHeadInfo += buff;
	strHeadInfo += "\n";

	strHeadInfo += strCharCount;
	
	//整合信息完毕后，写入到压缩文件中去
	fwrite(strHeadInfo.c_str(), 1, strHeadInfo.size(), pf);
}


//按行获取压缩文件首部信息
void FileCompress::Getline(FILE* pf, std::string& strContent)
{
	while (!feof(pf))//没有到达文件结尾
	{
		char ch = fgetc(pf);//逐个字符的读取
		if ('\n' == ch)//读到换行符，说明一行读完了
			break;

		strContent += ch;//该字符串不断添加字符
	}
}