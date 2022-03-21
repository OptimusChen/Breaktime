#pragma once

#include "main.hpp"

#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/Color.hpp"

using namespace UnityEngine;

DECLARE_CONFIG(PluginConfig,
               CONFIG_VALUE(Enabled, bool, "Mod Enabled", true);
               CONFIG_VALUE(minTime, float, "Minimum Time for a break", 4.0f);
               CONFIG_VALUE(SoundEnabled, bool, "Sound Enabled", true);
               CONFIG_VALUE(ShowText, bool, "Text Enabled", true);
               CONFIG_VALUE(ImageOpacity, float, "Image Opacity", 1.0f);
               CONFIG_VALUE(ScaleX, float, "Image ScaleX", 1.0f);
               CONFIG_VALUE(ScaleY, float, "Image ScaleY", 1.0f);
               CONFIG_VALUE(TextColor, Color, "Text Color", Color(1, 1, 1, 1));
               CONFIG_VALUE(ImageColor, Color, "Image Color", Color(1, 1, 1, 1));
               CONFIG_VALUE(ImagePath, std::string, "Image Path", "/sdcard/ModData/com.beatgames.beatsaber/Mods/BreakTime/Icons/check.png");
               CONFIG_VALUE(SoundPath, std::string, "Sound Path", "/sdcard/ModData/com.beatgames.beatsaber/Mods/BreakTime/Sounds/sound.ogg");
               CONFIG_INIT_FUNCTION(CONFIG_INIT_VALUE(Enabled);
                                    CONFIG_INIT_VALUE(minTime);
                                    CONFIG_INIT_VALUE(SoundEnabled);
                                    CONFIG_INIT_VALUE(ShowText);
                                    CONFIG_INIT_VALUE(ImageOpacity);
                                    CONFIG_INIT_VALUE(TextColor);
                                    CONFIG_INIT_VALUE(ImageColor);
                                    CONFIG_INIT_VALUE(ImagePath);
                                    CONFIG_INIT_VALUE(SoundPath);
                                    CONFIG_INIT_VALUE(ScaleX);
                                    CONFIG_INIT_VALUE(ScaleY);))