#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <iomanip>

#pragma comment(lib, "Ws2_32.lib")

DWORD WINAPI thread_func(LPVOID context);

struct Data
{
	SOCKET client_socket;
};

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
//struct MyCompare
//{
//	bool operator()(const Node* l, const Node* r) const
//	{
//		return l->weight < r->weight;
//	}
//};
//
//void BuildTable(Node* root, std::map<char, std::vector<bool>> table, std::vector<bool> code)
//{
//	if (root->left != NULL)
//	{
//		code.push_back(0);
//		BuildTable(root->left, table, code);
//	}
//
//	if (root->right != NULL)
//	{
//		code.push_back(1);
//		BuildTable(root->right, table, code);
//	}
//
//	if (root->left == NULL && root->right == NULL)
//	{
//		table[root->ch] = code;
//	}
//
//	code.pop_back();
//}

const char SERVER_IP[] = "127.0.0.1";
const int PORT = 8000;

int main()
{
	in_addr server_in_addr;

	if (inet_pton(AF_INET, SERVER_IP, &server_in_addr) <= 0)
	{
		std::cout << "Error in IP translation to special numeric format" << std::endl;

		return 1;
	}

	// WinSock initialization
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "Error WinSock version initializaion #" << WSAGetLastError() << std::endl;

		return 1;
	}

	std::cout << "WinSock initialization is OK" << std::endl;

	// Server socket initialization
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket == INVALID_SOCKET)
	{
		std::cout << "Error initialization socket #" << WSAGetLastError() << std::endl;

		closesocket(server_socket);
		WSACleanup();

		return 1;
	}

	std::cout << "Server socket initialization is OK" << std::endl;

	// Server socket binding
	sockaddr_in server_sockaddr_in;

	// Initializing servInfo structure
	ZeroMemory(&server_sockaddr_in, sizeof(server_sockaddr_in));

	server_sockaddr_in.sin_family = AF_INET;
	server_sockaddr_in.sin_addr = server_in_addr;
	server_sockaddr_in.sin_port = htons(PORT);

	if (bind(server_socket, (sockaddr*)&server_sockaddr_in, sizeof(server_sockaddr_in)) != 0)
	{
		std::cout << "Error Socket binding to server info. Error #" << WSAGetLastError() << std::endl;

		closesocket(server_socket);
		WSACleanup();

		return 1;
	}

	std::cout << "Binding socket to Server info is OK" << std::endl;

	//Starting to listen to any Clients

	if (listen(server_socket, SOMAXCONN) != 0)
	{
		std::cout << "Can't start to listen to. Error #" << WSAGetLastError() << std::endl;

		closesocket(server_socket);
		WSACleanup();

		return 1;
	}

	std::cout << "Listening..." << std::endl;

	while (true)
	{
		int _ = sizeof(sockaddr_in);

		sockaddr_in client_sockaddr_in;

		ZeroMemory(&client_sockaddr_in, sizeof(sockaddr_in));

		SOCKET client_socket = accept(server_socket, (sockaddr*)&client_sockaddr_in, &_);

		if (client_socket == INVALID_SOCKET)
		{
			std::cout << "Client detected, but can't connect to a client. Error #" << WSAGetLastError() << std::endl;

			closesocket(client_socket);

			closesocket(server_socket);
			WSACleanup();

			return 1;
		}

		Data data;

		data.client_socket = client_socket;

		QueueUserWorkItem(thread_func, (LPVOID)&data, WT_EXECUTEDEFAULT);
	}

	closesocket(server_socket);
	WSACleanup();

	return 0;
}

