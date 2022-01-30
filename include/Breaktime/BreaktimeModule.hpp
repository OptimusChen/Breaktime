#pragma once

#include "TMPro/TextMeshProUGUI.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "HMUI/ImageView.hpp"
#include "Breaktime/BreaktimeManager.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/util.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

DECLARE_CLASS_CODEGEN(Breaktime, BreaktimeModule, MonoBehaviour,
    DECLARE_INSTANCE_FIELD(bool, breakHappening);
    DECLARE_INSTANCE_FIELD(GameObject*, screen);
    DECLARE_INSTANCE_FIELD(AudioSource*, audioSource);
    DECLARE_INSTANCE_FIELD(AudioTimeSyncController*, audioController);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, image);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, text);
    DECLARE_INSTANCE_METHOD(void, ModifyVisuals);
    DECLARE_INSTANCE_METHOD(void, CreateScreen);
    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, BreakDetected, float);
    public:
    BreaktimeManager* manager;
    std::string timeText;
    void ctor(BreaktimeManager* manager, AudioTimeSyncController* controller);
    custom_types::Helpers::Coroutine StartBreak(float f);
    custom_types::Helpers::Coroutine UpdateText(float f);
    custom_types::Helpers::Coroutine HandleBreak(float f, std::pair<Sprite*, AudioClip*> pair);
    void SetupVisuals(std::pair<Sprite*, AudioClip*> pair);
)
    
