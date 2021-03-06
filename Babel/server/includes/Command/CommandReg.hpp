#pragma once

#include "ICommand.hpp"

class CommandReg : public ICommand, private boost::noncopyable {

    // default ctor-dtor
    public:
        CommandReg() { }
        virtual ~CommandReg() { }

    // private coplien form
    private:
        CommandReg(const CommandReg &) = delete;
        const CommandReg & operator = (const CommandReg &) = delete;

	//body
    #pragma pack(push, 1)
	struct Body{
		char				accountName[256];
		char				pseudo[256];
		char				password[256];
	};
    #pragma pack(pop)

	//heritage from ICommand
	std::vector<std::string>	*getParam(IClientSocket *socket);
	IClientSocket::Message		*setParam(const std::vector<std::string> &param);
    unsigned int				getSizeBody(void);
};