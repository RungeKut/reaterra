#ifndef SRC_SOUNDINTERFACE_H_
#define SRC_SOUNDINTERFACE_H_

#include <atomic>
#include <chrono>
#include <thread>

#include <alsa/asoundlib.h>

#include "SyncQueue.h"
#include "WaveFile.h"

/// Управление звуком.
class SoundInterface {

public:

	/// Вид звука.
	enum class Sound {
		Silent = -1,	///< Тишина.
		Click = 0,		///< Щелчок.
		Welcome,		///< Приветствие.
		Alarm,			///< Предупреждение.
		Bell,			///< Предупреждение ЧСС.
		Info			///< Информация.
	};

	/// Конструктор.
	SoundInterface();

	/// Деструктор.
	virtual ~SoundInterface();

	/*!
	 * Воспроизвести звук.
	 *
	 * @param sound тип звука.
	 */
	void play(Sound sound);


private:

	std::atomic_bool m_running;
	std::thread m_thread;
	snd_pcm_t *m_handle;
	bool m_playing;
	SyncQueue<Sound> m_queue;
	std::vector<std::shared_ptr<WaveFile>> m_sounds;

	void worker();
	void start(std::shared_ptr<WaveFile> wave);

};

#endif /* SRC_SOUNDINTERFACE_H_ */
