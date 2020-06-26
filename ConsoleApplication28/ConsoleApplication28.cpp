
///////////////////////////////////////////////               сервер
#include "pch.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable: 4996)
#include <opencv2/opencv.hpp>
#include <opencv/cv.hpp>

#include <opencv2/highgui.hpp>
#include <stdlib.h>

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

SOCKET Connections[100];//100 кол-во подключений
int Counter = 0;

void ClientHandler(int index) {

	int msg_size;
	while (true) {
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		char *msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connections[index], msg, msg_size, NULL);//принимаем сообщением отправленное клиентом 
		for (int i = 0; i < Counter; i++) {
			if (i == index) {
				continue; // чтобы сообщение не дулировалось два раза 
			}

			send(Connections[i], (char*)&msg_size, sizeof(int), NULL); //отправляем размер сообщение всем
			send(Connections[i], msg, msg_size, NULL); //отправляем сообщение всем


		}
		delete[] msg;
	}

}

int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	std::cout << "IP address 127.0.0.1\n";
	addr.sin_port = htons(1111);
	std::cout << "Server port 1111\n ";
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);// SOMAXCONN число подключений

	SOCKET newConnection;
	for (int i = 0; i < 100; i++) {// кол-во подключений
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client #" << i + 1 << " connected\n";


			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}

	}
	system("pause");
	return 0;
}
