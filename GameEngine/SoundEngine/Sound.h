#pragma once
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>
#include <string>

class Sound {
public:
	FMOD::Sound*	m_sound;
	FMOD::Channel*	m_channel;
	std::string		m_name;
	std::string		m_path;
	std::string		m_format;
	std::string		m_type;
	float			m_frequency;
	float			m_volume;
	float			m_balance;
	unsigned int	m_lenght;
	unsigned int	m_cur_position;

	// Default Constructor
	Sound();
	// Destructor
	~Sound();
};

