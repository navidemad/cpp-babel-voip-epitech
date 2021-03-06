#include "UdpClient.hpp"
#include "SocketException.hpp"
#include <memory>

UdpClient::UdpClient(void)
	: mQUdpSocket(NULL), mIsReadable(false), mListener(NULL)
{
	mQUdpSocket = new QUdpSocket(this);
}

UdpClient::~UdpClient(void) {
	if (mQUdpSocket)
		delete mQUdpSocket;
}

void	UdpClient::connect(const std::string &/*addr*/, int port) {
	close();
	if (mQUdpSocket->bind(QHostAddress::Any, port) == false)
		throw SocketException("fail QUdpSocket::bind");

	QObject::connect(mQUdpSocket, SIGNAL(readyRead()), this, SLOT(markAsReadable()));
	QObject::connect(mQUdpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
}

void	UdpClient::initFromSocket(void *socket) {
	mQUdpSocket = reinterpret_cast<QUdpSocket *>(socket);

	QObject::connect(mQUdpSocket, SIGNAL(readyRead()), this, SLOT(markAsReadable()));
	QObject::connect(mQUdpSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
}

void	UdpClient::closeClient(void) {
	close();
}

void	UdpClient::close(void) {
	if (mQUdpSocket->state() == QAbstractSocket::ConnectedState)
		mQUdpSocket->close();

	if (mListener)
		mListener->onSocketClosed(this);
}


void	UdpClient::send(const IClientSocket::Message &message) {
	mQUdpSocket->writeDatagram(message.msg.data(), message.msgSize, QHostAddress(QString(message.host.c_str())), message.port);
}

IClientSocket::Message	UdpClient::receive(unsigned int sizeToRead) {
	IClientSocket::Message message;
	QHostAddress host;
	quint16 port;

	if (nbBytesToRead() == 0) {
		message.msgSize = 0;

		return message;
	}

	std::unique_ptr<char[]> buffer(new char[sizeToRead]);

	message.msgSize = mQUdpSocket->readDatagram(buffer.get(), sizeToRead, &host, &port);
	if (message.msgSize == -1)
		throw SocketException("fail QUdpSocket::read");

	message.msg.insert(message.msg.end(), buffer.get(), buffer.get() + message.msgSize);
	message.host = host.toString().toStdString();
	message.port = port;
	mIsReadable = false;

	return message;
}

unsigned int	UdpClient::nbBytesToRead(void) const {
	return mQUdpSocket->bytesAvailable();
}

void	UdpClient::markAsReadable(void) {
	mIsReadable = true;

	if (mListener)
		mListener->onSocketReadable(this, mQUdpSocket->bytesAvailable());
}

void	UdpClient::bytesWritten(qint64 nbBytes) {
	if (mListener)
		mListener->onBytesWritten(this, nbBytes);
}

void	UdpClient::setOnSocketEventListener(OnSocketEvent *listener) {
	mListener = listener;
}

std::string UdpClient::getRemoteIp() const {
    return "127.0.0.1";
}
