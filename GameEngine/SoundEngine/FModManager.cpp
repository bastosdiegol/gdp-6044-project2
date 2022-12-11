#include <iostream>
#include <pugixml/pugixml.hpp>

#include "FModManager.h"

#ifdef _DEBUG
#define DEBUG_LOG_ENABLED
#endif
#include <glm/geometric.hpp>
#ifdef DEBUG_LOG_ENABLED
#define DEBUG_PRINT(x, ...) printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#endif

FModManager::FModManager(const int system_flags, bool uncompressedFiles) {
	DEBUG_PRINT("FModManager::FModManager(%d, %d)\n", system_flags, uncompressedFiles);
	// Sets if our FMOD will be using uncompressed music sound files
	m_isUsingUncompressedSound = uncompressedFiles;
	// Creates the System
	m_result = System_Create(&m_system);
	// Checks for errors
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}

	// Initialize the System
	m_result = m_system->init(MAX_CHANNELS, system_flags, nullptr);
	// Checks for errors
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

FModManager::~FModManager() {
	DEBUG_PRINT("FModManager::~FModManager()\n");

	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP;
	// Iterates trought map tree of sounds and call each destructor
	for (itDSP = m_dsp.begin(); itDSP != m_dsp.end(); itDSP++) {
		DEBUG_PRINT("FModManager::~DSPEffect(%d)\n", (int)itDSP->first);
		itDSP->second->release();
	}

	std::map<std::string, Sound*>::iterator itSounds;
	// Iterates trought map tree of sounds and call each destructor
	for (itSounds = m_sounds.begin(); itSounds != m_sounds.end(); itSounds++)
		itSounds->second->~Sound();

	std::map<std::string, ChannelGroup*>::iterator itChannels;
	// Iterates trought map tree of channels and call each destructor
	for (itChannels = m_channel_groups.begin(); itChannels != m_channel_groups.end(); itChannels++)
		itChannels->second->~ChannelGroup();

	// Clear the map tree
	m_dsp.clear();
	m_sounds.clear();
	m_channel_groups.clear();
	// Deletes the FMOD System
	m_system->release();
}

void FModManager::createChannelGroup(const std::string& name) {
	DEBUG_PRINT("FModManager::createChannelGroup(%s)\n", name.c_str());
	FMOD::ChannelGroup* fmod_channel_group;
	// Creates the FMOD Channel Group
	m_result = m_system->createChannelGroup(name.c_str(), &fmod_channel_group);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
	// Creates the Class Channel Group and Inserts the FMOD in
	ChannelGroup* channel_group = new ChannelGroup(fmod_channel_group);
	// Adds the class instane into the member map tree
	m_channel_groups.try_emplace(name, channel_group);
}

void FModManager::removeChannelGroup(const std::string& name) {
	DEBUG_PRINT("FModManager::removeChannelGroup(%s)\n", name.c_str());
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Release the FMOD Channel Group
	it->second->~ChannelGroup();
	// Removes the class instance from the member map tree
	m_channel_groups.erase(it);
}

void FModManager::setChannelGroupParent(const std::string& parent_name, const std::string& child_name) {
	DEBUG_PRINT("FModManager::setChannelGroupParent(%s, %s)\n", parent_name.c_str(), child_name.c_str());
	// Tries to find both Parent and Child channel groups
	std::map<std::string, ChannelGroup*>::iterator itParent = m_channel_groups.find(parent_name);
	std::map<std::string, ChannelGroup*>::iterator itChild  = m_channel_groups.find(child_name);
	// Checks if both parent and child was found
	if (itParent == m_channel_groups.end() || itChild == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << parent_name 
													<< "or ChannelGroup named #" << child_name << std::endl;
		return;
	}
	// If found tries to estabelish the relation using FMOD
	m_result = itParent->second->m_group->addGroup(itChild->second->m_group);
	// Checks for error
	if (m_result != FMOD_OK) 
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
}

ChannelGroup* FModManager::getChannelGroup(const std::string& name) {
	DEBUG_PRINT("FModManager::getChannelGroup(%s)\n", name.c_str());
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return nullptr;
	}
	return it->second;
}

void FModManager::getChannelGroupVolume(const std::string& name, float* volume) {
	DEBUG_PRINT("FModManager::getChannelGroupVolume(%s)\n", name.c_str());
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Gets the pointer to the value of the volume
	it->second->m_group->getVolume(volume);
}

