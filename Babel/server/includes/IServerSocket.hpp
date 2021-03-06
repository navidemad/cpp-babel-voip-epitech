#pragma once

#include "IClientSocket.hpp"

class IServerSocket {

	// CallBack interface
	public:
		class OnSocketEvent {
			public:
				virtual	~OnSocketEvent() {}
				virtual void	onNewConnection(IServerSocket *socket) = 0;
		};

	// virtual destructor
	public:
		virtual ~IServerSocket(void) {}

	// init
	public:
		virtual void	createServer(int port, int queueSize) = 0;
		virtual void	closeServer() = 0;

	// listeners
	public:
		virtual void	setOnSocketEventListener(IServerSocket::OnSocketEvent *listener) = 0;

	// handle clients
	public:
		virtual IClientSocket	*getNewClient() = 0;
		virtual bool			hasClientInQueue() const = 0;
		
};