DWORD WINAPI thread_func(LPVOID context)
{
	Data* data = (Data*)context;

	int matrix_1_row_count;
	int matrix_1_column_count;
	int matrix_2_row_count;
	int matrix_2_column_count;

	recv(data->client_socket, (char*)&matrix_1_row_count, sizeof(int), 0);
	recv(data->client_socket, (char*)&matrix_1_column_count, sizeof(int), 0);
	recv(data->client_socket, (char*)&matrix_2_row_count, sizeof(int), 0);
	recv(data->client_socket, (char*)&matrix_2_column_count, sizeof(int), 0);

	int matrix_1_length = matrix_1_row_count * matrix_1_column_count;
	int matrix_2_length = matrix_2_row_count * matrix_2_column_count;

	float* matrix_1 = new float[matrix_1_length];
	float* matrix_2 = new float[matrix_2_length];

	recv(data->client_socket, (char*)matrix_1, matrix_1_length * sizeof(float), 0);
	recv(data->client_socket, (char*)matrix_2, matrix_2_length * sizeof(float), 0);

	float* result_matrix = new float[matrix_1_row_count * matrix_2_column_count];

	for (int row_index = 0; row_index < matrix_1_row_count; ++row_index)
	{
		for (int column_index = 0; column_index < matrix_2_column_count; ++column_index)
		{
			float value = 0;

			for (int i = 0; i < matrix_1_column_count; ++i)
			{
				value += matrix_1[row_index * matrix_1_column_count + i] * matrix_2[i * matrix_2_column_count + column_index];
			}

			result_matrix[row_index * matrix_2_column_count + column_index] = value;
		}
	}

	send(data->client_socket, (char*)result_matrix, matrix_1_row_count * matrix_2_column_count * sizeof(float), 0);

	return 0;
}

//DWORD WINAPI thread_func(PVOID param)
//{
//	Data* data = (Data*)param;
//
//	bool is_compress;
//
//	while (true)
//	{
//		if (recv(data->client_socket, (char*)&is_compress, sizeof(bool), 0) == INVALID_SOCKET)
//		{
//			closesocket(data->client_socket);
//
//			return 0;
//		}
//
//		int message_length;
//
//		recv(data->client_socket, (char*)&message_length, sizeof(int), 0);
//
//
//
//		char* buffer = new char[message_length];
//
//		recv(data->client_socket, buffer, message_length * sizeof(char), 0);
//
//		if (is_compress)
//		{
//			compress();
//		}
//		else
//		{
//			decompress();
//		}
//	}
//}
//
//char* compress(char* message, int len)
//{
//	char* compressed_message = new char[len];
//
//	std::map<char, int> frequency;
//
//	for (int i = 0; i < len; ++i)
//	{
//		frequency[message[i]]++;
//	}
//
//	std::list<Node*> nodes;
//
//	for (auto itr = frequency.begin(); itr != frequency.end(); ++itr)
//	{
//		Node* p = new Node;
//
//		p->ch = itr->first;
//		p->weight = itr->second;
//
//		nodes.push_back(p);
//	}
//
//	while (nodes.size() != 1)
//	{
//		nodes.sort(MyCompare());
//
//		Node* SonL = nodes.front();
//		nodes.pop_front();
//
//		Node* SonR = nodes.front();
//		nodes.pop_front();
//
//		Node* parent = new Node(SonL, SonR);
//		nodes.push_back(parent);
//	}
//
//	Node* root = nodes.front();
//
//	std::map<char, std::vector<bool>> table;
//	std::vector<bool> code;
//
//	BuildTable(root, table, code);
//
//	int count = 0;
//	char buf = 0;
//
//	for (int i = 0; i < len; ++i)
//	{
//		std::vector<bool> x = table[message[i]];
//
//		for (int n = 0; n < x.size(); ++n)
//		{
//			buf = buf | x[n] << (7 - count);
//
//			count++;
//
//			if (count == 8)
//			{
//				count = 0;
//				compressed_message[i] = buf;
//				buf = 0;
//			}
//		}
//	}
//
//	return compressed_message;
//}
//
//char* decompress(char* message, int len)
//{
//	char* decompressed_message = new char[len];
//
//	Node* p = root;
//
//	int count = 0;
//	char byte;
//
//	for (int i = 0; i < len; ++i)
//	{
//		byte = message[i];
//
//		bool b = byte & (1 << (7 - count));
//
//		if (b)
//		{
//			p = p->right;
//		}
//		else
//		{
//			p = p->left;
//		}
//
//		if (p->left == NULL && p->right == NULL)
//		{
//			decompressed_message[i] = p->ch;
//			p = root;
//		}
//
//		count++;
//
//		if (count == 8)
//		{
//			count = 0;
//			byte = 
//		}
//	}
//
//	byte = F.get();
//	while (!F.eof())
//	{
//		bool b = byte & (1 << (7 - count));
//		if (b) p = p->right; else p = p->left;
//		if (p->left == NULL && p->right == NULL) { cout << p->ch; p = root; }
//		count++;
//		if (count == 8) { count = 0; byte = F.get(); }
//	}
//}