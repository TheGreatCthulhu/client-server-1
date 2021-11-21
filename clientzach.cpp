// clientzach.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <winsock2.h>
#include <thread>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <string>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define MIN 0
#define MAX 10000

using namespace std;

int Rand(int min, int max)
{
	return min + (std::rand() % (max - min + 1));
}

struct Command
{
	int number; //client number
	int value; //his number to guess
};

enum EqualityType
{
	LESS,
	EQUAL,
	MORE
};

volatile int currentValue;
int clientNumber;

void sent(SOCKET m_socket, int value) {

	Command cmd;
	cmd.number = clientNumber;

	cmd.value = value;

	send(m_socket, (char*)&cmd, sizeof(Command), 0);
}

void Session(SOCKET m_socket)
{
	int answer = 0;

  //Command answer;

	int leftb = MIN;
	int rightb = MAX;

	while (1)
	{
		sent(m_socket, currentValue);

		while (1)
		{
			/**/
			
			char recvbuf[sizeof(int)] = "";
			int bytesRecv = recv(m_socket, recvbuf, sizeof(int), 0);
			if (bytesRecv > 0)
			{
				answer = *(int*)recvbuf;
				break;
			}

		}

		if (answer == EQUAL)
		{
			break;
			
		}

		if (answer == LESS)//less
		{
			leftb = currentValue;
			currentValue = (leftb + rightb) / 2;

		}
		else //more
		{
			rightb = currentValue;
			currentValue = (leftb + rightb) / 2;
		}

		Sleep(200);

		cout << currentValue << endl;
	}
}

int main()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2),
		&wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	SOCKET m_socket;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
	}
	else
	{
		int s = socket(PF_INET, SOCK_STREAM, 0);
		struct sockaddr_in clientService;
		clientService.sin_family = AF_INET;//con det
		clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
		clientService.sin_port = htons(8080);

		cout << "Enter client number" << endl;
		cin >> clientNumber;

		srand(std::time(0));
		currentValue = Rand(MIN, MAX);

		if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
			printf("Failed to connect.\n");
			WSACleanup();
		}

		std::thread th1(&Session, m_socket);

		th1.join();

		closesocket(m_socket);
	}
	_getch();
	return 0;
}
