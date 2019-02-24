#pragma once
#include <queue>
#include <vector>


template<class W>
struct HTNode//����huffman���Ľڵ�
{
	HTNode(const W& weight)
		: _pLeft(nullptr)
		, _pRight(nullptr)
		, _pParent(nullptr)
		, _weight(weight)
	{}
	HTNode<W>* _pLeft;//����ָ��
	HTNode<W>* _pRight;//�Һ���ָ��
	HTNode<W>* _pParent;//˫�׽ڵ�ָ��
	W _weight;//Ȩֵ
};

//�º���
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
class HuffmanTree//huffman��
{
	typedef HTNode<W> Node;
	typedef Node* pNode;

public:
	HuffmanTree()//���캯����ʼ�����ڵ�
		:_pRoot(nullptr)
	{}

	~HuffmanTree()
	{
		Destroy(_pRoot);
	}

	void CreateHuffmanTree(const std::vector<W>& V, const W& invalid)//��Ȩֵ����huffman����������ͳ�Ƴ������ַ�������Ȩֵ��
	{
		if (V.empty())//���������Ϊ�գ���û��һ��Ȩֵ����ֱ�ӷ��أ����ô���huffman����
			return;

		//�� С��---���ȼ����� ����������ɭ��
		std::priority_queue<pNode, std::vector<pNode>, Compare<W>> hp;
		for (size_t i = 0; i < V.size(); ++i)
		{
			if (V[i] != invalid)//����ṹ�������е�ĳһ��Ԫ�ء��ַ��ṹ�塿������Ч�ġ���Ч�ģ��ṹ���е��ַ�������0��
				hp.push(new Node(V[i]));//����Ȩֵ
		}

		while (hp.size() > 1)//�������������������1������ѭ����ֱ��ֻʣ��һ�Ŷ��������ö���������huffman��
		{
			pNode pLeft = hp.top();//ѡȡ���ڵ���С����ʵ������С�Ѹ��ڵ㡿�Ķ�������������
			hp.pop();//ɾ��������ɭ���и��ڵ���С�Ķ�����

			pNode pRight = hp.top();//ѡȡ���ڵ���С����ʵ������С�Ѹ��ڵ㡿�Ķ����������Һ���
			hp.pop();//ɾ��������ɭ���и��ڵ���С�Ķ�����

			pNode pParent = new Node(pLeft->_weight + pRight->_weight);//����һ���½ڵ㣬���������Һ��ӵ�˫�׽ڵ�
			pParent->_pLeft = pLeft;//�����µĶ���������
			pLeft->_pParent = pParent;//����˫�׽ڵ�����ӽڵ�

			pParent->_pRight = pRight;//�����µĶ������Һ���
			pRight->_pParent = pParent;//����˫�׽ڵ���Һ��ӽڵ�

			hp.push(pParent);//���µĶ��������뵽������ɭ����ȥ
		}
		_pRoot = hp.top();//������ɭ����ֻʣ��huffman�������ڵ��ǶѶ�
	}

	pNode GetpRoot()
	{
		return _pRoot;//��ȡ���ڵ�
	}

private:
	void Destroy(pNode& pParent)//���ٴ����Ľڵ�
	{
		if (pParent)
		{
			Destroy(pParent->_pLeft);//ɾ������
			Destroy(pParent->_pRight);//ɾ���Һ���
			delete pParent;//ɾ�����ڵ�
			pParent = nullptr;//���ڵ��ÿ�
		}
	}
private:
	pNode _pRoot;//���ڵ�
};