void FModManager::setChannelGroupVolume(const std::string& name, float volume) {
	DEBUG_PRINT("FModManager::setChannelGroupVolume(%s, %f)\n", name.c_str(), volume);
	if (volume > 1.0f) {
		std::cout << "WARNING: YOU TRIED TO SET A VOLUME HIGHER THAN 1.0f.\nTHE PROGRAM RESETED THE VOLUME TO 1.0f" << std::endl;
		volume = 1.0f;
	}
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Sets the new value for the volume
	m_result = it->second->m_group->setVolume(volume);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
	}
	it->second->m_volume = volume;
}

void FModManager::getChannelGroupMuteStatus(const std::string& name, bool* muteStatus) {
	DEBUG_PRINT("FModManager::getChannelGroupMuteStatus(%s)\n", name.c_str());
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Gets the pointer to the value of the mute status
	m_result = it->second->m_group->getMute(muteStatus);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		muteStatus = nullptr;
	}
}

void FModManager::setChannelGroupMuteStatus(const std::string& name, bool muteStatus) {
	DEBUG_PRINT("FModManager::setChannelGroupMuteStatus(%s, %d)\n", name.c_str(), muteStatus);
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Sets the new value for the volume
	m_result = it->second->m_group->setMute(muteStatus);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
	}
}

void FModManager::getChannelGroupPan(const std::string& name, float* pan) {
	DEBUG_PRINT("FModManager::getChannelGroupPan(%s)\n", name.c_str());
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Gets the pointer to the value of the pan
	*pan = it->second->m_pan;
}

void FModManager::setChannelGroupPan(const std::string& name, float pan) {
	DEBUG_PRINT("FModManager::setChannelGroupPan(%s, %f)\n", name.c_str(), pan);
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Sets the new value for the pan
	it->second->m_group->setPan(pan);
}

void FModManager::setChannelGroupPitch(const std::string& name, float pitch) {
	DEBUG_PRINT("FModManager::setChannelGroupPitch(%s, %f)\n", name.c_str(), pitch);
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Checks if it was found
	if (it == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name << std::endl;
		return;
	}
	// Sets the new value for the pitch
	it->second->m_group->setPitch(pitch);
}

