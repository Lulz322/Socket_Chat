#include "client_app.hpp"
#include <thread>

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


void Client_app::log_in(){
	bool success = true;

	while (success) {
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		std::cout << buff << std::endl;
		getline(std::cin, output);
		write(sockfd, output.c_str(), sizeof(output.c_str()));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		std::cout << buff << std::endl;
		getline(std::cin, output);
		write(sockfd, output.c_str(), sizeof(output.c_str()));


		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		if (atoi(&buff[1]) == 1)
		{
			system("clear");
			std::cout << "Successfully logged in\n";
			success = false;
		}
		else
			std::cout << "Error, Login or Password incorrect\n";
	}
	main_menu();
}

void Client_app::register_window() {
	bool success = true;
	while (success)
	{
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));

		while (1)
		{
			std::cout << buff << std::endl;
			getline(std::cin, output);
			if (output.empty())
				std::cout << "Name cannot be empty\n";
			else
				break;
		}

		write(sockfd, output.c_str(), sizeof(output.c_str()));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		if (atoi(&buff[1]) == 1)
		{
			bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));

			while (1)
			{
				std::cout << buff << std::endl;
				getline(std::cin, output);
				if (output.empty())
					std::cout << "Password cannot be empty\n";
				else
					break;
			}
			write(sockfd, output.c_str(), sizeof(output.c_str()));
			bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));

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
	for (;;)
	{
		bzero(buff, 1024);
		read(sockfd, buff, 1024);
		std::cout << buff << std::endl;
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
		getline(std::cin, output);
		write(sockfd, output.c_str(), output.length());
	}
	}
}

void Client_app::start_app()
{
	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff));
	std::cout << buff;

	getline(std::cin, output);
	write(sockfd, output.c_str(), sizeof(output.c_str()));

	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff));
	if (atoi(&buff[1]) == 1001)
		register_window();
	else if (atoi(&buff[1]) == 1002)
		log_in();
	else
		std::cout << "error\n";
}
