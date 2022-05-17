#include "WaveFile.h"

#include <cstring>
#include <ios>

#include "Logger.h"


static const char RIFF[] = {'R', 'I', 'F', 'F'};
static const char WAVE[] = {'W', 'A', 'V', 'E'};
static const char FMT[] = {'f', 'm', 't', ' '};
static const char DATA[] = {'d', 'a', 't', 'a'};


WaveFile::WaveFile(std::string file_name)
:	m_file(file_name, std::ios::binary),
	m_valid {false},
	m_pointer {0},
	m_size {0},
	m_channels {2},
	m_samples_per_sec {44100},
	m_bits_per_sample {16},
	m_format {SND_PCM_FORMAT_S16}
{
	FileHead file_head;
	LD("Load sound file: " + file_name);
	m_file.read(reinterpret_cast<char*>(&file_head), sizeof(FileHead));
	if (strncmp(file_head.id, RIFF, 4) || strncmp(file_head.type, WAVE, 4)) {
		LE("Error! Sound file is not WAV");
		return;
	}

	ChunkHead chunk_head;
	while(m_file.read(reinterpret_cast<char*>(&chunk_head), sizeof(ChunkHead)), !m_file.eof()) {
		if (!strncmp(chunk_head.id, FMT, 4)) {
			LE("Format chunk found. Chunk size: " + std::to_string(chunk_head.length));
			Format format;
			m_file.read(reinterpret_cast<char*>(&format), sizeof(Format));
			if (chunk_head.length != sizeof(Format)) {
				LD("There are extra format data");
			}

			m_channels = format.channels;
			m_samples_per_sec = format.samples_per_sec;
			m_bits_per_sample = format.bits_per_sample;
			m_format = encodeFormat(m_bits_per_sample);

			LD("Format:" + (format.format_tag == 1 ? "PCM" : "notPCM")
				+ "  bps:" + std::to_string(format.avg_bytes_per_sec)
				+ "  sps:" + std::to_string(format.samples_per_sec)
				+ "  bitps:" + std::to_string(format.bits_per_sample)
				+ "  block_al:" + std::to_string(format.block_align)
				+ "  ch:" + std::to_string(format.channels)
			);

			if (format.format_tag != 1 && format.block_align != 4 && format.bits_per_sample != 16) {
				LE("Unsupported format!");
				return;
			}
		} else if (!strncmp(chunk_head.id, DATA, 4)) {
			LE("Data chunk found. Chunk size: " + std::to_string(chunk_head.length));
			m_buff = std::make_unique<char[]>(chunk_head.length);
			m_file.read(m_buff.get(), chunk_head.length);
			if (m_file.gcount() != static_cast<int>(chunk_head.length)) {
				LE("Error! Read error");
				return;
			}
			m_size = chunk_head.length;
			m_valid = true;
			break;
		} else {
			m_file.seekg(chunk_head.length, std::ios::cur);
			LE("Unknown chunk found. Chunk size: " + std::to_string(chunk_head.length));
		}
	}

	LD("Sound file processed");



}

WaveFile::~WaveFile() {
}

bool WaveFile::isValid() const {
	return m_valid;
}

size_t WaveFile::getAvailable() const {
	return m_size - m_pointer;
}

char* WaveFile::getData(size_t size) {
	char* ret = m_buff.get() + m_pointer;
	m_pointer += size;
	if (m_pointer >= m_size) {
		ret = nullptr;
		m_pointer = m_size;
	}
	return ret;
}

void WaveFile::rewind() {
	m_pointer = 0;
}

unsigned short WaveFile::getBitsPerSample() const {
	return m_bits_per_sample;
}

unsigned short WaveFile::getChannels() const {
	return m_channels;
}

unsigned int WaveFile::getSamplesPerSec() const {
	return m_samples_per_sec;
}

snd_pcm_format_t WaveFile::getFormat() const {
	return m_format;
}

snd_pcm_format_t WaveFile::encodeFormat(unsigned short bits_per_sample) {
	switch (bits_per_sample) {
	case 8:
		return SND_PCM_FORMAT_U8;
	case 16:
		return SND_PCM_FORMAT_S16;
	case 24:
		return SND_PCM_FORMAT_S24;
	case 32:
		return SND_PCM_FORMAT_S32;
	}
	return SND_PCM_FORMAT_UNKNOWN;
}
