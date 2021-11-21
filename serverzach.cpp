#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <conio.h>
#include <signal.h>
#include <thread>
#include <process.h>
#include <mutex>



#define PORT 8080 
#pragma comment(lib, "ws2_32.lib")

#define MIN 0
#define MAX 10000

using namespace std;

int Rand(int min, int max)
{
	return min + (std::rand() % (max - min + 1));
}

volatile int randomValue;
bool hasWinner = false;

std::mutex mtLock;

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

unsigned int __stdcall ClientSession(void* m_socket) {
	while (1)
	{
		char recvbuf[sizeof(Command)];
		int bytesRecv = recv((SOCKET)m_socket, recvbuf, sizeof(Command), 0);
		if (bytesRecv > 0)
		{
			Command* cmd = (Command*)recvbuf;

			int answer;

			if (cmd->value == randomValue)
				answer = EQUAL;
			else if (cmd->value > randomValue)
				answer = MORE;
			else if (cmd->value < randomValue)
				answer = LESS;

			send((SOCKET)m_socket, (char*)&answer, sizeof(int), 0);

			if (answer == EQUAL)
			{
				mtLock.lock();
				if (!hasWinner)
				{
					hasWinner = true;
					cout << "We have a winner, client = " << cmd->number;
				}
				mtLock.unlock();

				return 0;
			}
		}
	}
}

int main(int argc, char const* argv[])
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error \n");

	SOCKET m_socket;
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//trying to creat a socket
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
	}

	
	struct sockaddr_in sock_info;
	sock_info.sin_family = AF_INET;
	sock_info.sin_addr.s_addr = INADDR_ANY;
	sock_info.sin_port = htons(PORT);

	srand(std::time(0));
	randomValue = Rand(MIN, MAX); //min max params
	cout << randomValue << endl;

	if (bind(m_socket, (SOCKADDR*)&sock_info, sizeof(sock_info)) == SOCKET_ERROR)
	{
		printf("bind() failed.\n");
		closesocket(m_socket);
	}

	if (listen(m_socket, 1) == SOCKET_ERROR)
		printf("Error listening on socket.\n");

	SOCKET client_socket;
	while ((client_socket = accept(m_socket, NULL, NULL))) {
		// Create a new thread for the accepted client (also pass the accepted client socket).
		unsigned threadID;
		_beginthreadex(NULL, 0, &ClientSession, (void*)client_socket, 0, &threadID);
	}

	closesocket(m_socket);

	_getch();
	return 0;
}