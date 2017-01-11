#include <winsock2.h>
#include <windows.h>
#include <iostream>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed.\n";
		system("pause");
		return 1;
	}


	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	struct hostent *host; 

	char homepage[200]; 
	cout << "원하는 URL 입력 = ";
	cin >> homepage;

	if ((host = gethostbyname(homepage)) == 0) 
	{
		cout << "Could not connect";
		system("pause");
		return 1;
	}


	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);


	cout << "Connecting...\n";
	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0)
	{
		cout << "Could not connect";
		system("pause");
		return 1;
	}
	cout << "Connected.\n";



	char regi[100];
	sprintf(regi, "GET / HTTP/1.1\r\nHost:%s\r\nConnection: close\r\n\r\n", homepage);
	send(Socket, regi, strlen(regi), 0);
	cout << regi;





	char buffer[20000];
	int nDataLength;
	while ((nDataLength = recv(Socket, buffer, 20000, 0)) > 0) {
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
			cout << buffer[i];
			i += 1;
		}
	}
	closesocket(Socket);
	WSACleanup();
	system("pause");
	return 0;
}


//http://nine01223.tistory.com/270 Client 코드참조
//http://ryumin13.tistory.com/entry/sprintf-%EC%82%AC%EC%9A%A9%ED%95%98%EA%B8%B0 //sprintf 참조
