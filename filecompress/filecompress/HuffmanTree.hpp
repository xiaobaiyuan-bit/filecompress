#pragma once
#include <queue>
#include <vector>


template<class W>
struct HTNode//创建huffman树的节点
{
	HTNode(const W& weight)
		: _pLeft(nullptr)
		, _pRight(nullptr)
		, _pParent(nullptr)
		, _weight(weight)
	{}
	HTNode<W>* _pLeft;//左孩子指针
	HTNode<W>* _pRight;//右孩子指针
	HTNode<W>* _pParent;//双亲节点指针
	W _weight;//权值
};

//仿函数
template<class W>
class Compare
{
public:
	bool operator()(HTNode<W>* pLeft, HTNode<W>* pRight)
	{
		if (pLeft->_weight > pRight->_weight)
			return true;

		return false;
	}
};

template<class W>
class HuffmanTree//huffman树
{
	typedef HTNode<W> Node;
	typedef Node* pNode;

public:
	HuffmanTree()//构造函数初始化根节点
		:_pRoot(nullptr)
	{}

	~HuffmanTree()
	{
		Destroy(_pRoot);
	}

	void CreateHuffmanTree(const std::vector<W>& V, const W& invalid)//用权值创建huffman树【参数是统计出来的字符个数即权值】
	{
		if (V.empty())//如果数组中为空，即没有一个权值，就直接返回，不用创建huffman树了
			return;

		//用 小堆---优先级队列 创建二叉树森林
		std::priority_queue<pNode, std::vector<pNode>, Compare<W>> hp;
		for (size_t i = 0; i < V.size(); ++i)
		{
			if (V[i] != invalid)//如果结构体数组中的某一个元素【字符结构体】不是无效的【无效的：结构体中的字符个数是0】
				hp.push(new Node(V[i]));//插入权值
		}

		while (hp.size() > 1)//当堆里二叉树个数大于1，继续循环，直到只剩下一颗二叉树，该二叉树就是huffman树
		{
			pNode pLeft = hp.top();//选取根节点最小【其实就是最小堆根节点】的二叉树当做左孩子
			hp.pop();//删除二叉树森林中根节点最小的二叉树

			pNode pRight = hp.top();//选取根节点最小【其实就是最小堆根节点】的二叉树当做右孩子
			hp.pop();//删除二叉树森林中根节点最小的二叉树

			pNode pParent = new Node(pLeft->_weight + pRight->_weight);//创建一个新节点，来保存左右孩子的双亲节点
			pParent->_pLeft = pLeft;//创建新的二叉树左孩子
			pLeft->_pParent = pParent;//连接双亲节点和左孩子节点

			pParent->_pRight = pRight;//创建新的二叉树右孩子
			pRight->_pParent = pParent;//连接双亲节点和右孩子节点

			hp.push(pParent);//把新的二叉树插入到二叉树森林中去
		}
		_pRoot = hp.top();//二叉树森林中只剩下huffman树，根节点是堆顶
	}

	pNode GetpRoot()
	{
		return _pRoot;//获取根节点
	}

private:
	void Destroy(pNode& pParent)//销毁创建的节点
	{
		if (pParent)
		{
			Destroy(pParent->_pLeft);//删除左孩子
			Destroy(pParent->_pRight);//删除右孩子
			delete pParent;//删除根节点
			pParent = nullptr;//根节点置空
		}
	}
private:
	pNode _pRoot;//根节点
};