void FModManager::createDSP(FMOD_DSP_TYPE dsp_type) {
	DEBUG_PRINT("FModManager::createDSP(%d)\n", dsp_type);
	FMOD::DSP* dsp;

	// Creates the dsp
	m_result = m_system->createDSPByType(dsp_type, &dsp);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
	// Here we gonna set the values for the DSP in comments
	// Also In comments the Range Expected and Default Values to facilitate future dsp updates
	// If I want to tweek any dsp value outside the default I'll just have to uncomment the line and update the value
	// https://fmod.com/docs/2.02/api/core-api-common-dsp-effects.html
	switch (dsp_type) {
	case FMOD_DSP_TYPE_CHORUS:
		dsp->setParameterFloat(FMOD_DSP_CHORUS_MIX,	50.0f);			// Range: [0, 100] Default: 50		Units: Percentage
//		dsp->setParameterFloat(FMOD_DSP_CHORUS_RATE, 0.8f);			// Range: [0,  20] Default: 0.8		Units: Hertz
//		dsp->setParameterFloat(FMOD_DSP_CHORUS_DEPTH, 3.0f);		// Range: [0, 100] Default: 3		Units: Milliseconds
		break;
	case FMOD_DSP_TYPE_COMPRESSOR:
//		dsp->setParameterFloat(FMOD_DSP_COMPRESSOR_THRESHOLD, 0.0f);// Range: [-60, 0] Default: 0		Units: Decibels
//		dsp->setParameterFloat(FMOD_DSP_COMPRESSOR_RATIO, 2.5f);	// Range: [1, 50] Default: 2.5		Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_COMPRESSOR_ATTACK, 20.0f);	// Range: [0.1, 500] Default: 20	Units: Milliseconds
//		dsp->setParameterFloat(FMOD_DSP_COMPRESSOR_GAINMAKEUP, 0.0f);//Range: [-30, 30] Default: 0		Units: Milliseconds
//		dsp->setParameterBool(FMOD_DSP_COMPRESSOR_LINKED, true);	// false = Independent(compressor per channel), true = Linked
		break;
	case FMOD_DSP_TYPE_DELAY:
		dsp->setParameterFloat(FMOD_DSP_DELAY_MAXDELAY, 10.0f);	// Range: [0, 10000] Default: 10	Units: Milliseconds
		dsp->setParameterFloat(FMOD_DSP_DELAY_CH0, 1000.0f);		// CH0 = MASTER (?)
		// FMOD_DSP_DELAY_CH0
		// ...														// Range: [0, 10000] Default: 0		Units: Milliseconds
		// FMOD_DSP_DELAY_CH15
		break;
	case FMOD_DSP_TYPE_DISTORTION:
//		dsp->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, 0.5f);	// Range: [0, 1] Default: 0.5		Units: Linear
		break;
	case FMOD_DSP_TYPE_ECHO:
		dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 500.0f);		// Range: [1, 5000] Default: 500	Units: Milliseconds
//		dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 50.0f);		// Range: [0, 100] Default: 50		Units: Percentage
//		dsp->setParameterFloat(FMOD_DSP_ECHO_DRYLEVEL, 0.0f);		// Range: [-80, 10] Default : 0		Units: Decibels
//		dsp->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL, 0.0f);		// Range: [-80, 10] Default: 0		Units: Decibels
		break;
	case FMOD_DSP_TYPE_FADER:
		dsp->setParameterFloat(FMOD_DSP_FADER_GAIN, 0.0f);			// Range: [-80, 10] Default: 0		Units: Decibels
		// There's also a data parameter FMOD_DSP_FADER_OVERALL_GAIN for visualization purposes
		break;
	case FMOD_DSP_TYPE_FLANGE:
		dsp->setParameterFloat(FMOD_DSP_FLANGE_MIX, 50.0f);			// Range: [0, 100] Default: 50		Units: Percentage
//		dsp->setParameterFloat(FMOD_DSP_FLANGE_DEPTH, 1.0f);		// Range: [0.01, 1] Default: 1		Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_FLANGE_RATE, 1.0f);			// Range: [0, 20] Default: .1		Units: Hertz
		break;
	case FMOD_DSP_TYPE_NORMALIZE:
		dsp->setParameterFloat(FMOD_DSP_NORMALIZE_FADETIME, 5000.0f);//Range: [0, 20000] Default: 5000	Units: Milliseconds
//		dsp->setParameterFloat(FMOD_DSP_NORMALIZE_THRESHOLD, 0.1f);	// Range: [0, 1] Default: 0.1		Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_NORMALIZE_MAXAMP, 20.0f);	// Range: [1, 100000] Default: 20	Units: Linear
		break;
	case FMOD_DSP_TYPE_OSCILLATOR:
		dsp->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, 0);			// Default 0 = sine. 1 = square. 2 = sawup. 3 = sawdown. 4 = triangle. 5 = noise.
//		dsp->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, 220);		// Range: [0, 22000] Default: 220	Units: Hertz
		break;
	case FMOD_DSP_TYPE_SFXREVERB:
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, 1500.0f);	//Range: [100, 20000] Default: 1500		Units: Milliseconds
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYDELAY, 20.0f);	//Range: [0, 300] Default: 20			Units: Milliseconds
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_LATEDELAY, 40.0f);	//Range: [0, 100] Default: 40			Units: Milliseconds
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_HFREFERENCE, 5000.0f);//Range: [20, 20000] Default: 5000		Units: Hertz
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_HFDECAYRATIO, 50.0f);	//Range: [10, 100] Default: 50			Units: Percent
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, 50.0f);	//Range: [10, 100] Default: 50			Units: Percent
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, 50.0f);		//Range: [10, 100] Default: 50			Units: Percent
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFFREQUENCY, 250.0f);//Range: [20, 1000] Default: 250	Units: Hertz
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_LOWSHELFGAIN, 0.0f);	//Range: [-36, 12] Default: 0			Units: Decibels
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_HIGHCUT, 20000.0f);	//Range: [20, 20000] Default: 20000		Units: Hertz
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_EARLYLATEMIX, 50.0f);	//Range: [0, 100] Default: 50			Units: Percent
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_WETLEVEL, -6.0f);		//Range: [-80, 20] Default: -6			Units: Decibels
//		dsp->setParameterFloat(FMOD_DSP_SFXREVERB_DRYLEVEL, 0.0f);		//Range: [-80, 20] Default: 0			Units: Decibels
		break;
	case FMOD_DSP_TYPE_TREMOLO:
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, 5.0f);	// Range: [0.1, 20] Default: 5		Units: Hertz
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_DEPTH, 1.0f);		// Range: [0, 1] Default: 1			Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_SHAPE, 1.0f);		// Range: [0, 1] Default: 1			Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.0f);		// Range: [-1, 1] Default: 0		Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_DUTY, 0.5f);		// Range: [0, 1] Default: 0.5		Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_SQUARE, 1.0f);		// Range: [0, 1] Default: 1			Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_PHASE, 0.0f);		// Range: [0, 1] Default: 0			Units: Linear
//		dsp->setParameterFloat(FMOD_DSP_TREMOLO_SPREAD, 0.0f);		// Range: [-1, 1] Default: 0		Units: Linear
		break;
	case FMOD_DSP_TYPE_PITCHSHIFT:
		dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 1.0f);	// Range: [0.5, 2] Default: 1		Type: float
