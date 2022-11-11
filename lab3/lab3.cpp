#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

// A Tree node
struct Node
{
	char ch;
	int freq;
	Node* left, * right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// Comparison object to be used to order the heap
struct comp
{
	bool operator()(Node* l, Node* r)
	{
		// highest priority item has lowest frequency
		return l->freq > r->freq;
	}
};

// traverse the Huffman Tree and store Huffman Codes
// in a map.
void encode(Node* root, string str,
	unordered_map<char, string>& huffmanCode)
{
	if (root == nullptr)
		return;

	// found a leaf node
	if (!root->left && !root->right) {
		huffmanCode[root->ch] = str;
	}

	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}

// traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int& index, string str)
{
	if (root == nullptr) {
		return;
	}

	// found a leaf node
	if (!root->left && !root->right)
	{
		cout << root->ch;
		return;
	}

	index++;

	if (str[index] == '0')
		decode(root->left, index, str);
	else
		decode(root->right, index, str);
}

// Builds Huffman Tree and decode given input text
void buildHuffmanTree(string text)
{
	// count frequency of appearance of each character
	// and store it in a map
	unordered_map<char, int> freq;
	for (char ch : text) {
		freq[ch]++;
	}

	// Create a priority queue to store live nodes of
	// Huffman tree;
	priority_queue<Node*, vector<Node*>, comp> pq;

	// Create a leaf node for each character and add it
	// to the priority queue.
	for (auto pair : freq) {
		pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
	}

	// do till there is more than one node in the queue
	while (pq.size() != 1)
	{
		// Remove the two nodes of highest priority
		// (lowest frequency) from the queue
		Node* left = pq.top(); pq.pop();
		Node* right = pq.top();	pq.pop();

		// Create a new internal node with these two nodes
		// as children and with frequency equal to the sum
		// of the two nodes' frequencies. Add the new node
		// to the priority queue.
		int sum = left->freq + right->freq;
		pq.push(getNode('\0', sum, left, right));
	}

	// root stores pointer to root of Huffman Tree
	Node* root = pq.top();

	// traverse the Huffman Tree and store Huffman Codes
	// in a map. Also prints them
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	cout << "Huffman Codes are :\n" << '\n';
	for (auto pair : huffmanCode) {
		cout << pair.first << " " << pair.second << '\n';
	}

	cout << "\nOriginal string was :\n" << text << '\n';

	// print encoded string
	string str = "";
	for (char ch : text) {
		str += huffmanCode[ch];
	}

	cout << "\nEncoded string is :\n" << str << '\n';

	// traverse the Huffman Tree again and this time
	// decode the encoded string
	int index = -1;
	cout << "\nDecoded string is: \n";
	while (index < (int)str.size() - 2) {
		decode(root, index, str);
	}
}

// Huffman coding algorithm
int main()
{
	string text = "Huffman coding is a data compression algorithm.";

	buildHuffmanTree(text);

	return 0;
}











//#include <iostream>
//#include <vector>
//#include <map>
//#include <list>
//#include <fstream>
//using namespace std;
//
//class Node
//{
//public:
//	int weight;
//	char ch;
//	Node* left, * right;
//
//	Node()
//	{
//		left = right = NULL;
//	}
//
//	Node(Node* L, Node* R)
//	{
//		left = L;
//		right = R;
//		weight = L->weight + R->weight;
//	}
//};
//
//
//struct MyCompare
//{
//	bool operator()(const Node* l, const Node* r) const
//	{
//		return l->weight < r->weight;
//	}
//};
//
//
//vector<bool> code;
//map<char, vector<bool> > table;
//
//void BuildTable(Node* root)
//{
//	if (root->left != NULL)
//	{
//		code.push_back(0);
//		BuildTable(root->left);
//	}
//
//	if (root->right != NULL)
//	{
//		code.push_back(1);
//		BuildTable(root->right);
//	}
//
//	if (root->left == NULL && root->right == NULL)
//	{
//		table[root->ch] = code;
//	}
//
//	code.pop_back();
//}
//
//
//int main(int argc, char* argv[])
//{
//	////// считаем частоты символов	
//	ifstream f("1.txt", ios::out | ios::binary);
//
//	map<char, int> m;
//
//	while (!f.eof())
//	{
//		char c = f.get();
//		m[c]++;
//	}
//
//
//	////// записываем начальные узлы в список list
//
//	list<Node*> t;
//	for (map<char, int>::iterator itr = m.begin(); itr != m.end(); ++itr)
//	{
//		Node* p = new Node;
//		p->ch = itr->first;
//		p->weight = itr->second;
//		t.push_back(p);
//	}
//
//
//	//////  создаем дерево		
//
//	while (t.size() != 1)
//	{
//		t.sort(MyCompare());
//
//		Node* SonL = t.front();
//		t.pop_front();
//		Node* SonR = t.front();
//		t.pop_front();
//
//		Node* parent = new Node(SonL, SonR);
//		t.push_back(parent);
//	}
//
//	Node* root = t.front();   //root - указатель на вершину дерева
//
//	////// создаем пары 'символ-код':			
//
//	BuildTable(root);
//
//	////// Выводим коды в файл output.txt
//
//	f.clear(); f.seekg(0); // перемещаем указатель снова в начало файла
//
//	ofstream g("output.txt", ios::out | ios::binary);
//
//	int count = 0; char buf = 0;
//
//	while (!f.eof())
//	{
//		char c = f.get();
//		vector<bool> x = table[c];
//		for (int n = 0; n < x.size(); n++)
//		{
//			buf = buf | x[n] << (7 - count);
//			count++;
//			if (count == 8)
//			{
//				count = 0;
//				g << buf; buf = 0;
//			}
//		}
//	}
//
//	f.close();
//	g.close();
//
//	///// считывание из файла output.txt и преобразование обратно
//
//	ifstream F("output.txt", ios::in | ios::binary);
//
//	setlocale(LC_ALL, "Russian"); // чтоб русские символы отображались в командной строке
//
//	Node* p = root;
//	count = 0; char byte;
//	byte = F.get();
//	while (!F.eof())
//	{
//		bool b = byte & (1 << (7 - count));
//		if (b) p = p->right; else p = p->left;
//		if (p->left == NULL && p->right == NULL) { cout << p->ch; p = root; }
//		count++;
//		if (count == 8) { count = 0; byte = F.get(); }
//	}
//
//	F.close();
//
//	return 0;
//}