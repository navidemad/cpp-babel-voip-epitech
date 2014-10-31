#pragma once

#include "ICommand.hpp"

class CommandShow : public ICommand{
public:
	CommandShow();
	~CommandShow();

	//body
	struct __attribute__ ((packed)) Body{
		char				accountName[256];
	};

	struct __attribute__ ((packed)) BodySend{
		ICommand::Header	header;
		char				accountName[256];
		char				pseudo[256];
		char				status;
		char				isConnected;
	};

	//heritage from ICommand
	std::vector<std::string>	*getParam(IClientSocket *socket);
	IClientSocket::Message		*setParam(std::vector<std::string> *param);
    unsigned int				getSizeBody(void);
};