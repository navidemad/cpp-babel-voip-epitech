#pragma once

#include <qstring.h>
#include "ICommand.hpp"

class CommandCloseCall : public ICommand {

	// packet
	private:
#ifdef WIN32
		struct __declspec(align(1)) PacketFromClient{
			ICommand::Header	header;
			char				accountName[256];
		};

		struct __declspec(align(1)) PacketFromServer{
			char	accountName[256];
		};
#else
		struct __attribute__((packed)) PacketFromClient{
			ICommand::Header	header;
			char				accountName[256];
		};

		struct __attribute__((packed)) PacketFromServer{
			char	accountName[256];
		};
#endif

	// ctor - dtor
	public:
		CommandCloseCall(void);
		~CommandCloseCall(void);

	// coplien form
	private:
		CommandCloseCall(const CommandCloseCall &) {}
		const CommandCloseCall &operator=(const CommandCloseCall &) { return *this; }

	// implementation
	public:
		ICommand::Instruction	getInstruction(void) const;
		IClientSocket::Message	getMessage(void) const;
		unsigned int			getSizeToRead(void) const;
		void					initFromMessage(const IClientSocket::Message &message);

	// getters - setters
	public:
		const QString	&getAccountName(void) const;
		void			setAccountName(const QString &accountName);

	// attributes
	private:
		QString	mAccountName;

};
