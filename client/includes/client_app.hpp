#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <thread>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define READ(fd, buff, size) {if (read(sockfd, buff, sizeof(buff)) == 0){std::cout << "Lost connection w/ Server\n";exit(EXIT_FAILURE);}}
#define SA struct sockaddr

class Client_app{
public:
	Client_app();
	~Client_app();

	Client_app&operator=(const Client_app& rhs);

	void start_app();
	void welcome_window();
	void register_window();
	void log_in();
	void main_menu();
	void set_id();
	void reader();

private:
	int sockfd;
	struct sockaddr_in servaddr;
	std::string output;
};
