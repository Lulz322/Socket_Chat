#include "../includes/client_app.hpp"
#include <algorithm>
//also check drop connection
#define READ(fd, buff, size) {if (read(sockfd, buff, sizeof(buff)) == 0){std::cout << "Lost connection w/ Server\n";exit(EXIT_FAILURE);}}
#define BUFFSIZE 1024
Client_app::Client_app()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");
}

Client_app::~Client_app()
{
	close(sockfd);
}

Client_app& Client_app::operator=(const Client_app& rhs){
	if (this == &rhs)
		return (*this);
	else
		return (*(new Client_app()));
}

void Client_app::log_in(){
	bool success = true;
	char buff[BUFFSIZE];

	std::cout << "Log_in Window\n\n";
	while (success) {
		std::cout << "Login:" << std::endl;
		output.clear();
		while (output.empty())
   			getline(std::cin, output);
		write(sockfd, output.c_str(), output.length());

		std::cout << "Password:" << std::endl;

		output.clear();
		while (output.empty())
   			getline(std::cin, output);
		write(sockfd, output.c_str(), output.length());


		memset(buff, '\0', BUFFSIZE);
		READ(sockfd, buff, BUFFSIZE);

		if (atoi(&buff[1]) == 1)
		{
			memset(buff, '\0', BUFFSIZE);
			READ(sockfd, buff, BUFFSIZE);
			if (atoi(&buff[1]) == 0)
			{
				std::cout << "This user already login" << std::endl;
				close(sockfd);
				exit (EXIT_FAILURE);
			}
			memset(buff, '\0', BUFFSIZE);
			READ(sockfd, buff, BUFFSIZE);
			system("clear");
			std::cout << "Successfully logged in\n";
			std::cout << "CTRL + C for exit\n";
			success = false;
		}
		else
		{
			std::cout << "Error, Login or Password incorrect\n\n";
		 	welcome_window();
	 }

	}
	main_menu();
}

void Client_app::register_window() {
	bool success = true;
	char buff[BUFFSIZE];

	std::cout << "Register Window\n\n";
	while (success)
	{
		while (1)
		{
			output.clear();
			std::cout << "Login:" << std::endl;
	   		getline(std::cin, output);
			if (output.empty())
				std::cout << "Name cannot be empty\n";
			else if (std::count(output.begin(), output.end(), ':') != 0)
				std::cout << "Forbidden characther ':'\n";
			else
				break;
		}
		write(sockfd, output.c_str(), output.length());
		memset(buff, '\0', BUFFSIZE);
		READ(sockfd, buff, BUFFSIZE);
		if (atoi(&buff[1]) == 1)
		{
			std::cout << "Password:" << std::endl;
			memset(buff, '\0', BUFFSIZE);
			while (1)
			{
				std::cout << buff << std::endl;
				output.clear();
		   		getline(std::cin, output);
				if (output.empty())
					std::cout << "Password cannot be empty\n";
				else
					break;
			}
			write(sockfd, output.c_str(), output.length());

			memset(buff, '\0', BUFFSIZE);
			READ(sockfd, buff, BUFFSIZE);
			std::cout << "Successfully accout created\n";
			success = false;
			break;
		}
		else
			std::cout << "Error, this login already exist choose another\n";
	}
	log_in();
}

void Client_app::reader()
{
	int activity;
	char buff[BUFFSIZE];

	memset(buff, '\0', BUFFSIZE);
	for (;;)
	{
		while ((activity = read(sockfd , buff, BUFFSIZE)) != 0)
		{
			std::cout << buff << std::endl;
			memset(buff, '\0', BUFFSIZE);
		}
	}
}

void Client_app::main_menu()
{
	std::thread readers([&]()
	{
		reader();
	});
	readers.detach();

for (;;) {
		while (1)
		{
			output.clear();
			while (output.empty())
	   			getline(std::cin, output);
			write(sockfd, output.c_str(), output.length());
		}
	}
}

void Client_app::welcome_window()
{
	char buff[BUFFSIZE];
	std::cout << "Welcome in App Client\n";
	while (1) {
		std::cout  <<	"1. For login\n"
				   <<	"2. Register account\n";

		output.clear();
		while (output.empty())
			getline(std::cin, output);
		write(sockfd, output.c_str(), output.length());
		memset(buff, '\0', BUFFSIZE);
		READ(sockfd, buff, BUFFSIZE);
		if (atoi(&buff[1]) == 1002)
			log_in();
		else if (atoi(&buff[1]) == 1001)
			register_window();
		else
			std::cout << "Error input\n";
	}
}

void Client_app::start_app()
{
	char buff[BUFFSIZE];

	memset(buff, '\0', BUFFSIZE);
	READ(sockfd, buff, BUFFSIZE);
	std::cout << buff;

	while (1)
	{
		output.clear();
		while (output.empty())
			getline(std::cin, output);

		write(sockfd, output.c_str(), output.length());

		memset(buff, '\0', BUFFSIZE);
		READ(sockfd, buff, BUFFSIZE);
		if (atoi(&buff[1]) == 1001)
			register_window();
		else if (atoi(&buff[1]) == 1002)
			log_in();
		else
			std::cout << "error input\n";
	}
}
//best main ever;
int main(void)
{
	Client_app app;

	app.start_app();
}
