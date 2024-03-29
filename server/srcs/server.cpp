#include <netdb.h>
#include <string.h>
#include <string>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include "../includes/server.hpp"

#define PORT 8080
#define BUFFSIZE 1024
//also check drop connection
#define READ(fd, buff, size) {if (read(fd, buff,size) == 0) {std::cout << "Lost connection w/ user\n"; close(fd);return ;}}

Users::Users() {}
Users::Users(std::string name, int socket) : id(socket), _name(name)
{

}
Users::~Users() { }


Server&Server::operator=(const Server& rhs){
	if (this == &rhs)
		return (*this);
	else
	 	return (*(new Server()));
}

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
}

Server::~Server(){
	close(sockfd);
}
static bool log_pas(std::string str)
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
				return (false);
		}
	}
	in.close();
	return (true);
}

static bool log_in(const std::string name, const std::string pass)
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
				return (true);
		}
	}
	in.close();
	return (false);
}


//Adding client to "db" :)
void Server::add_client(std::string name, std::string pass)
{
	std::ofstream out;
	out.open("db", std::ios::app);
	out << name << ":" << pass << std::endl;
}

void Server::log_in_client(int sock)
{
	char buff[BUFFSIZE];
	std::string name;
	std::string pass;
	bool tick = true;

	write(sock, "[1002]", sizeof("[1002]")); //ID log_in room on client side
	while (tick)
	{
		bzero(buff, BUFFSIZE);
		READ(sock, buff, BUFFSIZE);
		name = buff;


		bzero(buff, BUFFSIZE);
		READ(sock, buff, BUFFSIZE);
		pass = buff;


		if (log_in(name, pass)) //checking in db for that info
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
			connected.push_back(new Users(name, sock)); // Adding user to list
			FD_SET(sock, &readfds);
			a_users++; //number of users
			check = true;
			write(sock, "[1]\n",sizeof("[1]\n"));
			tick = false;
			break;
		}
		else
		{
			write(sock, "[0]\n",sizeof("[0]\n"));
			welcome_window(sock);
			break;
		}
	}
}

void Server::register_client(int sock){

	char buff[BUFFSIZE];
	std::string name;
	std::string pass;
	bool success = true;

	write(sock, "[1001]", sizeof("[1001]")); // ID room in client side
	while (success)
	{
		//name
		memset(buff, '\0', BUFFSIZE);
		READ(sock, buff, BUFFSIZE);
		name = buff;
		if (log_pas(name)) //Check for unique name
		{
			write(sock, "[1]", sizeof("[1]"));
			memset(buff, '\0', BUFFSIZE);
			READ(sock, buff, BUFFSIZE);
			pass = buff;
			add_client(name, pass); //Creating client
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

//This func checking user activity
void Server::check_user(Users *user)
{
	int activity;
	char buffer[BUFFSIZE];
	std::string tmp;
	std::vector<Users *>::iterator it;

	bzero(buffer, BUFFSIZE);
	for (;;)
	{
		while ((activity = read(user->id , buffer, BUFFSIZE)) != 0)
		{
			tmp.clear();
			tmp += "<@" + user->_name + "> ";
			tmp += buffer;
			std::cout << tmp << std::endl;
			send_to_other(user, tmp);
			bzero(buffer, BUFFSIZE);
		}
		for (it = connected.begin(); *it != user; ++it) {
		}
		connected.erase(it);
		a_users--;
		tmp = user->_name + " left this channel";
		std::cout << tmp << std::endl;
		send_to_other(0, tmp);
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
		if (check == true) // If vector changed list updates and creating thread for new user;
		{
			start = connected.begin();
			end 	= connected.end();
			std::cout << "users on channel ["<< a_users << "]:\n";
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
	}
}

void Server::welcome_window(int connfd)
{
	char buff[BUFFSIZE];

	while (1)
	{
		bzero(buff, BUFFSIZE);
		READ(connfd, buff, BUFFSIZE);
		if (buff[0] != '\0')
		{
			if (buff[0] == '2' && buff[1] == '\0')
				 { register_client(connfd); break;}
			else if (buff[0] == '1' && buff[1] == '\0')
				{ log_in_client(connfd); break;}
			else
				write(connfd, "[0]", sizeof("[0]"));
		}
	}
}

//Thread for connecting users;
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
		}
	});
	peoples.detach();
	std::thread peoples1([&]() //For comfort
	{
		for (;;)
		{
			obs.check_people();
		}
	});
	peoples1.detach();


	obs.start_server(); //Main char/room
}