//		dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 1024.0f); // FFT window size - 256, 512, 1024, 2048, 4096
//		dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_MAXCHANNELS, 0.0f);// Range: [0, 16]					Type: float
		break;
	}
	m_dsp.try_emplace(dsp_type, dsp);
}

void FModManager::loadDSPs() {
	DEBUG_PRINT("FModManager::loadDSPs()\n");

	createDSP(FMOD_DSP_TYPE_CHORUS);
	createDSP(FMOD_DSP_TYPE_COMPRESSOR);
	createDSP(FMOD_DSP_TYPE_DELAY);
	createDSP(FMOD_DSP_TYPE_DISTORTION);
	createDSP(FMOD_DSP_TYPE_ECHO);
	createDSP(FMOD_DSP_TYPE_FADER);
	createDSP(FMOD_DSP_TYPE_FLANGE);
	createDSP(FMOD_DSP_TYPE_NORMALIZE);
	createDSP(FMOD_DSP_TYPE_OSCILLATOR);
	createDSP(FMOD_DSP_TYPE_SFXREVERB);
	createDSP(FMOD_DSP_TYPE_TREMOLO);
	createDSP(FMOD_DSP_TYPE_PITCHSHIFT);
}

void FModManager::setFloatParameterDSP(FMOD_DSP_TYPE dsp_type, int fmdDspParameter, float value) {
	DEBUG_PRINT("FModManager::setFloatParameterDSP(%d, %d, %f)\n", dsp_type, fmdDspParameter, value);
	// Finds the DSP group by FMOD_DSP_TYPE
	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP = m_dsp.find(dsp_type);
	// Checks if it was found
	if (itDSP == m_dsp.end()) {
		std::cout << "FModManager error: Couldn't find the DSP type #" << dsp_type << std::endl;
		return;
	}
	m_result = itDSP->second->setParameterFloat(fmdDspParameter, value);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

void FModManager::setBoolParameterDSP(FMOD_DSP_TYPE dsp_type, int fmdDspParameter, bool value) {
	DEBUG_PRINT("FModManager::setBoolParameterDSP(%d, %d, %d)\n", dsp_type, fmdDspParameter, value);
	// Finds the DSP group by FMOD_DSP_TYPE
	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP = m_dsp.find(dsp_type);
	// Checks if it was found
	if (itDSP == m_dsp.end()) {
		std::cout << "FModManager error: Couldn't find the DSP type #" << dsp_type << std::endl;
		return;
	}
	m_result = itDSP->second->setParameterBool(fmdDspParameter, value);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

void FModManager::setIntParameterDSP(FMOD_DSP_TYPE dsp_type, int fmdDspParameter, int value) {
	DEBUG_PRINT("FModManager::setIntParameterDSP(%d, %d, %d)\n", dsp_type, fmdDspParameter, value);
	// Finds the DSP group by FMOD_DSP_TYPE
	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP = m_dsp.find(dsp_type);
	// Checks if it was found
	if (itDSP == m_dsp.end()) {
		std::cout << "FModManager error: Couldn't find the DSP type #" << dsp_type << std::endl;
		return;
	}
	m_result = itDSP->second->setParameterInt(fmdDspParameter, value);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

void FModManager::getFloatParameterDSP(FMOD_DSP_TYPE dsp_type, int fmdDspParameter, float* value) {
	//DEBUG_PRINT("FModManager::getFloatParameterDSP(%d, %d)\n", dsp_type, fmdDspParameter);
	// Finds the DSP group by FMOD_DSP_TYPE
	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP = m_dsp.find(dsp_type);
	// Checks if it was found
	if (itDSP == m_dsp.end()) {
		std::cout << "FModManager error: Couldn't find the DSP type #" << dsp_type << std::endl;
		return;
	}
	m_result = itDSP->second->getParameterFloat(fmdDspParameter, value, nullptr, 0);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

void FModManager::getIntParameterDSP(FMOD_DSP_TYPE dsp_type, int fmdDspParameter, int* value) {
	//DEBUG_PRINT("FModManager::getIntParameterDSP(%d, %d)\n", dsp_type, fmdDspParameter);
	// Finds the DSP group by FMOD_DSP_TYPE
	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP = m_dsp.find(dsp_type);
	// Checks if it was found
	if (itDSP == m_dsp.end()) {
		std::cout << "FModManager error: Couldn't find the DSP type #" << dsp_type << std::endl;
		return;
	}
	m_result = itDSP->second->getParameterInt(fmdDspParameter, value, nullptr, 0);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

void FModManager::addDSPEffect(const std::string& name, const FMOD_DSP_TYPE& dsp_type) {
	DEBUG_PRINT("FModManager::addDSPEffect(%s, %d)\n", name.c_str(), dsp_type);
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Finds the DSP by type
	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP = m_dsp.find(dsp_type);
	// Checks if it was found
	if (it == m_channel_groups.end() || itDSP == m_dsp.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name 
												     << "or DSP Type #" << dsp_type << std::endl;
		return;
	}
	int num_dsp;
	m_result = it->second->m_group->getNumDSPs(&num_dsp);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
	m_result = it->second->m_group->addDSP(num_dsp, itDSP->second);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

void FModManager::removeDSPEffect(const std::string& name, const FMOD_DSP_TYPE& dsp_type) {
	DEBUG_PRINT("FModManager::removeDSPEffect(%s, %d)\n", name.c_str(), dsp_type);
	// Finds the Channel group by Name
	std::map<std::string, ChannelGroup*>::iterator it = m_channel_groups.find(name);
	// Finds the DSP by type
	std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP = m_dsp.find(dsp_type);
	// Checks if it was found
	if (it == m_channel_groups.end() || itDSP == m_dsp.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << name
			<< "or DSP Type #" << dsp_type << std::endl;
		return;
	}
	m_result = it->second->m_group->removeDSP(itDSP->second);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}
}

void FModManager::loadSoundsFromFile(const std::string filePath) {
	DEBUG_PRINT("FModManager::loadSounds()\n");
	// Create a document object
	pugi::xml_document soundlibrary;
	// Load a xml file into the object
	pugi::xml_parse_result result = soundlibrary.load_file(filePath.c_str());
	if (!result) {
		std::cout << "FModManager error: Failed to load file named #" << filePath << std::endl;
		return;
	}
	DEBUG_PRINT("Successfully loaded file named #%s\n", filePath.c_str());

	// Gets all nodes of sound inside the soundlibrary
	pugi::xml_node sounds = soundlibrary.child("soundlibrary");
	Sound* newSound;

	// Iterates through each node
	for (pugi::xml_node sound = sounds.child("sound"); sound; sound = sound.next_sibling("sound")) {
		// Instantiate a new sound and set its values from XML
		newSound = new Sound();
		newSound->m_name		 = sound.attribute("title").value();
		newSound->m_path		 = sound.attribute("path").value();
		newSound->m_format		 = sound.attribute("format").value();
		newSound->m_type		 = sound.attribute("type").value();
		// Checks for music MP3 files while using uncompressed sounds
		// If found skip them
		if ((m_isUsingUncompressedSound == true) && newSound->m_type == "music" && newSound->m_format == "mp3") {
			newSound->~Sound();
			continue;
		}
		// Checks for music WAV files while using compressed sounds
		// If found skip them
		if ((m_isUsingUncompressedSound == false) && newSound->m_type == "music" && newSound->m_format == "wav") {
			newSound->~Sound();
			continue;
		}
		newSound->m_frequency	 = sound.attribute("frequency").as_float();
		newSound->m_volume		 = sound.attribute("volume").as_float();
		newSound->m_balance		 = sound.attribute("balance").as_float();
		//newSound->m_lenght	   = sound.attribute("lenght").as_uint();
		//newSound->m_cur_position = sound.attribute("position").as_uint();
		// Calls FMOD createSound according with the type of the sound
		if (newSound->m_type == "music") {
			// Calls the FMOD function to create the new sound with mode LOOP - BGM
			m_result = m_system->createSound(newSound->m_path.c_str(), FMOD_LOOP_NORMAL | FMOD_3D | FMOD_DEFAULT, nullptr, &newSound->m_sound);
		} else if (newSound->m_type == "fx") {
			// Calls the FMOD function to create the new sound with mode DEFAULT - FX
			m_result = m_system->createSound(newSound->m_path.c_str(), FMOD_DEFAULT, nullptr, &newSound->m_sound);
		}
		// Checks the result
		if (m_result != FMOD_OK) {
			std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
			newSound->~Sound(); // Deletes the instance
			continue;			// Continues the loop
		}
		// Adds the new sound to the map tree of sounds
		m_sounds.try_emplace(newSound->m_name, newSound);
		// Gets attributes from FMOD instead of XML
		getSoundCurrentPosition(newSound->m_name);
		getSoundLength(newSound->m_name);
		getSoundCurrentFrequency(newSound->m_name);
	}
}

void FModManager::playSound(const std::string& sound_name, const std::string& channel_group_name) {
	DEBUG_PRINT("FModManager::playSound(%s, %s)\n", sound_name.c_str(), channel_group_name.c_str());
	// Tries to find the sound
	std::map<std::string, Sound*>::iterator itSound = m_sounds.find(sound_name);
	// Tries to find the channel group
	std::map<std::string, ChannelGroup*>::iterator itChannel = m_channel_groups.find(channel_group_name);

	if (itSound == m_sounds.end() || itChannel == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the Sound named #" << sound_name
													<< " or ChannelGroup named #" << channel_group_name << std::endl;
		return;
	}

	m_result = m_system->playSound(itSound->second->m_sound, itChannel->second->m_group, true, &itSound->second->m_channel);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}

	m_result = itSound->second->m_channel->setPaused(false);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}

}

void FModManager::play3DSound(const std::string& sound_name, glm::vec3 position, float max_distance) {
	DEBUG_PRINT("FModManager::playSound(%s)\n", sound_name.c_str());

	// Tries to find the sound
	std::map<std::string, Sound*>::iterator itSound = m_sounds.find(sound_name);
	if (itSound == m_sounds.end() ) {
		std::cout << "FModManager error: Couldn't find the Sound named #" << sound_name << std::endl;
		return;
	}

	m_result = m_system->playSound(itSound->second->m_sound, nullptr, true, &itSound->second->m_channel);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}

	FMOD_VECTOR fmod_sound_position;
	fmod_sound_position.x = position.x;
	fmod_sound_position.y = position.y;
	fmod_sound_position.z = position.z;

	m_result = itSound->second->m_channel->set3DAttributes(&fmod_sound_position, nullptr);
	// Checks the result
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}

	m_result = itSound->second->m_channel->set3DMinMaxDistance(max_distance, FLT_MAX);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}

	m_result = itSound->second->m_channel->setPaused(false);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return;
	}

}

