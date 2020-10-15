#pragma once
#include <queue>
#include <map>
#include <stack>
#include "InputParser.h"

class HuffmanCoder
{
public:

	struct TreeNode
	{
		unsigned char data;//symbol to hold
		int freq;//num of occurances

		bool code;//1 or 0 depending if left or right node
		bool isLeaf = false;

		TreeNode * nextNode;
		TreeNode * left;
		TreeNode * right;

	};

	class myComparator
	{
	public:
		int operator() (const TreeNode * n1, const TreeNode * n2)
		{
			return n1->freq > n2->freq;
		}
	};

	HuffmanCoder();
	~HuffmanCoder();

	void encodeData(unsigned char* seq, int inputSize);
	void decodeData();

	TreeNode* createNodeLeaf(unsigned char data, int freq);
	TreeNode* createInnerNode(int freq, TreeNode* left, TreeNode* right);

	void createHuffmanTree(std::priority_queue<TreeNode*, std::vector<TreeNode*>, myComparator >& q, std::map<unsigned char, int>& freq, std::map<unsigned char, TreeNode*>& elements);

};

