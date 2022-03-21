#pragma once

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/NoteLineLayer.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/BeatmapObjectManager_NoteWasCutDelegate.hpp"
#include "System/Action_1.hpp"
#include "map"

#include "custom-types/shared/coroutine.hpp"

using namespace GlobalNamespace;
using namespace custom_types;    

namespace Breaktime {
    class BreaktimeManager {
        public:
            BreaktimeManager(IDifficultyBeatmap* map);
            ~BreaktimeManager();
            Helpers::Coroutine Initialize();
            void NoteCut(NoteController* noteController);
            void NoteEnded(NoteController* noteController);
            std::function<void(float)> breakDetected;
            BeatmapLevelsModel* model;
            IDifficultyBeatmap* difficultyBeatmap;
            BeatmapObjectManager* beatmapObjectManager;
            std::map<std::tuple<NoteLineLayer, int, float>, BeatmapDataItem*>* breaks;
        private:
            void Ctor(IDifficultyBeatmap* difficultyBeatmap);
    };
}