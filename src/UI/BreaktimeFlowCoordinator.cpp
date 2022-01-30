#include "UI/BreaktimeFlowCoordinator.hpp"

#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "questui/shared/BeatSaberUI.hpp"

DEFINE_TYPE(Breaktime::UI, BreaktimeFlowCoordinator);

using namespace HMUI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

namespace Breaktime::UI {

    void BreaktimeFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if (firstActivation) {
            imageSelectionView = CreateViewController<ImageSelectionViewController*>();
            soundSelectionView = CreateViewController<SoundSelectionViewController*>();
            settingsView = CreateViewController<SettingsViewController*>();

            SetTitle(il2cpp_utils::newcsstr("Breaktime"), ViewController::AnimationType::Out);
            set_showBackButton(true);
            ProvideInitialViewControllers(settingsView, soundSelectionView, imageSelectionView, nullptr, nullptr);
        }
    }

    void BreaktimeFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
        this->parentFlowCoordinator->DismissFlowCoordinator(this, ViewController::AnimationDirection::Horizontal, nullptr, false);
    }
}