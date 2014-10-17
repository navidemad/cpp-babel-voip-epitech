#include "EncodeManager.hpp"
#include "SoundException.hpp"

EncodeManager::EncodeManager(void) {
	int error;

	mEncoder = opus_encoder_create(Sound::SAMPLE_RATE, Sound::NB_CHANNELS, OPUS_APPLICATION_VOIP, &error);
	if (error != OPUS_OK)
		throw new SoundException("fail opus_encoder_create");

	mDecoder = opus_decoder_create(Sound::SAMPLE_RATE, Sound::NB_CHANNELS, &error);
	if (error != OPUS_OK)
		throw new SoundException("fail opus_decoder_create");
}

EncodeManager::~EncodeManager(void) {
	if (mEncoder)
		opus_encoder_destroy(mEncoder);

	if (mDecoder)
		opus_decoder_destroy(mDecoder);
}

Sound::Encoded	EncodeManager::encode(const Sound::Decoded &sound) {
	Sound::Encoded encoded;

	encoded.buffer = new unsigned char[sound.size];
	encoded.size = opus_encode_float(mEncoder, sound.buffer, Sound::FRAMES_PER_BUFFER, encoded.buffer, sound.size);

	if (encoded.size < 0)
		throw new SoundException("fail opus_encode_float");

	return encoded;
}

Sound::Decoded	EncodeManager::decode(const Sound::Encoded &sound) {
	Sound::Decoded decoded;

	decoded.buffer = new float[Sound::FRAMES_PER_BUFFER * Sound::NB_CHANNELS];
	decoded.size = opus_decode_float(mDecoder, sound.buffer, sound.size, decoded.buffer, Sound::FRAMES_PER_BUFFER, 0);

	if (decoded.size < 0)
		throw new SoundException("fail opus_decode_float");

	return decoded;
}
