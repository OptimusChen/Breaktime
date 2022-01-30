#pragma once

#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"
#include "UI/ViewControllers/ImageSelectionViewController.hpp"
#include "UI/ViewControllers/SoundSelectionViewController.hpp"
#include "UI/ViewControllers/SettingsViewController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(Breaktime::UI, BreaktimeFlowCoordinator, HMUI::FlowCoordinator,
                      DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::FlowCoordinator::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
                      DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::FlowCoordinator::BackButtonWasPressed>::get(), HMUI::ViewController* topViewController);

                      DECLARE_INSTANCE_FIELD_DEFAULT(Breaktime::UI::ImageSelectionViewController*, imageSelectionView, nullptr);
                      DECLARE_INSTANCE_FIELD_DEFAULT(Breaktime::UI::SoundSelectionViewController*, soundSelectionView, nullptr);
                      DECLARE_INSTANCE_FIELD_DEFAULT(Breaktime::UI::SettingsViewController*, settingsView, nullptr);

)