#include "main.hpp"

#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/BeatmapObjectManager.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "Breaktime/BreaktimeManager.hpp"
#include "Breaktime/BreaktimeModule.hpp"
#include "UnityEngine/Resources.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "PluginConfig.hpp"

using namespace Breaktime;

GameObject* obj = nullptr;
std::shared_ptr<BreaktimeManager> manager;
BreaktimeModule* breaktime = nullptr;
IDifficultyBeatmap* map = nullptr;

MAKE_HOOK_MATCH(NoteController_SendNoteWasCutEvent,
                &NoteController::SendNoteWasCutEvent, void,
                NoteController* self, ByRef<NoteCutInfo> noteCutInfo) {
  if (getPluginConfig().Enabled.GetValue()) {
    assert(manager != nullptr);
    manager->NoteCut(self);
  }
  
  NoteController_SendNoteWasCutEvent(self, noteCutInfo);
}

MAKE_HOOK_MATCH(BeatmapObjectManager_HandleNoteControllerNoteWasMissed,
                &BeatmapObjectManager::HandleNoteControllerNoteWasMissed, void,
                BeatmapObjectManager* self, NoteController* noteController) {
  if (getPluginConfig().Enabled.GetValue()) {
    assert(manager);
    manager->NoteEnded(noteController);
  }
  
  BeatmapObjectManager_HandleNoteControllerNoteWasMissed(self, noteController);
}

MAKE_HOOK_MATCH(AudioTimeSyncController_StartSong, &AudioTimeSyncController::StartSong, void, AudioTimeSyncController* self, float startTimeOffset) {
  AudioTimeSyncController_StartSong(self, startTimeOffset);
  
  getLogger().info("SongStart");
  if (!getPluginConfig().Enabled.GetValue()) return;

  if (manager)
  {
    manager.reset();
    manager = nullptr;
  }
  getLogger().info("Destroyed Manager");

  Object::Destroy(obj);
  Object::Destroy(breaktime);
  getLogger().info("Destroyed Objects");

  if (map){
    manager = std::make_shared<BreaktimeManager>(map);
    getLogger().info("Created new Manager");
    obj = GameObject::New_ctor(il2cpp_utils::newcsstr("Breaktime"));
    getLogger().info("Created new Obj");
    breaktime = obj->AddComponent<BreaktimeModule*>();
    breaktime->ctor(manager.get(), self);
    breaktime->Start();

    Object::DontDestroyOnLoad(breaktime);
    Object::DontDestroyOnLoad(obj);

    getLogger().info("Created new Module");
    
    manager->Initialize();  
    getLogger().info("Initialize Manager");
  }

  getLogger().info("SongStart Finished");
}

MAKE_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &StandardLevelDetailView::RefreshContent, void,
                StandardLevelDetailView* self) {

  StandardLevelDetailView_RefreshContent(self);
  
  if (!getPluginConfig().Enabled.GetValue()) return;

  if (!self) return;

  map = self->selectedDifficultyBeatmap;
}

MAKE_HOOK_MATCH(MenuTransitionsHelper_RestartGame, 
                &MenuTransitionsHelper::RestartGame,
                void, MenuTransitionsHelper* self, System::Action_1<Zenject::DiContainer *> *finishCallback) {
  if (getPluginConfig().Enabled.GetValue()) {
    if (manager)
    {
      manager.reset();
      manager = nullptr;
    }

    map = nullptr;
  }

  MenuTransitionsHelper_RestartGame(self, finishCallback);
}

void Breaktime::Hooks::BreaktimeHooks(){
  INSTALL_HOOK(getLogger(), NoteController_SendNoteWasCutEvent);
  INSTALL_HOOK(getLogger(), BeatmapObjectManager_HandleNoteControllerNoteWasMissed);
  INSTALL_HOOK(getLogger(), AudioTimeSyncController_StartSong);
  INSTALL_HOOK(getLogger(), StandardLevelDetailView_RefreshContent);
  INSTALL_HOOK(getLogger(), MenuTransitionsHelper_RestartGame);
}