void FModManager::stopSound(const std::string& channel_group_name) {
	DEBUG_PRINT("FModManager::stopSound(%s)\n", channel_group_name.c_str());
	// Tries to find the channel group
	std::map<std::string, ChannelGroup*>::iterator itChannel = m_channel_groups.find(channel_group_name);

	if (itChannel == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << channel_group_name << std::endl;
		return;
	}

	std::map<std::string, Sound*>::iterator itSound;
	for (itSound = m_sounds.begin(); itSound != m_sounds.end(); itSound++) {
		if (channel_group_name.find(itSound->second->m_type) != -1) {
			itSound->second->m_channel = nullptr;
		}
	}

	itChannel->second->m_group->stop();
}

void FModManager::setPause(const std::string& channel_group_name, const bool pause) {
	DEBUG_PRINT("FModManager::setPause(%s, %d)\n", channel_group_name.c_str(), pause);
	// Tries to find the channel group
	std::map<std::string, ChannelGroup*>::iterator itChannel = m_channel_groups.find(channel_group_name);

	if (itChannel == m_channel_groups.end()) {
		std::cout << "FModManager error: Couldn't find the ChannelGroup named #" << channel_group_name << std::endl;
		return;
	}

	itChannel->second->m_group->setPaused(pause);
}

