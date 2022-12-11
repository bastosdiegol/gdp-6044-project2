#include "ChannelGroup.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

ChannelGroup::ChannelGroup(FMOD::ChannelGroup* channel_group) {
	DEBUG_PRINT("ChannelGroup::ChannelGroup(FMOD::ChannelGroup*)\n");
	m_group = channel_group;
	m_volume= 1.0f;
	m_pan	= 0.0f;
	m_pitch = 1.0f;
	m_chorusMix = 0.0f;
	m_compressorThreshold = 0.0f;
	m_delayMax = 10.0f;
	m_distortionLevel = 0.5f;
	m_echoDelay = 1.0f;
	m_faderGain = 0.0f;
	m_flangeMix = 50.f;
	m_normalizeFadetime = 5000.0f;
	m_oscillatorType = 0;
	m_reverbDecay = 1500.0f;
	m_tremoloFrequency = 5.0f;
	m_pitchshiftPitch = 1.0f;
	m_isMuted = false;
	m_isPaused = false;
	m_dspChorus = false;
	m_dspCompressor = false;
	m_dspDelay = false;
	m_dspDistortion = false;
	m_dspEcho = false;
	m_dspFader = false;
	m_dspFlange = false;
	m_dspNormalize = false;
	m_dspOscillator = false;
	m_dspSFXReverb = false;
	m_dspTremolo = false;
	m_dspPitchshift = false;
}

ChannelGroup::~ChannelGroup() {
	DEBUG_PRINT("ChannelGroup::~ChannelGroup()\n");
	m_group->release();
}
