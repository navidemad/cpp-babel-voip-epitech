#include "CommandPacketBuilder.hpp"
#include "TcpClient.hpp"
#include "CommandException.hpp"
#include "SocketException.hpp"

CommandPacketBuilder::CommandPacketBuilder(void)
	: mClient(new TcpClient), mCurrentCommand(nullptr), mCurrentState(CommandPacketBuilder::HEADER)
{
	mClient->setOnSocketEventListener(this);
}

CommandPacketBuilder::~CommandPacketBuilder(void) {
}

void    CommandPacketBuilder::close(void) {
    mClient->closeClient();
}

void	CommandPacketBuilder::sendCommand(const ICommand *command) {
	try {
		ICommand::Header header;
		header.instructionCode = static_cast<int>(command->getInstruction());
		header.magicCode = ICommand::MAGIC_CODE;

		IClientSocket::Message message;
		IClientSocket::Message bodyMessage = command->getMessage();
		message.msg.assign(reinterpret_cast<char *>(&header), reinterpret_cast<char *>(&header + 1));
		message.msg.insert(message.msg.end(), bodyMessage.msg.begin(), bodyMessage.msg.end());
		message.msgSize = sizeof(ICommand::Header) + bodyMessage.msgSize;

		mClient->send(message);
	}
	catch (const SocketException &) {
		emit disconnectedFromHost();
	}
}

void	CommandPacketBuilder::connectToServer(const QString &addr, int port) {
	mClient->connect(addr.toStdString(), port);
}

void	CommandPacketBuilder::fetchCommandHeader(void) {
	IClientSocket::Message message = mClient->receive(ICommand::HEADER_SIZE);
	ICommand::Header *header = reinterpret_cast<ICommand::Header *>(message.msg.data());

	mCurrentCommand = ICommand::getCommand(static_cast<ICommand::Instruction>(header->instructionCode));
	mCurrentState = CommandPacketBuilder::CONTENT;

	if (header->magicCode != ICommand::MAGIC_CODE || mCurrentCommand == NULL) {
		mClient->closeClient();
		return;
	}

	onSocketReadable(mClient.get(), mClient->nbBytesToRead());
}

void	CommandPacketBuilder::fetchCommandContent(void) {
	IClientSocket::Message message = mClient->receive(mCurrentCommand->getSizeToRead());

	try {
		mCurrentCommand->initFromMessage(message);
	}
	catch (const CommandException &) {
		mClient->closeClient();
		return;
	}

	emit receiveCommand(mCurrentCommand);

	resetCurrentCommand();
	onSocketReadable(mClient.get(), mClient->nbBytesToRead());
}

void	CommandPacketBuilder::onSocketReadable(IClientSocket *, unsigned int nbBytesToRead) {
	if (mCurrentState == CommandPacketBuilder::HEADER && nbBytesToRead >= ICommand::HEADER_SIZE)
		fetchCommandHeader();
	else if (mCurrentState == CommandPacketBuilder::CONTENT && nbBytesToRead >= mCurrentCommand->getSizeToRead())
		fetchCommandContent();
}

void	CommandPacketBuilder::onSocketClosed(IClientSocket *) {
	resetCurrentCommand();
	emit disconnectedFromHost();
}

void	CommandPacketBuilder::resetCurrentCommand(void) {
	mCurrentCommand = nullptr;
	mCurrentState = CommandPacketBuilder::HEADER;
}