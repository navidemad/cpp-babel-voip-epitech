#include "Command/CommandAcceptAdd.hpp"

std::vector<std::string>		*CommandAcceptAdd::getParam(IClientSocket *socket){

	std::vector<std::string>	*t = new std::vector<std::string>;
    CommandAcceptAdd::Body		*body = new CommandAcceptAdd::Body;
	IClientSocket::Message		data;
	std::string					status = "";

	std::memset(body, 0, this->getSizeBody());
	data = socket->receive(this->getSizeBody());
	body = reinterpret_cast<CommandAcceptAdd::Body*>(data.msg);

	t->push_back(body->accountName);
	status += body->hasAccepted;
	t->push_back(status);

	return t;
}

IClientSocket::Message			*CommandAcceptAdd::setParam(const std::vector<std::string> &param){

	CommandAcceptAdd::BodySend	*bodySend = new CommandAcceptAdd::BodySend;
	IClientSocket::Message		*msg = new IClientSocket::Message;

	std::memset(bodySend, 0, sizeof(CommandAcceptAdd::BodySend));
	bodySend->header.instructionCode = ICommand::ACCEPT_ADD;
	bodySend->header.magicCode = ICommand::MAGIC_CODE;
	std::memcpy(bodySend->accountName, param[0].c_str(), MIN(param[0].size(), sizeof(bodySend->accountName) - 1));

	bodySend->hasAccepted = param[1][0];

	msg->msgSize = sizeof(CommandAcceptAdd::BodySend);
	msg->msg = reinterpret_cast<char *>(bodySend);

	return (msg);
}

unsigned int					CommandAcceptAdd::getSizeBody(void){
	return sizeof(CommandAcceptAdd::Body);
}
