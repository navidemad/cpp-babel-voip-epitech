#include <QDate>
#include <cstring>
#include "SoundPacketBuilder.hpp"
#include "UdpClient.hpp"

const int SoundPacketBuilder::DEFAULT_BABEL_CALL_PORT = 4242;

SoundPacketBuilder::SoundPacketBuilder(void)
: mClient(NULL), mAcceptedHost(""), mAcceptedPort(0), mTimestamp(0)
{
	mClient = new UdpClient;
	mClient->setOnSocketEventListener(this);
	mClient->connect("127.0.0.1", SoundPacketBuilder::DEFAULT_BABEL_CALL_PORT);
}

SoundPacketBuilder::~SoundPacketBuilder(void) {
}

void	SoundPacketBuilder::acceptPacketsFrom(const QString &addr) {
	mAcceptedHost = addr;
	mAcceptedPort = SoundPacketBuilder::DEFAULT_BABEL_CALL_PORT;
}

void	SoundPacketBuilder::sendSound(const Sound::Encoded &sound) {
	SoundPacketBuilder::SoundPacket soundPacket;

	soundPacket.magic_code = 0x150407CA;
	soundPacket.soundSize = sound.size;
	std::memset(soundPacket.sound, 0, sizeof(soundPacket.sound));
	memcpy(soundPacket.sound, sound.buffer.data(), sound.size);
	soundPacket.timestamp = QDateTime::currentDateTime().toTime_t();


	IClientSocket::Message msg;

	msg.msg.assign(reinterpret_cast<char *>(&soundPacket), reinterpret_cast<char *>(&soundPacket + 1));
	msg.msgSize = sizeof(soundPacket);
	msg.host = mAcceptedHost.toStdString();
	msg.port = mAcceptedPort;

	mClient->send(msg);
}

void	SoundPacketBuilder::onBytesWritten(IClientSocket *, unsigned int) {
}

void	SoundPacketBuilder::onSocketReadable(IClientSocket *, unsigned int) {
	IClientSocket::Message msg;
	SoundPacketBuilder::SoundPacket soundPacket;
	Sound::Encoded sound;

	msg = mClient->receive(sizeof(soundPacket));
	if (msg.host == mAcceptedHost.toStdString() && msg.port == mAcceptedPort) {
		memcpy(&soundPacket, msg.msg.data(), msg.msgSize);

		if (soundPacket.magic_code == 0x150407CA && soundPacket.timestamp >= mTimestamp) {
			sound.buffer.assign(soundPacket.sound, soundPacket.sound + sizeof(soundPacket.sound));
			sound.size = soundPacket.soundSize;
			mTimestamp = soundPacket.timestamp;

			emit SoundPacketBuilder::receiveSound(sound);
		}
	}
}

void	SoundPacketBuilder::onSocketClosed(IClientSocket *) {
}
