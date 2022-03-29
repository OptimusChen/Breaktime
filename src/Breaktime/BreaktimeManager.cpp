#include "Breaktime/BreaktimeManager.hpp"
#include "System/Action_1.hpp"
#include "System/Func_2.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/Tasks/ITaskCompletionAction.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "System/Linq/Enumerable.hpp"
#include "System/Collections/Generic/LinkedList_1.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapLineData.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "GlobalNamespace/BeatmapEnvironmentHelper.hpp"
#include "../include/PluginConfig.hpp"

#include "questui/shared/ArrayUtil.hpp"

#include "main.hpp"

using namespace QuestUI;
using namespace Breaktime;
using namespace System;    
using namespace custom_types;    

BreaktimeManager::BreaktimeManager(IDifficultyBeatmap* map){
    Ctor(map);
}

BreaktimeManager::~BreaktimeManager(){
    delete breaks;
    getLogger().info("BreaktimeManager Destroyed.");
}

void BreaktimeManager::Ctor(IDifficultyBeatmap* difficultyBeatmap){
    this->difficultyBeatmap = difficultyBeatmap;
    this->breaks = new std::map<std::tuple<NoteLineLayer, int, float>, BeatmapDataItem*>();
}

bool compare(BeatmapDataItem* first, BeatmapDataItem* second){
    return first->get_time() < second->get_time();
}

Helpers::Coroutine BreaktimeManager::Initialize(){
    if (difficultyBeatmap)
    {
        breaks->clear();

        if (model == nullptr) model = ArrayUtil::First(Resources::FindObjectsOfTypeAll<BeatmapLevelsModel*>());

        IPreviewBeatmapLevel* level = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(difficultyBeatmap->get_level());

        std::vector<BeatmapDataItem*> objects = {};
        
        auto envInfo = level->get_environmentInfo();
        auto task = difficultyBeatmap->GetBeatmapDataAsync(envInfo);

        while (!task->get_IsCompleted()) co_yield nullptr;

        auto data = task->get_ResultOnSuccess();

        getLogger().info("[Breaktime] lets go %p", data->get_allBeatmapDataItems());

        ArrayW<BeatmapDataItem*> lines = ::Array<BeatmapDataItem*>::NewLength
            (data->get_allBeatmapDataItems()->get_Count());

        data->get_allBeatmapDataItems()->CopyTo(lines, 0);

        getLogger().info("[Breaktime] lets go 1.5");
        
        for (int i = 0; i < lines.Length(); i++){
            getLogger().info("[Breaktime] lets go 2");
            auto objectData = lines->get(i);
            if (objectData->klass == il2cpp_utils::GetClassFromName("", "NoteData"))
            {
                getLogger().info("[Breaktime] lets go 3");
                objects.push_back(objectData);
            }
        }
        
        std::sort(objects.begin(), objects.end(), compare);

        for (int i = 0; i < objects.size() - 1; i++){
            auto first = reinterpret_cast<NoteData*>(objects[i]);
            auto second = objects[i + 1];

            if (second->get_time() - first->get_time() > getPluginConfig().minTime.GetValue())
            {
                breaks->insert({std::make_tuple(first->get_noteLineLayer(), first->get_lineIndex(), first->get_time()), second});
            }
        }

        for (std::pair<std::tuple<GlobalNamespace::NoteLineLayer, int, float>, GlobalNamespace::BeatmapDataItem*> pair : *breaks){
            getLogger().info("Time: %.2f, Obj: %p", pair.second->get_time(), pair.second);
        }
    }
    co_return;
}

void BreaktimeManager::NoteCut(NoteController* noteController){
    NoteEnded(noteController);
}

void BreaktimeManager::NoteEnded(NoteController* noteController){
    if (!noteController) return;
    auto first = noteController->dyn__noteData();
    if (!first) return;
    std::tuple<GlobalNamespace::NoteLineLayer, int, float> tuple = 
        std::make_tuple(first->get_noteLineLayer(), first->get_lineIndex(), first->get_time());
    if (!breaks) return;
    if (breaks->contains(tuple))
    {
        BeatmapDataItem* data = breaks->at(tuple);

        if (data)
        {
            if (breakDetected)
            {
                breakDetected(data->get_time());
            }
        }
    }
}