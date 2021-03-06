#include "Client.hpp"
#include "Database.hpp"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <ctime>

/*
** Copelien
*/
Client::Client(IClientSocket* clientSocket, Client::OnClientEvent* listenerClient) : 
Socket(clientSocket),
status(Client::Status::DISCONNECTED),
statusCall(Client::StatusCall::NONE),
pseudo(""),
account(""),
isConnected(false),
Listener(listenerClient),
lastPingTime(std::time(nullptr)),
communicationClient(NULL),
handshaked(false)
{
    if (this->Socket)
        this->Socket->setOnSocketEventListener(this);
    this->handleCmd = this->Socket ? new HandleCmd(this->Socket) : NULL;
    usersFolderPath = getAbsolutePathDatabaseUsersFolder().string();
}

Client::~Client()
{
    if (Listener)
        Listener->onCloseConnection(this);
    delete this->handleCmd;
}

const Client & Client::operator = (const Client &other)
{
    if (&other != this)
    {
        this->status = other.status;
        this->statusCall = other.statusCall;
        this->pseudo = other.pseudo;
        this->contact = other.contact;
        this->account = other.account;
        this->isConnected = other.isConnected;
        this->Listener = other.Listener;
        this->lastPingTime = other.lastPingTime;
        this->communicationClient = other.communicationClient;
        this->Socket = other.Socket;
        this->handleCmd = other.handleCmd;
        this->usersFolderPath = other.usersFolderPath;
    }
    return *this;
}

void Client::disconnect(void)
{
    this->status = Client::Status::DISCONNECTED;
    this->statusCall = Client::StatusCall::NONE;
    this->isConnected = false;
    this->lastPingTime = std::time(nullptr);
    this->communicationClient = NULL;
}

void Client::connect(void)
{
    this->status = Client::Status::CONNECTED;
    this->statusCall = Client::StatusCall::NONE;
    this->isConnected = true;
    this->lastPingTime = std::time(nullptr);
    this->communicationClient = NULL;
    this->handshaked = true;
}

void Client::resetAttributes(void)
{
    this->pseudo = "";
    this->account = "";
    this->clearContact();
}

/*
** IClientSocket::OnSocketEvent
*/
void	Client::onSocketReadable(IClientSocket *, unsigned int)
{
    std::vector<std::string> *param;
    ICommand::Instruction instruction;

    while ((param = this->handleCmd->unPackCmd()) != NULL)
    {
        instruction = this->handleCmd->getInstruction();
        this->treatCommand(instruction, *param);
        delete param;
        param = NULL;
    }
}

void    Client::onBytesWritten(IClientSocket *, unsigned int)
{

}

void	Client::onSocketClosed(IClientSocket*)
{
    std::cout << "[CLIENT] onSocketClose " << this->getAccount() << std::endl;
    delete this;
}

/*
** internal functions
*/
boost::filesystem::path Client::getAbsolutePathDatabaseUsersFolder(void)
{
    boost::filesystem::path absolutePathDatabaseUsersFolder = boost::filesystem::complete(Database::DATABASE_FOLDER_USERS);
    if (boost::filesystem::exists(absolutePathDatabaseUsersFolder) == false)
        boost::filesystem::create_directory(absolutePathDatabaseUsersFolder);
    return absolutePathDatabaseUsersFolder;
}

bool Client::saveData(void)
{
    const std::string& path = usersFolderPath + this->account + Database::DATABASE_EXTENSION;
    std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
    if (!ofs.good() || ofs.fail())
        return false;
    boost::archive::text_oarchive oa(ofs);
    oa << *this;
    ofs.close();
    return true;
}

bool Client::loadData(void)
{
    const std::string& path = usersFolderPath + this->account + Database::DATABASE_EXTENSION;
    std::ifstream ifs(path);
    if (!ifs.good() || ifs.fail())
        return false;
    boost::archive::text_iarchive ia(ifs);
    ia >> *this;
    ifs.close();
    return true;
}

/*
** Use client's data
*/

