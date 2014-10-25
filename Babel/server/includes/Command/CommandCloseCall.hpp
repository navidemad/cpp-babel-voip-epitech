#pragma once

#include "ICommand.hpp"

class CommandCloseCall : public ICommand{
public:
	CommandCloseCall();
	~CommandCloseCall();

	//body
	struct Body{
		char	accountName[256];
	};

	//heritage from ICommand
	std::vector<std::string>	*getParam(IClientSocket *socket);
	IClientSocket::Message		*setParam(std::vector<std::string> *param);
	int							getSizeBody(void);
};