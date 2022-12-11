#include "SoundUI.h"
#include "imgui/imgui.h"
#include "imgui/IconsFontaudio.h"
#include "imgui/imgui-knobs.h"

void SoundUI::render() {
	// Create a window called "Audio Settings", with a menu bar.
	ImGui::Begin("Sound Engine Tools", &is_my_tool_active, ImGuiWindowFlags_MenuBar );

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New game", "N")) {
                menuState = NEWGAME;
            }
            if (ImGui::MenuItem("Load game", "Ctrl+L")) {
                menuState = LOAD;
            }
            if (ImGui::MenuItem("Save game", "Ctrl+K")) {
                menuState = SAVE;
            }
            if (ImGui::MenuItem("Close", "")) { 
                is_my_tool_active = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    static const char* current_item_music;
    static const char* current_item_fx;
    std::vector<const char*> musics;
    std::vector<const char*> fx;

    for (itSounds = m_fmod_manager->m_sounds.begin(); itSounds != m_fmod_manager->m_sounds.end(); itSounds++) {
        if (itSounds->second->m_type == "music")
            musics.push_back(itSounds->first.c_str());
        if (itSounds->second->m_type == "fx")
            fx.push_back(itSounds->first.c_str());
    }

    static std::map<std::string, ChannelGroup*>::iterator itChannels;
    for (itChannels = m_fmod_manager->m_channel_groups.begin(); itChannels != m_fmod_manager->m_channel_groups.end(); itChannels++) {
        std::string channelName = itChannels->first;
        m_channel = itChannels->second;
        if (ImGui::BeginTabBar(channelName.c_str())) {
            // -------
            // CHANNEL
            // -------
            if (ImGui::BeginTabItem(channelName.c_str())) {
                if (channelName == "ch1 music" || channelName == "ch2 fx") {
                    if (channelName == "ch1 music") {
                        ImGui::Text("Musics Available:"); 
                        ImGui::SameLine();
                        if (ImGui::BeginCombo("##combo", current_item_music)) {
                            for (int i = 0; i < musics.size(); i++) {
                                bool is_selected = (current_item_music == musics[i]);
                                if (ImGui::Selectable(musics[i], is_selected))
                                    current_item_music = musics[i];
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    } else {
                        ImGui::Text("FXs Available:");
                        ImGui::SameLine();
                        if (ImGui::BeginCombo("##combo", current_item_fx)) {
                            for (int i = 0; i < fx.size(); i++) {
                                bool is_selected = (current_item_fx == fx[i]);
                                if (ImGui::Selectable(fx[i], is_selected))
                                    current_item_fx = fx[i];
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    }
                } else {
                    ImGui::Text("Master Channel Menu:");
                    ImGui::SameLine();
                }
                ImGui::SameLine();
                if (ImGui::Button(ICON_FAD_PLAY)) { // Play Button
                    if (m_channel->m_isPaused) {
                        m_fmod_manager->setPause(channelName, false);
                        m_channel->m_isPaused = false;
                    } else {
                        if (channelName == "ch1 music") {
                            m_fmod_manager->playSound(current_item_music, channelName);
                        } else if (channelName == "ch2 fx") {
                            m_fmod_manager->playSound(current_item_fx, channelName);
                        }
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(ICON_FAD_PAUSE)) { // Pause Button
                    m_fmod_manager->setPause(channelName, true);
                    m_channel->m_isPaused = true;
                }
                ImGui::SameLine();
                if (ImGui::Button(ICON_FAD_STOP)) { // Stop Button
                    m_fmod_manager->stopSound(channelName);
                    m_channel->m_isPaused = false;
                }
                ImGui::SameLine();
                if (m_channel->m_isMuted) {
                    if (ImGui::Button(ICON_FAD_MUTE)) { // UnMute Button
                        m_channel->m_isMuted = false;
                        m_fmod_manager->setChannelGroupMuteStatus(channelName, m_channel->m_isMuted);
                    }
                } else {
                    if (ImGui::Button(ICON_FAD_MUTE)) { // Mute Button
                        m_channel->m_isMuted = true;
                        m_fmod_manager->setChannelGroupMuteStatus(channelName, m_channel->m_isMuted);
                    }
                }
                // Table with information about the selected sound CH1
                if (channelName == "ch1 music" && current_item_music != nullptr) {
                    ImGui::BeginTable("Music Data", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersOuter);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    for (itSounds = m_fmod_manager->m_sounds.begin(); itSounds != m_fmod_manager->m_sounds.end(); itSounds++) {
                        if (itSounds->first == current_item_music) {
                            ImGui::Text("Path: %s", itSounds->second->m_path.c_str());
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn();
                            ImGui::Text("Format: %s", itSounds->second->m_format.c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text("                       ");
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Volume: %d%%", (int)itSounds->second->m_volume*100);
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn();
                            ImGui::Text("Balance: %0.1f", itSounds->second->m_balance);
                            ImGui::TableNextColumn();
                            if (ImGui::SliderFloat("##Balance", &itSounds->second->m_balance, -1.0f, 1.0f)) {
                                m_fmod_manager->setChannelGroupPan(channelName, itSounds->second->m_balance); // Sets new pan
                            }
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            m_fmod_manager->getSoundLength(itSounds->first);
                            ImGui::Text("Lenght: %d:%d", (int)itSounds->second->m_lenght / 1000 / 60, (int)itSounds->second->m_lenght / 1000 % 60);
                            ImGui::TableNextColumn();
                            m_fmod_manager->getSoundCurrentPosition(itSounds->first);
                            ImGui::Text("Position: %d:%d", (int)itSounds->second->m_cur_position / 1000 / 60, (int)itSounds->second->m_cur_position / 1000 % 60);
                            ImGui::TableNextColumn();
                            m_fmod_manager->getSoundCurrentFrequency(itSounds->first);
                            ImGui::Text("Frequency: %0.1f", itSounds->second->m_frequency);
                            ImGui::TableNextColumn();
                            // Frequency Knob
                            if (ImGui::SliderFloat("##Frequency", &itSounds->second->m_frequency, -10000.0f, 48000.0f)) {
                                m_fmod_manager->setSoundCurrentFrequency(itSounds->first, itSounds->second->m_frequency); // Sets new pan
                            }
                        }
                    }
                    ImGui::EndTable();
                }
                // Table with information about the selected sound CH2
                if (channelName == "ch2 fx" && current_item_fx != nullptr) {
                    ImGui::BeginTable("Fx Data", 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersOuter);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    for (itSounds = m_fmod_manager->m_sounds.begin(); itSounds != m_fmod_manager->m_sounds.end(); itSounds++) {
                        if (itSounds->first == current_item_fx) {
                            ImGui::Text("Path: %s", itSounds->second->m_path.c_str());
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn();
                            ImGui::Text("Format: %s", itSounds->second->m_format.c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text("                       ");
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("Volume: %d%%", (int)itSounds->second->m_volume * 100);
                            ImGui::TableNextColumn();
                            ImGui::TableNextColumn();
                            ImGui::Text("Balance: %0.1f", itSounds->second->m_balance);
                            ImGui::TableNextColumn();
                            if (ImGui::SliderFloat("##Balance", &itSounds->second->m_balance, -1.0f, 1.0f)) {
                                m_fmod_manager->setChannelGroupPan(channelName, itSounds->second->m_balance); // Sets new pan
                            }
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            m_fmod_manager->getSoundLength(itSounds->first);
                            ImGui::Text("Lenght: %d:%d", (int)itSounds->second->m_lenght / 1000 / 60, (int)itSounds->second->m_lenght / 1000 % 60);
                            ImGui::TableNextColumn();
                            m_fmod_manager->getSoundCurrentPosition(itSounds->first);
                            ImGui::Text("Position: %d:%d", (int)itSounds->second->m_cur_position / 1000 / 60, (int)itSounds->second->m_cur_position / 1000 % 60);
                            ImGui::TableNextColumn();
                            m_fmod_manager->getSoundCurrentFrequency(itSounds->first);
                            ImGui::Text("Frequency: %0.1f", itSounds->second->m_frequency);
                            ImGui::TableNextColumn();
                            if (ImGui::SliderFloat("##Frequency", &itSounds->second->m_frequency, -10000.0f, 48000.0f)) {
                                m_fmod_manager->setSoundCurrentFrequency(itSounds->first, itSounds->second->m_frequency); // Sets new pan
                            }
                        }
                    }
                    ImGui::EndTable();
                }

                // ***********************
                // CHANNEL DSP TABLE START
                // ***********************
                if (ImGui::BeginTable(channelName.c_str(), 15, ImGuiTableFlags_SizingFixedFit)) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    // Volume Knob
                    //m_fmod_manager->getChannelGroupVolume(channelName, &m_channel->m_volume); // Gets the volume
                    m_channel->m_volume *= 100; // Conversion 0% to 100%
                    if (ImGuiKnobs::Knob("Volume", &m_channel->m_volume, 0.0f, 100.0f, 1.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_channel->m_volume /= 100; // Back to the original float value
                        if (m_channel->m_volume > 1.0f) { // Safe Guard to avoid hardware issues
                            m_channel->m_volume = 1.0f;
                        }
                        m_fmod_manager->setChannelGroupVolume(channelName, m_channel->m_volume); // Sets new volume
                    } else {
                        m_channel->m_volume /= 100; // Back to the original float value
                    }
                    ImGui::TableNextColumn();
                    // Pan Knob
                    if (ImGuiKnobs::Knob("Pan", &m_channel->m_pan, -1.0f, 1.0f, 0.01f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setChannelGroupPan(channelName, m_channel->m_pan); // Sets new pan
                    }
                    ImGui::TableNextColumn();
                    // Pitch Knob
                    if (ImGuiKnobs::Knob("Pitch", &m_channel->m_pitch, 0.5f, 2.0f, 0.01f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setChannelGroupPitch(channelName, m_channel->m_pitch); // Sets new pan
                    }
                    // DSP CHORUS
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Chorus", &m_channel->m_dspChorus)) {
                        if (m_channel->m_dspChorus) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_CHORUS);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_CHORUS);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_CHORUS, FMOD_DSP_CHORUS_MIX, &m_channel->m_chorusMix); // Gets the chorus mix
                    if (ImGuiKnobs::Knob("Chorus Mix", &m_channel->m_chorusMix, 0.0f, 100.0f, 1.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_CHORUS, FMOD_DSP_CHORUS_MIX, m_channel->m_chorusMix); // Sets new chorus mix
                    }
                    // DSP COMPRESSOR
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Compressor", &m_channel->m_dspCompressor)) {
                        if (m_channel->m_dspCompressor) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_COMPRESSOR);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_COMPRESSOR);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_COMPRESSOR, FMOD_DSP_COMPRESSOR_THRESHOLD, &m_channel->m_compressorThreshold); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Threshold", &m_channel->m_compressorThreshold, -60.0f, 0.0f, 0.5f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_COMPRESSOR, FMOD_DSP_COMPRESSOR_THRESHOLD, m_channel->m_compressorThreshold); // Sets new echo delay
                    }
                    // DSP DELAY
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Delay", &m_channel->m_dspDelay)) {
                        if (m_channel->m_dspDelay) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_DELAY);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_DELAY);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_DELAY, FMOD_DSP_DELAY_MAXDELAY, &m_channel->m_delayMax); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Max", &m_channel->m_delayMax, 0.0f, 10000.0f, 10.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_DELAY, FMOD_DSP_DELAY_MAXDELAY, m_channel->m_delayMax); // Sets new echo delay
                    }
                    // DSP DISTORTION
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Distortion", &m_channel->m_dspDistortion)) {
                        if (m_channel->m_dspDistortion) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_DISTORTION);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_DISTORTION);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_DISTORTION, FMOD_DSP_DISTORTION_LEVEL, &m_channel->m_distortionLevel); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Level", &m_channel->m_distortionLevel, 0.0f, 1.0f, 0.01f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_DISTORTION, FMOD_DSP_DISTORTION_LEVEL, m_channel->m_distortionLevel); // Sets new echo delay
                    }
                    // DSP ECHO
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Echo", &m_channel->m_dspEcho)) {
                        if (m_channel->m_dspEcho) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_ECHO);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_ECHO);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_ECHO, FMOD_DSP_ECHO_DELAY, &m_channel->m_echoDelay); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Delay", &m_channel->m_echoDelay, 1.0f, 5000.0f, 10.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_ECHO, FMOD_DSP_ECHO_DELAY, m_channel->m_echoDelay); // Sets new echo delay
                    }
                    // DSP FADER
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Fader", &m_channel->m_dspFader)) {
                        if (m_channel->m_dspFader) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_FADER);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_FADER);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_FADER, FMOD_DSP_FADER_GAIN, &m_channel->m_faderGain); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Gain", &m_channel->m_faderGain, -80.0f, 10.0f, 1.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_FADER, FMOD_DSP_FADER_GAIN, m_channel->m_faderGain); // Sets new echo delay
                    }
                    // DSP FLANGE
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Flange", &m_channel->m_dspFlange)) {
                        if (m_channel->m_dspFlange) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_FLANGE);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_FLANGE);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_FLANGE, FMOD_DSP_FLANGE_MIX, &m_channel->m_flangeMix); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Flange Mix", &m_channel->m_flangeMix, 0.0f, 100.0f, 1.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_FLANGE, FMOD_DSP_FLANGE_MIX, m_channel->m_flangeMix); // Sets new echo delay
                    }
                    // DSP NORMALIZE
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Normalize", &m_channel->m_dspNormalize)) {
                        if (m_channel->m_dspNormalize) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_NORMALIZE);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_NORMALIZE);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_NORMALIZE, FMOD_DSP_NORMALIZE_FADETIME, &m_channel->m_normalizeFadetime); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Fade Time", &m_channel->m_normalizeFadetime, 0.0f, 20000.0f, 10.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_NORMALIZE, FMOD_DSP_NORMALIZE_FADETIME, m_channel->m_normalizeFadetime); // Sets new echo delay
                    }
                    // DSP OSCILLATOR
                    // I will be commenting this DSP for safty reasons
                    // It really scared me and I don't know if it could harm your hardware
                    // Uncomment it on your own
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Oscillator", &m_channel->m_dspOscillator)) {
                        if (m_channel->m_dspOscillator) {
                            //m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_OSCILLATOR);
                        } else {
                            //m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_OSCILLATOR);
                        }
                    }
                    m_fmod_manager->getIntParameterDSP(FMOD_DSP_TYPE_OSCILLATOR, FMOD_DSP_OSCILLATOR_TYPE, &m_channel->m_oscillatorType); // Gets the echo delay
                    if (ImGuiKnobs::KnobInt("Type", &m_channel->m_oscillatorType, 0, 5, 0.1f, "%d", ImGuiKnobVariant_Stepped)) {
                        m_fmod_manager->setIntParameterDSP(FMOD_DSP_TYPE_OSCILLATOR, FMOD_DSP_OSCILLATOR_TYPE, m_channel->m_oscillatorType); // Sets new echo delay
                    }
                    // DSP SFXREVERB
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("SFX Reverb", &m_channel->m_dspSFXReverb)) {
                        if (m_channel->m_dspSFXReverb) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_SFXREVERB);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_SFXREVERB);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_SFXREVERB, FMOD_DSP_NORMALIZE_FADETIME, &m_channel->m_reverbDecay); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Decay", &m_channel->m_reverbDecay, 100.0f, 20000.0f, 100.0f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_SFXREVERB, FMOD_DSP_NORMALIZE_FADETIME, m_channel->m_reverbDecay); // Sets new echo delay
                    }
                    // DSP TREMOLO
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("Tremolo", &m_channel->m_dspTremolo)) {
                        if (m_channel->m_dspTremolo) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_TREMOLO);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_TREMOLO);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_TREMOLO, FMOD_DSP_TREMOLO_FREQUENCY, &m_channel->m_tremoloFrequency); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Frequency", &m_channel->m_tremoloFrequency, 0.1f, 20.0f, 0.1f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_TREMOLO, FMOD_DSP_TREMOLO_FREQUENCY, m_channel->m_tremoloFrequency); // Sets new echo delay
                    }
                    // DSP PITCHSHIFT
                    ImGui::TableNextColumn();
                    if (ImGui::Checkbox("PitchShift", &m_channel->m_dspPitchshift)) {
                        if (m_channel->m_dspPitchshift) {
                            m_fmod_manager->addDSPEffect(channelName, FMOD_DSP_TYPE_PITCHSHIFT);
                        } else {
                            m_fmod_manager->removeDSPEffect(channelName, FMOD_DSP_TYPE_PITCHSHIFT);
                        }
                    }
                    m_fmod_manager->getFloatParameterDSP(FMOD_DSP_TYPE_PITCHSHIFT, FMOD_DSP_PITCHSHIFT_PITCH, &m_channel->m_pitchshiftPitch); // Gets the echo delay
                    if (ImGuiKnobs::Knob("Shift", &m_channel->m_pitchshiftPitch, 0.5f, 2.0f, 0.01f, "%.1f%", ImGuiKnobVariant_WiperDot)) {
                        m_fmod_manager->setFloatParameterDSP(FMOD_DSP_TYPE_PITCHSHIFT, FMOD_DSP_PITCHSHIFT_PITCH, m_channel->m_pitchshiftPitch); // Sets new echo delay
                    }
                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();            
        }
    }
    // TODO: Future development TabBar for DSP with each choosen DSP with its own tab
    // Inside each tab the user can modify each attribute from the DSP
    /*ImGui::BeginTabBar("DSP Effects");
    std::map<FMOD_DSP_TYPE, FMOD::DSP*>::iterator itDSP;
    for (itDSP = m_fmod_manager->m_dsp.begin(); itDSP != m_fmod_manager->m_dsp.end(); itDSP++) {
        if (ImGui::BeginTabItem(std::to_string(itDSP->first).c_str())) {
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();*/
    ImGui::End();
}