// ** Setter
void Client::setConnected(bool state) { this->isConnected = state; }
void Client::setHandshaked(bool state) { this->handshaked = state; }
void Client::setStatus(Client::Status state) { this->status = state; }
void Client::setStatusCall(Client::StatusCall state) { this->statusCall = state; }
void Client::setPseudo(const std::string& pseudoName) { this->pseudo = pseudoName; }
void Client::setAccount(const std::string& accountName) { this->account = accountName; }
void Client::addContact(const std::string& accountName)
{ 
    if (std::find(this->contact.begin(), this->contact.end(), accountName) == this->contact.end())
        this->contact.push_back(accountName);
}
void Client::delContact(const std::string& accountName)
{
    if (std::find(this->contact.begin(), this->contact.end(), accountName) != this->contact.end())
        this->contact.remove(accountName);
}
void Client::clearContact() { this->contact.clear(); }
void Client::setLastPingTime(std::time_t time) { this->lastPingTime = time; }
void Client::setCommunicationClient(Client* client)
{
    this->communicationClient = client;
}

// ** Getter
Client::Status				  Client::getStatus(void) const { return this->status; }
Client::StatusCall			  Client::getStatusCall(void) const { return this->statusCall; }
const std::string&            Client::getPseudo(void) const { return this->pseudo; }
const std::string&            Client::getAccount(void) const { return this->account; }
const std::list<std::string>& Client::getContact(void) const { return this->contact; }
IClientSocket*                Client::getSocket(void) const { return this->Socket; }
bool						  Client::isConnect(void) const { return this->isConnected; }
bool                          Client::isHandshaked(void) const { return this->handshaked; }
std::time_t	                  Client::getLastPingTime() const { return this->lastPingTime; }
Client*                       Client::getCommunicationClient(void) const { return this->communicationClient; }
Client::OnClientEvent*        Client::getListener(void) const { return this->Listener; }

bool                          Client::isAlreadyFriends(const std::string& accountName) const
{
    return (std::find(this->contact.begin(), this->contact.end(), accountName) != this->contact.end());
}

const Client::HandleCommand Client::handleCommandsTab[] =
{
    { ICommand::HANDSHAKE, &Client::OnClientEvent::onHandshake },
    { ICommand::ADD, &Client::OnClientEvent::onAdd },
    { ICommand::UPDATE, &Client::OnClientEvent::onUpdate },
    { ICommand::REG, &Client::OnClientEvent::onReg },
    { ICommand::LOG, &Client::OnClientEvent::onLog },
    { ICommand::LIST, &Client::OnClientEvent::onList },
    { ICommand::SHOW, &Client::OnClientEvent::onShow },
    { ICommand::CALL, &Client::OnClientEvent::onCall },
    { ICommand::ACCEPT_ADD, &Client::OnClientEvent::onAcceptAdd },
    { ICommand::DEL, &Client::OnClientEvent::onDel },
    { ICommand::EXIT, &Client::OnClientEvent::onExit },
    { ICommand::SEND, &Client::OnClientEvent::onSend },
    { ICommand::ACCEPT_CALL, &Client::OnClientEvent::onAcceptCall },
    { ICommand::CLOSE_CALL, &Client::OnClientEvent::onCloseCall },
    { ICommand::UNKNOWN_INSTRUCTION, NULL }
};

void	Client::treatCommand(ICommand::Instruction instruction, std::vector<std::string> &param)
{
    if (this->Listener)
    {
        if (this->handshaked == false && instruction != ICommand::HANDSHAKE)
        {
            std::cout << "onClose" << std::endl;
            ((this->Listener)->onCloseConnection)(this);
        }   
        else
        {
            int i;
            for (i = 0; handleCommandsTab[i].instruction != ICommand::UNKNOWN_INSTRUCTION && handleCommandsTab[i].instruction != instruction; ++i);

            if (handleCommandsTab[i].instruction == instruction)
                ((this->Listener)->*handleCommandsTab[i].handler)(this, param, instruction);
        }
    }
}

void Client::display() const
{
    std::cout
        << "        [#] account      : '" << this->account << "'" << std::endl
        << "        [#] pseudo       : '" << this->pseudo << "'" << std::endl
        << "        [#] status       : '" << this->status << "'" << std::endl
        << "        [#] statusCall   : '" << this->statusCall << "'" << std::endl
        << "        [#] isConnected  : '" << std::boolalpha << this->isConnected << "'" << std::endl
        << "        [#] lastPingTime : '" << this->lastPingTime << "'" << std::endl
        << "        [#] handshaked   : '" << std::boolalpha << this->handshaked << "'" << std::endl
        << "        [#] contacts     : ";
    if (this->getContact().size())
    {
        std::for_each(this->contact.begin(), this->contact.end(), [](const std::string &targetAccount) {
            std::cout << "'" << targetAccount << "' ";
        });
        std::cout << std::endl;
    }
    else
        std::cout << " no friends" << std::endl;
}
