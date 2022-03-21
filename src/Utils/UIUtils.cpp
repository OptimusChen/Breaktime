#include "Utils/UIUtils.hpp"

#include "HMUI/ImageView.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Vector3.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "UnityEngine/GameObject.hpp"

using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;

namespace UIUtils {

    HorizontalLayoutGroup* CreateHeader(UnityEngine::Transform* parent)
    {
        VerticalLayoutGroup* vertical = CreateVerticalLayoutGroup(parent);
        vertical->get_rectTransform()->set_anchoredPosition({0.0f, 45.0f});
        HorizontalLayoutGroup* horizontal =
            CreateHorizontalLayoutGroup(vertical->get_transform());

        auto layoutElem = horizontal->GetComponent<LayoutElement*>();
        layoutElem->set_preferredHeight(10);
        layoutElem->set_preferredWidth(100);

        Backgroundable* background =
            horizontal->get_gameObject()->AddComponent<Backgroundable*>();
        background->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr("panel-top"),
                                             1.0f);

        ImageView* imageView =
            background->get_gameObject()->GetComponentInChildren<ImageView*>();
        imageView->dyn__gradient() = true;
        imageView->dyn__gradientDirection() = 0;
        imageView->set_color(UnityEngine::Color::get_white());
        imageView->set_color0(UnityEngine::Color(0.0f,0.37f, 0.5f, 1));
        imageView->set_color1(UnityEngine::Color(0.0f,0.37f, 0.5f, 0));
        imageView->dyn__curvedCanvasSettingsHelper()->Reset();

        return horizontal;
    }

    void SkewButton(Button* button, float skew){
        ArrayW<ImageView*> images = button->GetComponentsInChildren<ImageView*>();
        
        for (int i = 0; i < images.Length(); i++){
            ImageView* image = images.get(i);
            image->dyn__skew() = skew;
        }
    }
}