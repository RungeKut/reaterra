#ifndef WAVEFILE_H_
#define WAVEFILE_H_

#include <alsa/asoundlib.h>
#include <fstream>
#include <memory>
#include <string>

/// Звуковой файл.
class WaveFile {

public:

	/*!
	 * @brief Конструктор.
	 *
	 * @param file_name имя файла.
	 */
	WaveFile(std::string file_name);

	/// Деструктор.
	virtual ~WaveFile();

	/*!
	 * @brief Получение звуковых данных.
	 *
	 * @param size размер данных.
	 * @return данные.
	 */
	char* getData(size_t size);

	/// Установка указателя на начало данных.
	void rewind();

	/*!
	 * @brief Получение количества каналов.
	 *
	 * @return количество каналов.
	 */
	unsigned short getChannels() const;

	/*!
	 * @brief Получение количества отсчетов в секунду.
	 *
	 * @return количество отсчетов в секунду.
	 */
	unsigned int getSamplesPerSec() const;

	/*!
	 * @brief Получение формата кодирования.
	 *
	 * @return формат кодирования.
	 */
	snd_pcm_format_t getFormat() const;

	/*!
	 * @brief Получение количества битов на отсчет.
	 *
	 * @return количество битов на отсчет.
	 */
	unsigned short getBitsPerSample() const;

private:

// Заголовок WAVE файла
#pragma pack (push, 1)

	// IFF заголовок
	typedef struct {
		char id[4];
		unsigned length;		// Длина данных (включая остаток заголовка).
		char type[4];
	} FileHead;


	// Заголовок фрагмента IFF
	typedef struct {
		char id[4];				// ID фрагмента
		unsigned int length;	// Длина фрагмента
	} ChunkHead;

	// Фрагмент WAVE формата
	typedef struct {
		short format_tag;
		unsigned short channels;
		unsigned int samples_per_sec;
		unsigned int avg_bytes_per_sec;
		unsigned short block_align;
		unsigned short bits_per_sample;
	} Format;

#pragma pack(pop)


	std::ifstream m_file;
	bool m_valid;
	std::unique_ptr<char[]> m_buff;
	size_t m_pointer;
	size_t m_size;
	unsigned short m_channels;
	unsigned int m_samples_per_sec;
	unsigned short m_bits_per_sample;
	snd_pcm_format_t m_format;

	snd_pcm_format_t encodeFormat(unsigned short bits_per_sample);
	bool isValid() const;
	size_t getAvailable() const;

};

#endif /* WAVEFILE_H_ */
