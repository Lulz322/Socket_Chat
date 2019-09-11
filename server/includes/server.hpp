#ifndef SERVER_HPP
#define SERVER_HPP
#include "users.hpp"
#include <vector>
#include <string>
#include <unistd.h>
#include <fstream>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iterator>
#define SA struct sockaddr
class Server{
public:

	Server();
	~Server();

	Server&operator=(const Server& rhs);

	void register_client(int sock);
	void log_in_client(int sock);
	void add_client(std::string name, std::string pass);

	void welcome_window(int connfd);
	void check_people();
	void start_server();

	void check_user(Users *user);
	void send_to_other(Users *user, std::string str);



private:
	std::vector<Users *> connected;
	int sockfd;
	int a_users;
	struct sockaddr_in cli, servaddr;
	unsigned int 	   len;
	bool check;
	fd_set readfds;
};

#endif


/*
** 0 - Error
** 1 - Ok
** 1001 - reg window;
** 1002 - log_in window;


*/
