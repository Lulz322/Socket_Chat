#include <netdb.h>
#include <string.h>
#include <string>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include "server.hpp"


#define MAX 80
#define PORT 8080

#define READ(fd, buff, size) {if (read(fd, buff,size) == 0) {std::cout << "Lost connection w/ user\n"; close(fd);return ;}}

Users::Users() {}
Users::Users(std::string name, int socket) : id(socket), _name(name)
{

}
Users::~Users() { }


Server::Server() {
	check = false;
	FD_ZERO(&readfds);
	connected.clear();
	a_users = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("=> Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));


	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);


	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Socket bind failed...\n");
		exit(0);
	}
	else
		printf("=> Socket successfully binded..\n");

	listen(sockfd, 5);

	len = sizeof(cli);
	FD_SET(0, &readfds);
	a_users++;
}

Server::~Server(){
	close(sockfd);
}
int log_pas(std::string str)
{
	std::string line;
	std::ifstream in("db");
	std::string tmp;
	int i;

	if (in.is_open())
	{
		while (getline(in, line))
		{
			i = -1;
			tmp.clear();
			while (line[++i] != ':')
				tmp += line[i];
			if (str == tmp)
				return (0);
		}
	}
	in.close();
	return (1);
}

int log_in(std::string name, std::string pass)
{
	std::string line;
	std::ifstream in("db");
	std::string tmp;
	int i;

	if (in.is_open())
	{
		while (getline(in, line))
		{
			i = -1;
			tmp.clear();
			while (line[++i] != ':')
				tmp += line[i];
			if (name == tmp && pass == &line[i + 1])
				return (1);
		}
	}
	in.close();
	return (0);
}


//Adding client to "db" :D
void Server::add_client(std::string name, std::string pass)
{
	std::ofstream out;
	out.open("db", std::ios::app);
	out << name << ":" << pass << std::endl;
}

void Server::log_in_client(int sock)
{
	char buff[1024];
	std::string name;
	std::string pass;

	write(sock, "[1002]", sizeof("[1002]"));
	while (1)
	{
		write(sock, "Login:\n", sizeof("Login:\n"));
		READ(sock, buff, 1024);
		name = buff;
		bzero(buff, 1024);
		write(sock, "Password:\n", sizeof("Password:\n"));
		READ(sock, buff, 1024);
		pass = buff;
		if (log_in(name, pass))
		{
			write(sock, "[1]\n",sizeof("[1]\n"));

			for (std::vector<Users *>::iterator it = connected.begin();
			 it != connected.end(); ++it) { // Looking if someone in system w/ that nickname
				if ((*it)->_name == name)
				{
					write(sock, "[0]\n",sizeof("[0]\n"));
					std::cout << sock << " has been kicked, for duplicate" << std::endl;
					close(sock);
					return ;
				}
			}
			write(sock, "[1]\n",sizeof("[1]\n"));
			connected.push_back(new Users(name, sock));
			FD_SET(sock, &readfds);
			a_users++;
			check = true;
			write(sock, "[1]\n",sizeof("[1]\n"));
			break;
		}
		else
		{
			write(sock, "[0]\n",sizeof("[0]\n"));
			welcome_window(sock);
		}
	}
}

void Server::register_client(int sock){

	char buff[1024];
	std::string name;
	std::string pass;
	bool success = true;


	write(sock, "[1001]", sizeof("[1001]"));
	while (success)
	{
		bzero(buff, sizeof(buff));
		write(sock, "Login:\n", sizeof("Login:\n"));
		READ(sock, buff, 1024);
		name = buff;
		if (log_pas(name))
		{
			write(sock, "[1]", sizeof("[1]"));
			bzero(buff, 1024);
			write(sock, "Password:\n", sizeof("Password:\n"));
			READ(sock, buff, 1024);
			pass = buff;
			add_client(name, pass);
			success = false;
			break;
		}
		else
			write(sock, "[0]", sizeof("[0]"));
	}
	log_in_client(sock);
}

void Server::send_to_other(Users *user, std::string str){
	for (std::vector<Users *>::iterator it = connected.begin();
	 it != connected.end(); ++it) {
		if (*it != user)
			write((*it)->id, str.c_str(), str.length());
	}
}

void Server::check_user(Users *user)
{
	int activity;
	char buffer[1024];
	std::string tmp;
	std::vector<Users *>::iterator it;

	bzero(buffer, 1024);
	for (;;)
	{
		while ((activity = read(user->id , buffer, 1024)) != 0)
		{
			tmp.clear();
			tmp += "<@" + user->_name + "> ";
			tmp += buffer;
			std::cout << tmp << std::endl;
			send_to_other(user, tmp);
			bzero(buffer, 1024);
		}
		for (it = connected.begin(); *it != user; ++it) {
		}
		connected.erase(it);
		std::cout << user->_name <<" left this channel" << std::endl;
		delete (user);
		return ;
	}
}

void Server::start_server()
{
	Users *tmp;
	std::vector<Users *>::iterator it;

	std::vector<Users *>::iterator start = connected.begin();
	std::vector<Users *>::iterator end = connected.end();

	std::string tmp_text;

	for(;;)
	{
		if (check == true)
		{
			start = connected.begin();
			end 	= connected.end();
			std::cout << "users on channel: \n";
			for (it = start; it != end; ++it)
			{
				tmp = *it;
				std::cout << "[" << (*it)->id << "] " << (*it)->_name<< std::endl;
			}
			tmp_text.clear();
			tmp_text = tmp->_name + " Joined this channel";
			send_to_other(0, tmp_text);
			std::thread test([&]()
			{
				check_user(tmp);
			});
			test.detach();
			check = false;
		}
		else
		{

		}
	}
}


void Server::welcome_window(int connfd)
{
	char buff[1024];


	READ(connfd, buff, 1024);

	while (1)
	{
		if (buff[0] == '2' && buff[1] == '\0')
			 { register_client(connfd); break;}
		else if (buff[0] == '1' && buff[1] == '\0')
			{ log_in_client(connfd); break;}
		else
			write(connfd, "[0]", sizeof("[0]"));
	}
}

void Server::check_people()
{
	int connfd;

	while (1)
	{

		connfd = accept(sockfd, (SA*)&cli, &len);
		std::cout << "Someone connected on [" << connfd << "] fd" << std::endl;
		write(connfd, "1.For Login\n2.Register\n", sizeof("1.For Login\n2.Register\n"));
		welcome_window(connfd);
		if (connfd < 0) {
			printf("server acccept failed...\n");
			exit(0);
		}
	}
}

int main()
{

	Server obs;
	std::thread peoples([&]()
	{
		for (;;)
		{
			obs.check_people();
			obs.check_people();
		}
	});
	peoples.detach();


	obs.start_server();
}
