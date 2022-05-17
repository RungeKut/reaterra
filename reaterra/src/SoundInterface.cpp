#include "SoundInterface.h"

#include <cmath>

#include "Context.h"
#include "EnumExtension.h"
#include "Logger.h"


SoundInterface::SoundInterface()
:	m_running {true},
	m_handle {nullptr},
	m_playing {false},
	m_sounds {
		std::make_unique<WaveFile>("reaterra_snds/click.wav"),
		std::make_unique<WaveFile>("reaterra_snds/welcome.wav"),
		std::make_unique<WaveFile>("reaterra_snds/alert.wav"),
		std::make_unique<WaveFile>("reaterra_snds/bell.wav"),
		std::make_unique<WaveFile>("reaterra_snds/info.wav")
	}
{
	LD("Ctor");

	int ret = system(("amixer -q set Headphone " + std::to_string(ctx.m_settings.getParamInt("sounds_volume")) + "%").c_str());
	if (ret) {
		LE("Sound volume setting error"  + std::to_string(ret));
	}

	m_thread = std::thread(&SoundInterface::worker, this);
}


SoundInterface::~SoundInterface() {
	m_running = false;
	m_queue.putEvent(Sound::Silent);

	if(m_thread.joinable()) {
    	m_thread.join();
    }
	LD("Dtor");
}


void SoundInterface::play(Sound sound) {
	LD("Play");
	m_playing = false;
//	if (!ctx.m_remote.isConnected()) {
		if (ctx.m_settings.getParamInt("sounds_volume")) {
			m_queue.putEvent(sound);
		}
//	}
}


void SoundInterface::worker() {
	LI("Sound interface is started");
	while (m_running) {

		Sound sound = m_queue.getEvent();

		if (sound == Sound::Silent) {
			m_playing = false;
			continue;
		} else {
			std::shared_ptr<WaveFile> wave = m_sounds.at(to_base(sound));
			LD("Start playing at rate " + std::to_string(wave->getSamplesPerSec()) + "   channels:" + std::to_string(wave->getChannels()));
		    int err = snd_pcm_open(&m_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
			if (err < 0) {
				LE("Sound interface open error: " + std::string {snd_strerror(err)});
				return;
			}
			LD("State: " + snd_pcm_state_name(snd_pcm_state(m_handle)));
			err = snd_pcm_set_params(m_handle, wave->getFormat(), SND_PCM_ACCESS_RW_INTERLEAVED, wave->getChannels(), wave->getSamplesPerSec(), 1, 100000);
			if (err < 0) {
				LE("Parameters setting error: " + std::string {snd_strerror(err)});
				snd_pcm_close(m_handle);
				return;
			}

//	        snd_pcm_sw_params_t *params;
//		    snd_pcm_sw_params_malloc(&params);
//		    snd_pcm_sw_params_current(m_handle, params);
//		    snd_pcm_sw_params_set_silence_size(m_handle, params, 1000);
//		    snd_pcm_sw_params_set_silence_threshold(m_handle, params, 1);
//		    snd_pcm_sw_params (m_handle, params);
//		    snd_pcm_sw_params_free(params);


//			static snd_output_t *output = NULL;
//			snd_output_stdio_attach(&output, stdout, 0);
//			snd_pcm_dump(m_handle, output);

			start(wave);
			m_playing = false;
			snd_pcm_close(m_handle);
			LD("Stop playing");
		}
	}
	LI("Sound interface is stopped");
}


void SoundInterface::start(std::shared_ptr<WaveFile> wave) {
	register snd_pcm_uframes_t frames;

	m_playing = true;

	char* data;
	while (m_playing) {
		snd_pcm_avail_update(m_handle);
		if (snd_pcm_avail(m_handle) < 256 / (wave->getBitsPerSample() * wave->getChannels() / 8)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		data = wave->getData(256);
		if (!data) {
			break;
		}
		frames = snd_pcm_writei(m_handle, data, 256 / (wave->getBitsPerSample() * wave->getChannels() / 8));
		if (frames < 0) {
			frames = snd_pcm_recover(m_handle, frames, 0);
			LD("Recover");
		}
		if (frames < 0) {
			LE("Error playing wave: " + snd_strerror(frames));
			break;
		}
	}

	while (m_playing && snd_pcm_state(m_handle) == SND_PCM_STATE_RUNNING) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	snd_pcm_pause(m_handle, 1);
	snd_pcm_drop(m_handle);
	wave->rewind();
}
