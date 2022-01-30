#include "Breaktime/BreaktimeManager.hpp"
#include "System/Action_1.hpp"
#include "System/Func_2.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "System/Linq/Enumerable.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/BeatmapObjectType.hpp"
#include "../include/PluginConfig.hpp"

using namespace Breaktime;
using namespace System;    

BreaktimeManager::BreaktimeManager(IDifficultyBeatmap* map){
    Ctor(map);
}

BreaktimeManager::~BreaktimeManager(){
    getLogger().info("BreaktimeManager Destroyed.");
}

void BreaktimeManager::Ctor(IDifficultyBeatmap* difficultyBeatmap){
    this->difficultyBeatmap = difficultyBeatmap;
    this->breaks = new std::map<std::tuple<NoteLineLayer, int, float>, BeatmapObjectData*>();
}

bool compare(BeatmapObjectData* first, BeatmapObjectData* second){
    return first->time < second->time;
}

void BreaktimeManager::Initialize(){
    if (difficultyBeatmap)
    {
        breaks->clear();

        std::vector<BeatmapObjectData*> objects = {};
        auto lines = difficultyBeatmap->get_beatmapData()->beatmapLinesData;

        for (int i = 0; i < lines->get_Length(); i++){
            auto line = lines->get(i);
            for (int n = 0; n < line->beatmapObjectsData->get_Count(); n++){
                BeatmapObjectData* objectData = line->beatmapObjectsData->get_Item(n);
                if (objectData->get_beatmapObjectType().value == BeatmapObjectType::Note)
                {
                    objects.push_back(objectData);
                }
            }
        }
        
        std::sort(objects.begin(), objects.end(), compare);


        for (int i = 0; i < objects.size() - 1; i++){
            auto first = reinterpret_cast<NoteData*>(objects[i]);
            BeatmapObjectData* second = objects[i + 1];

            if (second->time - first->time > getPluginConfig().minTime.GetValue())
            {
                breaks->insert({std::make_tuple(first->noteLineLayer, first->lineIndex, first->time), second});
            }
        }

        for (std::pair<std::tuple<GlobalNamespace::NoteLineLayer, int, float>, GlobalNamespace::BeatmapObjectData*> pair : *breaks){
            getLogger().info("Time: %.2f, Obj: %p", pair.second->get_time(), pair.second);
        }
    }
}

void BreaktimeManager::NoteCut(NoteController* noteController){
    NoteEnded(noteController);
}

void BreaktimeManager::NoteEnded(NoteController* noteController){
    auto first = noteController->noteData;
    std::tuple<GlobalNamespace::NoteLineLayer, int, float> tuple = 
        std::make_tuple(first->get_noteLineLayer(), first->lineIndex, first->time);
    if (breaks->contains(tuple))
    {
        BeatmapObjectData* data = breaks->at(tuple);

        if (data)
        {
            if (breakDetected)
            {
                breakDetected(data->time);
            }
        }
    }
}