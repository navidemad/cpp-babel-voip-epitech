#include "SoundInputDevice.hpp"
#include "SoundException.hpp"
#include "SoundOutputDevice.hpp"
#include "ScopedLock.hpp"
#include <cstring>

SoundInputDevice::SoundInputDevice(void) : mStream(NULL), mIsRunning(false), mListener(NULL) {
	if (Pa_Initialize() != paNoError)
		throw SoundException("fail Pa_Initialize");

	initInputDevice();
}

SoundInputDevice::~SoundInputDevice(void) {
	stopStream();
	Pa_Terminate();
}


void	SoundInputDevice::initInputDevice(void) {
	mInputParameters.device = Pa_GetDefaultInputDevice();
	if (mInputParameters.device == paNoDevice)
		throw SoundException("fail Pa_GetDefaultInputDevice");

	mInputParameters.channelCount = Sound::NB_CHANNELS;
	mInputParameters.sampleFormat = paFloat32;
	mInputParameters.suggestedLatency = Pa_GetDeviceInfo(mInputParameters.device)->defaultLowInputLatency;
	mInputParameters.hostApiSpecificStreamInfo = NULL;
}

void	SoundInputDevice::startStream(void) {
	if (mIsRunning == false) {
		if (Pa_OpenStream(&mStream, &mInputParameters, NULL, Sound::SAMPLE_RATE, Sound::FRAMES_PER_BUFFER, paClipOff, SoundInputDevice::callback, this) != paNoError)
			throw SoundException("fail Pa_OpenStream");

		if (Pa_StartStream(mStream) != paNoError)
			throw SoundException("fail Pa_StartStream");

		mIsRunning = true;
	}
}

void	SoundInputDevice::stopStream(void) {
	if (mIsRunning == true) {
		if (Pa_CloseStream(mStream) != paNoError)
			throw SoundException("fail Pa_StopStream");

		mIsRunning = false;
	}
}

ISoundDevice	&SoundInputDevice::operator<<(const Sound::Decoded &) {
	return *this;
}

ISoundDevice	&SoundInputDevice::operator>>(Sound::Decoded &sound) {
	ScopedLock	scopedLock(&mMutex);

	if (mBuffers.size()) {
		Sound::Decoded soundCpy = mBuffers.front();

		sound.buffer = soundCpy.buffer;
		sound.size = soundCpy.size;

		mBuffers.pop_front();
	}
	else
		sound.size = 0;

	return *this;
}

int	SoundInputDevice::callback(const void *inputBuffer, void *, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags, void *data) {
	SoundInputDevice *obj = reinterpret_cast<SoundInputDevice *>(data);

	{
		ScopedLock scopedLock(&obj->mMutex);

		Sound::Decoded sound;
		sound.size = framesPerBuffer * Sound::NB_CHANNELS;
		sound.buffer.assign(reinterpret_cast<const float *>(inputBuffer), reinterpret_cast<const float *>(inputBuffer) + framesPerBuffer * Sound::NB_CHANNELS);

		obj->mBuffers.push_back(sound);
	}

	if (obj->mListener)
	  obj->mListener->onSoundAvailable(obj);

	return paContinue;
}

void	SoundInputDevice::setOnSoundDeviceEventListener(ISoundDevice::OnSoundDeviceEvent *listener) {
  mListener = listener;
}
