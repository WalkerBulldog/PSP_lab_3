#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>
#include <iomanip>

#pragma comment(lib, "Ws2_32.lib")

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
	WSADATA wsData;

	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
	{
		std::cout << "Error WinSock version initializaion #" << WSAGetLastError() << std::endl;

		return 1;
	}

	std::cout << "WinSock initialization is OK" << std::endl;

	// Socket initialization
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (client_socket == INVALID_SOCKET)
	{
		std::cout << "Error initialization socket #" << WSAGetLastError() << std::endl;

		closesocket(client_socket);
		WSACleanup();

		return 1;
	}

	std::cout << "Client socket initialization is OK" << std::endl;

	// Establishing a connection to Server
	sockaddr_in server_sockddr_in;

	ZeroMemory(&server_sockddr_in, sizeof(sockaddr_in));

	server_sockddr_in.sin_family = AF_INET;
	server_sockddr_in.sin_addr = server_in_addr;
	server_sockddr_in.sin_port = htons(PORT);

	if (connect(client_socket, (sockaddr*)&server_sockddr_in, sizeof(sockaddr_in)) != 0)
	{
		std::cout << "Connection to Server is FAILED. Error #" << WSAGetLastError() << std::endl;

		closesocket(client_socket);
		WSACleanup();

		return 1;
	}

	std::cout << "Connection established SUCCESSFULLY." << std::endl;

	float* matrix_1 = new float[3 * 4];
	float* matrix_2 = new float[4 * 5];

	matrix_1[0] = 1;
	matrix_1[1] = 2;
	matrix_1[2] = 3;
	matrix_1[3] = 4;
	matrix_1[4] = 5;
	matrix_1[5] = 6;
	matrix_1[6] = 7;
	matrix_1[7] = 8;
	matrix_1[8] = 9;
	matrix_1[9] = 1;
	matrix_1[10] = 2;
	matrix_1[11] = 3;

	matrix_2[0] = 1;
	matrix_2[1] = 2;
	matrix_2[2] = 3;
	matrix_2[3] = 4;
	matrix_2[4] = 5;
	matrix_2[5] = 6;
	matrix_2[6] = 7;
	matrix_2[7] = 8;
	matrix_2[8] = 9;
	matrix_2[9] = 0;
	matrix_2[10] = 1;
	matrix_2[11] = 2;
	matrix_2[12] = 3;
	matrix_2[13] = 5;
	matrix_2[14] = 7;
	matrix_2[15] = 8;
	matrix_2[16] = 6;
	matrix_2[17] = 4;
	matrix_2[18] = 2;
	matrix_2[19] = 5;

	int matrix_1_row_count = 3;
	int matrix_1_column_count = 4;
	int matrix_2_row_count = 4;
	int matrix_2_column_count = 5;

	send(client_socket, (char*)&matrix_1_row_count, sizeof(int), 0);
	send(client_socket, (char*)&matrix_1_column_count, sizeof(int), 0);
	send(client_socket, (char*)&matrix_2_row_count, sizeof(int), 0);
	send(client_socket, (char*)&matrix_2_column_count, sizeof(int), 0);

	send(client_socket, (char*)matrix_1, 12 * sizeof(float), 0);
	send(client_socket, (char*)matrix_2, 20 * sizeof(float), 0);

	float* result_matrix = new float[3 * 5];

	recv(client_socket, (char*)result_matrix, 15 * sizeof(float), 0);

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			std::cout << std::setw(5) << result_matrix[i * 5 + j];
		}
		
		std::cout << std::endl;
	}

	closesocket(client_socket);
	WSACleanup();

	return 0;
}