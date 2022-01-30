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
BreaktimeManager* manager = nullptr;
BreaktimeModule* breaktime = nullptr;
IDifficultyBeatmap* map = nullptr;

MAKE_HOOK_MATCH(NoteController_SendNoteWasCutEvent,
                &NoteController::SendNoteWasCutEvent, void,
                NoteController* self, ByRef<NoteCutInfo> noteCutInfo) {
  if (!getPluginConfig().Enabled.GetValue())
  
  manager->NoteCut(self);
  
  NoteController_SendNoteWasCutEvent(self, noteCutInfo);
}

MAKE_HOOK_MATCH(BeatmapObjectManager_HandleNoteControllerNoteWasMissed,
                &BeatmapObjectManager::HandleNoteControllerNoteWasMissed, void,
                BeatmapObjectManager* self, NoteController* noteController) {
  if (!getPluginConfig().Enabled.GetValue()) return;

  manager->NoteEnded(noteController);
  
  BeatmapObjectManager_HandleNoteControllerNoteWasMissed(self, noteController);
}

MAKE_HOOK_MATCH(AudioTimeSyncController_StartSong, &AudioTimeSyncController::StartSong, void, AudioTimeSyncController* self, float startTimeOffset) {
  AudioTimeSyncController_StartSong(self, startTimeOffset);

  if (!getPluginConfig().Enabled.GetValue()) return;

  Object::Destroy(obj);
  Object::Destroy(breaktime);

  if (manager)
  {
    manager->~BreaktimeManager();
  }

  manager = new BreaktimeManager(map);

  obj = GameObject::New_ctor(il2cpp_utils::newcsstr("Breaktime"));
  breaktime = obj->AddComponent<BreaktimeModule*>();
  breaktime->ctor(manager, self);
  breaktime->Start();
  
  manager->difficultyBeatmap = map;
  manager->Initialize();
  
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
  if (!getPluginConfig().Enabled.GetValue()) return;

  if (manager)
  {
    manager->~BreaktimeManager();
  }

  map = nullptr;


  MenuTransitionsHelper_RestartGame(self, finishCallback);
}

void Breaktime::Hooks::BreaktimeHooks(){
  INSTALL_HOOK(getLogger(), NoteController_SendNoteWasCutEvent);
  INSTALL_HOOK(getLogger(), BeatmapObjectManager_HandleNoteControllerNoteWasMissed);
  INSTALL_HOOK(getLogger(), AudioTimeSyncController_StartSong);
  INSTALL_HOOK(getLogger(), StandardLevelDetailView_RefreshContent);
  INSTALL_HOOK(getLogger(), MenuTransitionsHelper_RestartGame);
}