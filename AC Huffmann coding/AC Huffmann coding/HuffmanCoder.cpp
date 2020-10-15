#include "HuffmanCoder.h"



HuffmanCoder::HuffmanCoder()
{
}


HuffmanCoder::~HuffmanCoder()
{
}

//method to encode a text with Huffman coding
void HuffmanCoder::encodeData(unsigned char * seq, int inputSize)
{
	//create freq model
	std::map<unsigned char, int> freq;// key -> (count of key)
	for (int i = 0; i < inputSize; i++) {
		freq[seq[i]] += 1; //count the occurences of the key
	}

	//safe freq model
	uint64_t freqSize = freq.size();
	BitIO::writeBit(freqSize, 64);

	for (auto it = freq.begin(); it != freq.end(); it++) {
		unsigned char key = it->first;
		uint64_t prob = it->second;

		BitIO::writeBit(key, 8);//save key 

		//safe some space by first encoding the prob len
		uint8_t probLen = 0;
		uint64_t probCopy = prob;
		while (probCopy > 0) {
			probCopy >>= 1;
			probLen++;
		}
		if (probLen == 0)
			probLen = 1;

		BitIO::writeBit(probLen, 6);
		BitIO::writeBit(prob, probLen);//save prob
	}

	BitIO::writeBit(inputSize, 64); //save the number of elements to write

	//build huffman tree
	std::priority_queue<TreeNode*, std::vector<TreeNode*>, myComparator > q;
	std::map<unsigned char, TreeNode*> elements;//vector to hold all alements for fast access

	createHuffmanTree(q, freq, elements);

	std::stack<bool> path;//path of goalNode from root to node

	//encode data with huffman coding
		for (int i = 0; i < inputSize;i++) {
			TreeNode * cur = elements[seq[i]];//get current symbol

			while (cur->nextNode != nullptr) {//find path of symbol to root
				path.push(cur->code);
				cur = cur->nextNode;
			}

			while (!path.empty()) {//encode symbol with path
				BitIO::writeBit(path.top(), 1);
				path.pop();
			}
		}
	
}

void HuffmanCoder::decodeData()
{
	//read and recreate the saved frequenzy table
	uint64_t freqSize = BitIO::readBit(64); //table size safed with 64 bit

	std::map<unsigned char, int> freq;//key -> frequenzy

	for (int i = 0;i < freqSize;i++) {
		uint64_t key = BitIO::readBit(8);
		uint64_t probLen = BitIO::readBit(6);
		uint64_t prob = BitIO::readBit(probLen);

		freq[key] = prob;
	}

	//read in safed data for vector
	uint64_t inputSize = BitIO::readBit(64);
	BitIO::pData = new unsigned char[inputSize];
	BitIO::pSize = inputSize;

	//create huffman tree
	std::priority_queue<TreeNode*, std::vector<TreeNode*>, myComparator > q;
	std::map<unsigned char, TreeNode*> elements;//vector to hold all alements

	createHuffmanTree(q, freq, elements);

	std::stack<bool> path;//path from root to node

	//decode data with huffman coding
	for (int i = 0; i < inputSize; i++) {
		//decode symbol
		TreeNode * cur = q.top();
		while (!cur->isLeaf) {//read in bits until we find a leaf-node(encoded symbol)
			bool curBit = BitIO::readBit(1) ? 1 : 0;//read in bit from data
			if (curBit)//1 right
				cur = cur->right;
			else {//0 left
				cur = cur->left;
			}
		}

		BitIO::pData[i] = cur->data;//push back found symbol
	}

}

HuffmanCoder::TreeNode * HuffmanCoder::createNodeLeaf(unsigned char data, int freq)
{
	TreeNode* nNode = (TreeNode*)malloc(sizeof(TreeNode));
	nNode->data = data;
	nNode->freq = freq;

	nNode->nextNode = nullptr;
	nNode->isLeaf = true;

	return nNode;
}

HuffmanCoder::TreeNode * HuffmanCoder::createInnerNode(int freq, TreeNode * left, TreeNode * right)
{
	TreeNode* nNode = (TreeNode*)malloc(sizeof(TreeNode));
	nNode->freq = freq;
	nNode->left = left;
	nNode->right = right;

	nNode->nextNode = nullptr;
	nNode->isLeaf = false;

	return nNode;
}

//creates Huffman Tree from freq-Tables in queue q
void HuffmanCoder::createHuffmanTree(std::priority_queue<TreeNode*, std::vector<TreeNode*>, myComparator>& q, std::map<unsigned char, int>& freq, std::map<unsigned char, TreeNode*>& elements)
{
	//push in nodes from freq-Tables: Symbols
	for (auto it = freq.begin(); it != freq.end(); it++) {
		TreeNode* cur = createNodeLeaf(it->first, it->second);
		q.push(cur);
		elements[it->first] = cur;
	}

	//merge nodes to create Huffman-Tree in q
	while (q.size() > 1) {
		TreeNode* cur1 = q.top();
		q.pop();

		TreeNode* cur2 = q.top();
		q.pop();

		TreeNode* nNode = createInnerNode(cur1->freq + cur2->freq, cur1, cur2);//create parent node with joint frequency

		cur1->nextNode = nNode; //left: 0
		cur1->code = 0;

		cur2->nextNode = nNode; //right: 1
		cur2->code = 1;

		q.push(nNode);
	}
}
