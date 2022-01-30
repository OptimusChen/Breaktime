#pragma once

#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Color.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;

namespace UIUtils
{
    HorizontalLayoutGroup* CreateHeader(Transform* parent);
    void SkewButton(Button* button, float skew);
}