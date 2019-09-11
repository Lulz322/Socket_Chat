#ifndef USERS_HPP
#define USERS_HPP
#include <vector>
#include <string>


class Users
{
public:
	Users();
	Users(std::string name, int socket);
	~Users();


	int 		id;
	std::string _name;
};

#endif
