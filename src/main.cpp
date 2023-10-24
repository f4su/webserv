#include <iostream>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "error arguments\n";
		return (1);
	}
	int	serv_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (serv_socket == -1)
	{
		std::cout << "error in socket\n";
		return (1);
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[1]));
	address.sin_addr.s_addr = INADDR_ANY;
	if (bind(serv_socket, (const sockaddr *)&address, sizeof(address)) == -1)
	{

		std::cout << "error in bind\n";
		return (1);
	}
	if (listen(serv_socket, 20) == -1)
	{

		std::cout << "error in listen\n";
		return (1);
	}
	fd_set readfds;
	int	fdMax = serv_socket;
	FD_ZERO(&readfds);
	FD_SET(fdMax, &readfds);
	fd_set	fdNow;
	while (1)
	{
		fdNow = readfds;
		if (select(fdMax + 1, &fdNow, NULL, NULL, NULL) == -1)
		{
			std::cout << "select in accept\n";
			continue ;
		}
		std::cout << "PASSED SELECT"<< std::endl;
		for (int i = 0; i <= fdMax; i++)
		{
			std::cout << "--------------------------------\n" << "I: " << i << std::endl << "SERV_SOCKET: " << serv_socket << std::endl << std::endl;
			if (!FD_ISSET(i, &fdNow))
				continue ;
			std::cout << "PASSED ISSET\n";
			if (i == serv_socket)
			{
				socklen_t address_len = sizeof(address);
				int client = accept(serv_socket, (sockaddr *)&address, &address_len);
				std::cout << "CLIENT: " << client << std::endl;
				if (client == -1)
				{
					std::cout << "error in accept\n";
					continue ;
				}
				fdMax = fdMax > client ? fdMax : client;
				std::cout << "FDMAX: " << fdMax << std::endl;
				FD_SET(client, &readfds);
				break;
			}
			char r_buff[1024];
			int	readed = recv(i, r_buff, 1024, 0);
			std::cout << "READED: " << readed << std::endl;
			if (readed == -1)
			{
				std::cout << "error in recv\n";
				continue ;
			}
			std::string s_buff = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: 13\r\n\n<h1>Hola</h1>";
			if (send(i, s_buff.c_str(), strlen(s_buff.c_str()), 0) == -1)
			{
				std::cout << "error in send\n";
				continue ;
			}
			std::cout << "connected to client\n";
			std::cout << "ERRNO: " << errno << std::endl << "FDMAX: " << fdMax << std::endl << "--------------------------------\n";
		}
	}
	return (0);
}