void FModManager::getSoundCurrentPosition(const std::string& sound_name) {
	//DEBUG_PRINT("FModManager::getSoundCurrentPosition(%s)\n", sound_name.c_str());
	// Tries to find the sound
	std::map<std::string, Sound*>::iterator itSound = m_sounds.find(sound_name);

	if (itSound == m_sounds.end()) {
		std::cout << "FModManager error: Couldn't find the Sound named #" << sound_name  << std::endl;
		return;
	}

	if (itSound->second->m_channel != nullptr) {
		m_result = itSound->second->m_channel->getPosition(&itSound->second->m_cur_position, FMOD_TIMEUNIT_MS);
		if (m_result != FMOD_OK) {
			std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
			return;
		}
	}
}


void FModManager::setSoundCurrentFrequency(const std::string& sound_name, float frequency) {
	DEBUG_PRINT("FModManager::setSoundCurrentFrequency(%s)\n", sound_name.c_str());
	// Tries to find the sound
	std::map<std::string, Sound*>::iterator itSound = m_sounds.find(sound_name);

	if (itSound == m_sounds.end()) {
		std::cout << "FModManager error: Couldn't find the Sound named #" << sound_name << std::endl;
		return;
	}

	if (itSound->second->m_channel != nullptr) {
		m_result = itSound->second->m_channel->setFrequency(frequency);
		if (m_result != FMOD_OK) {
			std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
			return;
		}
	}
}

