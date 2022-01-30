#pragma once

#include "HMUI/ViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CODEGEN(
    Breaktime::UI, ImageSelectionViewController,
    HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate,
                            il2cpp_utils::FindMethodUnsafe("HMUI",
                                                           "ViewController",
                                                           "DidActivate", 3),
                            bool firstActivation, bool addedToHierarchy,
                            bool screenSystemEnabling);
    private:
        custom_types::Helpers::Coroutine SetupListElements(UnityEngine::Transform* parent);
)