#pragma once
#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>
#include <string>

class ChannelGroup {
public:
	// Overloaded Constructor
	// Receives pointer of a Channel Group
	ChannelGroup(FMOD::ChannelGroup* channel_group);
	// Destructor
	~ChannelGroup();
//private:
	FMOD::ChannelGroup* m_group; // FMOD Channel Group
    float m_volume;
	float m_pan;				// Pan variable
	float m_pitch;				// Pitch variablebool isMusicPaused;
    bool m_isMuted;
    bool m_isPaused;
    // Variables that store user input for DSP parameters
    // Later I want to mave it to a DSP Class of its own
    float m_chorusMix;
    float m_compressorThreshold;
    float m_delayMax;
    float m_distortionLevel;
    float m_echoDelay;
    float m_faderGain;
    float m_flangeMix;
    float m_normalizeFadetime;
    int   m_oscillatorType;
    float m_reverbDecay;
    float m_tremoloFrequency;
    float m_pitchshiftPitch;
    // Variables that defines if a DSP is being applying on this channel
    bool m_dspChorus;
    bool m_dspCompressor;
    bool m_dspDelay;
    bool m_dspDistortion;
    bool m_dspEcho;
    bool m_dspFader;
    bool m_dspFlange;
    bool m_dspNormalize;
    bool m_dspOscillator;
    bool m_dspSFXReverb;
    bool m_dspTremolo;
    bool m_dspPitchshift;

	friend class FModManager; // Allows FModManager to access the private variables above
    //friend class SoundUI;
};

