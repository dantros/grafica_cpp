
/*
 * Based on the tutorial: https://ffainelli.github.io/openal-example/
 * and the source code: https://github.com/ffainelli/openal-example/blob/master/openal-example.c
*/

#include <iostream>
#include <cassert>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#include <chrono>

#define OPENALCALL(function)\
	function;\
	{\
		ALenum error = alGetError();\
		assert(error != AL_NO_ERROR, "OpenAL Error");\
	}

static void list_audio_devices(const ALCchar* devices)
{
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");
}

bool load_wav_file(const char* audiofile, ALuint bufferId)
{
	/* loading the wav file to OpenAL */
	struct WavData {
		unsigned int channels = 0;
		unsigned int sampleRate = 0;
		drwav_uint64 totalPCMFrameCount = 0;
		std::vector<uint16_t> pcmData;
		drwav_uint64 GetTotalSamples() const { return totalPCMFrameCount * channels; }
	};

	/*
	* Primero se cargan los datos del archivo ubicado en audioFilePath
	* usando la libreria drwav (https://github.com/mackron/dr_libs)
	*/

	WavData audioData;
	drwav_int16* sampleData = drwav_open_file_and_read_pcm_frames_s16(
		audiofile,
		&audioData.channels,
		&audioData.sampleRate,
		&audioData.totalPCMFrameCount,
		nullptr);

	if (!sampleData)
	{
		std::cerr << "Audio Clip Error: Failed to load file " << audiofile;
		drwav_free(sampleData, nullptr);
		return false;
	}
	else if (audioData.GetTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
	{
		std::cerr << "Audio Clip Error: File " << audiofile << " is to big to be loaded.";
		drwav_free(sampleData, nullptr);
		return false;
	}

	/*at this point, the wav file is correctly loaded */

	//Si la carga usando dr_wav fue exitosa se comienza el transpaso de estos datos a OpenAL.
	audioData.pcmData.resize(size_t(audioData.GetTotalSamples()));

	//Primero se copian todos los datos a un vector de uint16_t, para luego liberar los datos recien copiados.
	std::memcpy(audioData.pcmData.data(), sampleData, audioData.pcmData.size() * 2);
	drwav_free(sampleData, nullptr);

	assert(bufferId != 0);

	OPENALCALL(alBufferData(
		bufferId,
		audioData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
		audioData.pcmData.data(),
		audioData.pcmData.size() * 2,
		audioData.sampleRate)
	);

	/* end loading wav to OpenAL */
}


int main()
{
	/* Conecting to the default device */
	ALCdevice* device = OPENALCALL(alcOpenDevice(NULL));
	if (device == nullptr) return -1;

	/* Checking if our OpenAL support enumeration of devices */
	ALboolean enumeration = OPENALCALL(alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"));
	if (enumeration == AL_FALSE)
		std::cout << "enumeration not supported" << std::endl;
	else
		std::cout << "enumeration supported" << std::endl;


	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	ALCcontext* context = OPENALCALL(alcCreateContext(device, NULL));
	bool success = OPENALCALL(alcMakeContextCurrent(context));
	if (!success) return -1;

	std::cout << "OpenAL context OK" << std::endl;

	/* Setting up the listener */
	ALfloat listenerOrigin[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

	OPENALCALL(alListener3f(AL_POSITION, 0, 0, 1.0f));
	OPENALCALL(alListener3f(AL_VELOCITY, 0, 0, 0));
	OPENALCALL(alListenerfv(AL_ORIENTATION, listenerOrigin));

	/* Setting up a source */
	ALuint source;
	OPENALCALL(alGenSources((ALuint)1, &source));
	OPENALCALL(alSourcef(source, AL_PITCH, 1));
	OPENALCALL(alSourcef(source, AL_GAIN, 1));
	OPENALCALL(alSource3f(source, AL_POSITION, 0, 0, 0));
	OPENALCALL(alSource3f(source, AL_VELOCITY, 0, 0, 0));
	OPENALCALL(alSourcei(source, AL_LOOPING, AL_FALSE));

	/* Generating a buffer */
	ALuint buffer;
	OPENALCALL(alGenBuffers((ALuint)1, &buffer));

	success = load_wav_file("demo.wav", buffer);
	if (!success) return -1;

	std::cout << "wav file loaded correctly" << std::endl;

	/* Binding the buffer with the data to source */
	OPENALCALL(alSourcei(source, AL_BUFFER, buffer));

	auto t0 = std::chrono::steady_clock::now();

	/* Playing the source */
	OPENALCALL(alSourcePlay(source));

	/* Wait while playing the song */
	ALint source_state;
	OPENALCALL(alGetSourcei(source, AL_SOURCE_STATE, &source_state));
	while (source_state == AL_PLAYING) {
		OPENALCALL(alGetSourcei(source, AL_SOURCE_STATE, &source_state));
	}

	auto dt = std::chrono::steady_clock::now() - t0;

	// cleanup context
	OPENALCALL(alDeleteSources(1, &source));
	OPENALCALL(alDeleteBuffers(1, &buffer));
	device = OPENALCALL(alcGetContextsDevice(context));
	OPENALCALL(alcMakeContextCurrent(NULL));
	OPENALCALL(alcDestroyContext(context));
	OPENALCALL(alcCloseDevice(device));

	const auto duration = std::chrono::duration_cast<std::chrono::seconds>(dt).count();

	std::cout << "The wav file lasted " << duration << " seconds." << std::endl;
}