void FModManager::getSoundCurrentFrequency(const std::string& sound_name) {
	//DEBUG_PRINT("FModManager::getSoundCurrentFrequency(%s)\n", sound_name.c_str());
	// Tries to find the sound
	std::map<std::string, Sound*>::iterator itSound = m_sounds.find(sound_name);

	if (itSound == m_sounds.end()) {
		std::cout << "FModManager error: Couldn't find the Sound named #" << sound_name << std::endl;
		return;
	}

	if (itSound->second->m_channel != nullptr) {
		m_result = itSound->second->m_channel->getFrequency(&itSound->second->m_frequency);
		if (m_result != FMOD_OK) {
			std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
			return;
		}
	}
}

void FModManager::getSoundLength(const std::string& sound_name) {
	//DEBUG_PRINT("FModManager::getSoundLength(%s)\n", sound_name.c_str());
	std::map<std::string, Sound*>::iterator itSound = m_sounds.find(sound_name);

	if (itSound == m_sounds.end()) {
		std::cout << "FModManager error: Couldn't find the Sound named #" << sound_name << std::endl;
		return;
	}

	m_result = itSound->second->m_sound->getLength(&itSound->second->m_lenght, FMOD_TIMEUNIT_MS);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		itSound->second->m_lenght = 0;
		return;
	}
}

bool FModManager::tick(const glm::vec3& controllable_position) {

	FMOD_VECTOR fmod_controllable_position;
	fmod_controllable_position.x = controllable_position.x;
	fmod_controllable_position.y = controllable_position.y;
	fmod_controllable_position.z = controllable_position.z;

	FMOD_VECTOR fmod_controllable_foward;
	fmod_controllable_foward.x = 0.0f;
	fmod_controllable_foward.y = 0.0f;
	fmod_controllable_foward.z = -1.0f;

	FMOD_VECTOR upVector;
	upVector.x = 0.0f;
	upVector.y = 1.0f;
	upVector.z = 0.0f;

	m_result = m_system->set3DListenerAttributes(0, &fmod_controllable_position, nullptr, &fmod_controllable_foward, &upVector);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return false;
	}

	m_result = m_system->update();
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return false;
	}

	return m_result;
}

bool FModManager::update3DSoundPosition(FMOD::Channel* channel, const glm::vec3 position) {
	FMOD_VECTOR fmod_position;
	fmod_position.x = position.x;
	fmod_position.y = position.y;
	fmod_position.z = position.z;

	m_result = channel->set3DAttributes(&fmod_position, nullptr);
	if (m_result != FMOD_OK) {
		std::cout << "fmod error: #" << m_result << "-" << FMOD_ErrorString(m_result) << std::endl;
		return false;
	}

	return m_result;
}

Sound* FModManager::getSound(const std::string& sound_name) {
	// Tries to find the sound
	std::map<std::string, Sound*>::iterator itSound = m_sounds.find(sound_name);

	if (itSound == m_sounds.end()) {
		std::cout << "FModManager error: Couldn't find the Sound named #" << sound_name << std::endl;
		return nullptr;
	}

	return itSound->second;
}

bool FModManager::setUseXYZ() {
	//m_system->set3DSettings();
	//m_system->set3DListenerAttributes();
	return false;
}