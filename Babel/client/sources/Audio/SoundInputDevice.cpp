#include "SoundInputDevice.hpp"
#include "SoundException.hpp"
#include "SoundOutputDevice.hpp"
#include "ScopedLock.hpp"
#include <cstring>

SoundInputDevice::SoundInputDevice(void) : mStream(NULL) {
	if (Pa_Initialize() != paNoError)
		throw new SoundException("fail Pa_Initialize");

	initInputDevice();
}

SoundInputDevice::~SoundInputDevice(void) {
	Pa_Terminate();
}


void	SoundInputDevice::initInputDevice(void) {
	mInputParameters.device = Pa_GetDefaultInputDevice();
	if (mInputParameters.device == paNoDevice)
		throw new SoundException("fail Pa_GetDefaultInputDevice");

	mInputParameters.channelCount = Sound::NB_CHANNELS;
	mInputParameters.sampleFormat = paFloat32;
	mInputParameters.suggestedLatency = Pa_GetDeviceInfo(mInputParameters.device)->defaultLowInputLatency;
	mInputParameters.hostApiSpecificStreamInfo = NULL;
}

void	SoundInputDevice::startStream(void) {
	if (Pa_OpenStream(&mStream, &mInputParameters, NULL, Sound::SAMPLE_RATE, Sound::FRAMES_PER_BUFFER, paClipOff, SoundInputDevice::callback, this) != paNoError)
		throw new SoundException("fail Pa_OpenStream");

	if (Pa_StartStream(mStream) != paNoError)
		throw new SoundException("fail Pa_StartStream");
}

void	SoundInputDevice::stopStream(void) {
	if (Pa_CloseStream(mStream) != paNoError)
		throw new SoundException("fail Pa_StopStream");
}

#include <iostream>
ISoundDevice	&SoundInputDevice::operator<<(const Sound::Decoded &sound) {
	ScopedLock	scopedLock(&mMutex);

	if (sound.buffer) {
		Sound::Decoded *soundCpy = new Sound::Decoded;
		soundCpy->buffer = sound.buffer;
		soundCpy->size = sound.size;
		mBuffers.push_back(soundCpy);
	}

	return *this;
}

ISoundDevice	&SoundInputDevice::operator>>(Sound::Decoded &sound) {
	ScopedLock	scopedLock(&mMutex);

	if (mBuffers.size()) {
		Sound::Decoded *soundCpy = mBuffers.front();

		sound.buffer = soundCpy->buffer;
		sound.size = soundCpy->size;

		delete soundCpy;
		mBuffers.pop_front();
	}
	else {
		sound.size = 0;
		sound.buffer = NULL;
	}

	return *this;
}

int	SoundInputDevice::callback(const void *inputBuffer, void *, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *, PaStreamCallbackFlags, void *data) {
	SoundInputDevice *obj = reinterpret_cast<SoundInputDevice *>(data);
	ScopedLock scopedLock(&obj->mMutex);

	Sound::Decoded *sound = new Sound::Decoded;
	sound->size = framesPerBuffer * Sound::NB_CHANNELS;
	sound->buffer = new float[sound->size];
	std::memcpy(sound->buffer, inputBuffer, framesPerBuffer * Sound::NB_CHANNELS * sizeof(float));

	obj->mBuffers.push_back(sound);

	return paContinue;
}
