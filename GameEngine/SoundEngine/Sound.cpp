#include "Sound.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

Sound::Sound()
	: m_sound(nullptr),
	m_channel(nullptr),
	m_name(""),
	m_path(""),
	m_format(""),
	m_type(""),
	m_frequency(1.0f),
	m_volume(0.5f),
	m_balance(0.0f),
	m_lenght(0),
	m_cur_position(0) {
	DEBUG_PRINT("Sound::Sound(%s)\n", m_name.c_str());
}

Sound::~Sound() {
	DEBUG_PRINT("Sound::~Sound(%s)\n", m_name.c_str());
	m_sound->release();
}
