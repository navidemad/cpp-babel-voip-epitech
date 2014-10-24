#include "TcpClient.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>

#include <cstring>
#include <cstdio>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

//a enlever
#include "CommandUpdate.hpp"
#include <stdlib.h>
#include <string.h>

TcpClient::TcpClient() : mListener(NULL), mSocket(NULL)
{

}

TcpClient::~TcpClient()
{
    closeClient();
}

void TcpClient::connect(const std::string &/* addr */, int /* port */)
{
}

void TcpClient::initFromSocket(void *socket)
{
    mSocket = reinterpret_cast<tcp::socket*>(socket);
    startRead();
}

void TcpClient::closeClient()
{
    if (mSocket)
        mSocket->close();
    if (mListener)
        mListener->onSocketClosed(this);
}

void TcpClient::startRead()
{
    mSocket->async_receive(boost::asio::buffer(mReadBuffer, TcpClient::BUFFER_SIZE),
        boost::bind(&TcpClient::readHandler, this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpClient::send(const IClientSocket::Message &msg)
{
    boost::mutex::scoped_lock lock(mMutex);
    {
        bool write_in_progress = !mWriteMessageQueue.empty();
        if (msg.msgSize <= 0)
            return;
        mWriteMessageQueue.push_back(msg);
        if (!write_in_progress)
        {
            boost::asio::async_write(*mSocket,
                boost::asio::buffer(mWriteMessageQueue.front().msg, mWriteMessageQueue.front().msgSize),
                boost::bind(&TcpClient::sendHandler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
    }
}

void TcpClient::sendHandler(const boost::system::error_code &error, std::size_t bytesTransfered)
{
    if (!error)
    {
        if (mListener)
            mListener->onBytesWritten(this, bytesTransfered);
        boost::mutex::scoped_lock lock(mMutex);
        {
            delete[] mWriteMessageQueue.front().msg;
            mWriteMessageQueue.pop_front();
            if (!mWriteMessageQueue.empty())
            {
                boost::asio::async_write(*mSocket,
                    boost::asio::buffer(mWriteMessageQueue.front().msg, mWriteMessageQueue.front().msgSize),
                    boost::bind(&TcpClient::sendHandler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            }
        }
    }
    else
    {
        std::cerr << error.message() << std::endl;
        closeClient();
    }
}

IClientSocket::Message TcpClient::receive(unsigned int sizeToRead)
{
	IClientSocket::Message message;
	CommandUpdate::Body *body;
	ICommand::Header *header;

	this->mNbBytesToRead = sizeToRead;

	header = (ICommand::Header *)malloc(100);
	header->magicCode = ICommand::MAGIC_CODE;
	header->instructionCode = ICommand::UPDATE;

	body = (CommandUpdate::Body *)malloc(2000);
	body->accountName[0] = 0;
	strcat(body->accountName, "myAccountNameIsZer");
	body->password[0] = 0;
	strcat(body->password, "trollzer");
	body->pseudo[0] = 0;
	strcat(body->pseudo, "sheytane");
	body->status = '9';

	if (sizeToRead == ICommand::HEADER_SIZE)
		this->mMessage.msg = (char *)header;
	else
		this->mMessage.msg = (char *)body;
	return this->mMessage;
	//-----------------------------------------------------------------------------------------------
    if (sizeToRead > mBuffer.size())
    {
        message.msg = NULL;
        message.msgSize = -1;
        return message;
    }

    std::string str(mBuffer.begin(), mBuffer.begin() + sizeToRead);
    message.msg = new char[str.size() + 1];
    message.msgSize = str.size();
    std::copy(str.begin(), str.end(), message.msg);
    message.msg[str.size()] = '\0';
    
    mBuffer.erase(mBuffer.begin(), mBuffer.begin() + sizeToRead);

    return message;
}

void TcpClient::readHandler(const boost::system::error_code & error, std::size_t bytesTransfered)
{
    if (!error)
    {
        std::string str(mReadBuffer, bytesTransfered);
        mBuffer.insert(mBuffer.end(), str.begin(), str.end());
        if (mListener)
            mListener->onSocketReadable(this, bytesTransfered);
        startRead();
    }
    else
    {
        std::cout << error.message() << std::endl;
        closeClient();
    }
}

unsigned int TcpClient::nbBytesToRead() const
{

    //return mBuffer.size();
	return (2000);
}

void TcpClient::setOnSocketEventListener(IClientSocket::OnSocketEvent *listener)
{
    mListener = listener;
}