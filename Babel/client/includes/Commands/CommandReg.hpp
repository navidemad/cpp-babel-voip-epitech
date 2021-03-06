#pragma once

#include <qstring.h>
#include "ICommand.hpp"

class CommandReg : public ICommand {

	// packet
	private:
		#pragma pack(push, 1)
		struct PacketFromClient {
			char	accountName[256];
			char	pseudo[256];
			char	password[256];
		};
		#pragma pack(pop)

	// ctor - dtor
	public:
		CommandReg(void);
		~CommandReg(void);

	// coplien form
	private:
		CommandReg(const CommandReg &) {}
		const CommandReg &operator=(const CommandReg &) { return *this; }

	// implementation
	public:
		ICommand::Instruction	getInstruction(void) const;
		IClientSocket::Message	getMessage(void) const;
		unsigned int			getSizeToRead(void) const;
		void					initFromMessage(const IClientSocket::Message &message);

		// getters - setters
	public:
		const QString	&getAccountName(void) const;
		const QString	&getPseudo(void) const;
		const QString	&getPassword(void) const;
		void			setAccountName(const QString &accountName);
		void			setPseudo(const QString &pseudo);
		void			setPassword(const QString &password);

	// attributes
	private:
		QString	mAccountName;
		QString	mPseudo;
		QString	mPassword;

};
