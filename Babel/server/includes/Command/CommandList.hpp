#pragma once

#include "ICommand.hpp"

class CommandList : public ICommand{
public:
	CommandList();
	~CommandList();

	//body
#ifdef WIN32
	struct __declspec(align(1)) Body{

	};
#else
	struct __attribute__((packed)) Body{

	};
#endif

	//heritage from ICommand
	std::vector<std::string>	*getParam(IClientSocket *socket);
	IClientSocket::Message		*setParam(std::vector<std::string> *param);
    unsigned int				getSizeBody(void);
};