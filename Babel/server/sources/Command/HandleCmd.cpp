#include "HandleCmd.hpp"
#include "Factory.hpp"

std::vector<std::string>		*HandleCmd::unPackCmd(void){
	IClientSocket::Message		data;
	std::vector<std::string>	*param;

	if (this->header == NULL && this->socket->nbBytesToRead() >= ICommand::HEADER_SIZE){
		data = this->socket->receive(ICommand::HEADER_SIZE);
		this->header = reinterpret_cast<ICommand::Header *>(data.msg);
        this->instruction = static_cast<ICommand::Instruction>(header->instructionCode);
		this->body = Factory::getCommand(this->getInstruction());
        if (!(this->body = Factory::getCommand(this->getInstruction())))
            this->header = NULL;
	}

	if (this->header != NULL && this->socket->nbBytesToRead() >= this->body->getSizeBody()){
		param = this->body->getParam(this->socket);
		delete this->body;
        this->body = NULL;
		this->header = NULL;
		return param;
	}
	return NULL;
}

void						HandleCmd::packCmd(ICommand::Instruction instruction, const std::vector<std::string> &param){
	ICommand				*cmd;
    IClientSocket::Message	*msg;

    if ((cmd = Factory::getCommand(instruction)))
    {
        if ((msg = cmd->setParam(param)))
        {
            this->socket->send(*msg);
        }   
        else
        {
            std::cerr << "[USAGE] InstructionCode '" << instruction << "' SHOULD NOT BE EXEC]" << std::endl;
        }
    }
    else
    {
        std::cerr << "  [UNKNOWN CMD]" << std::endl;
    }	
}

ICommand::Instruction		HandleCmd::getInstruction(void){
	return this->instruction;
}