#pragma once

#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/NoteLineLayer.hpp"
#include "GlobalNamespace/BeatmapObjectData.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/BeatmapObjectManager_NoteWasCutDelegate.hpp"
#include "System/Action_1.hpp"
#include "map"

using namespace GlobalNamespace;

namespace Breaktime {
    class BreaktimeManager {
        public:
            BreaktimeManager(IDifficultyBeatmap* map);
            ~BreaktimeManager();
            void Initialize();
            void NoteCut(NoteController* noteController);
            void NoteEnded(NoteController* noteController);
            std::function<void(float)> breakDetected;
            IDifficultyBeatmap* difficultyBeatmap;
            BeatmapObjectManager* beatmapObjectManager;
            std::map<std::tuple<NoteLineLayer, int, float>, BeatmapObjectData*>* breaks;
        private:
            void Ctor(IDifficultyBeatmap* difficultyBeatmap);
    };
}