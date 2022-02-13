#pragma once

#include "TMPro/TextMeshProUGUI.hpp"
#include "questui/shared/CustomTypes/Components/FloatingScreen/FloatingScreen.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "HMUI/ImageView.hpp"
#include "Breaktime/BreaktimeManager.hpp"

using namespace custom_types;
using namespace UnityEngine;
using namespace QuestUI;
using namespace TMPro;
using namespace HMUI;

namespace Breaktime {
    class BreaktimeDummy{
        public:
            BreaktimeDummy();  
            void SpawnDummy();
        private:
            TextMeshProUGUI* text;
            GameObject* screen;
            ImageView* image;
            void CreateScreen();
            Helpers::Coroutine PlaySound();
    };
}