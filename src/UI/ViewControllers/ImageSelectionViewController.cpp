#include "UI/ViewControllers/ImageSelectionViewController.hpp"

#include "Utils/UIUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

DEFINE_TYPE(Breaktime::UI, ImageSelectionViewController);

using namespace HMUI;
using namespace TMPro;
using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI::BeatSaberUI;

#define SetPreferredSize(identifier, width, height)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \
    layout##identifier->set_preferredHeight(height)

#define StartCoroutine(method)                                               \
    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine( \
        reinterpret_cast<System::Collections::IEnumerator*>(                 \
            custom_types::Helpers::CoroutineHelper::New(method)))


namespace Breaktime::UI {

    void ImageSelectionViewController::DidActivate(bool firstActivation, bool addedToHierarchy,
                                        bool screenSystemEnabling) {
        if (firstActivation) 
        {
            if (!get_gameObject()) return;

            get_gameObject()->AddComponent<Touchable*>();

            GameObject* mainLayout = GameObject::New_ctor();
            RectTransform* parent = mainLayout->AddComponent<RectTransform*>();
            parent->SetParent(get_transform(), false);
            parent->set_localPosition(UnityEngine::Vector3(38.0f, 0.0f));

            HorizontalLayoutGroup* header = UIUtils::CreateHeader(get_transform());

            TextMeshProUGUI* text =
            CreateText(header->get_transform(), "Image Selection");
            text->set_fontSize(text->get_fontSize() * 1.5f);
            text->set_alignment(TextAlignmentOptions::Center);

            auto containerGroup = CreateHorizontalLayoutGroup(get_transform());
            containerGroup->get_rectTransform()->set_anchoredPosition(Vector2(0.0f, 0.0f));
            
            SetPreferredSize(containerGroup, 20, 80);

            auto bg = containerGroup->get_gameObject()->AddComponent<Backgroundable*>();
            bg->ApplyBackground(il2cpp_utils::newcsstr("panel-top"));
            bg->background->set_color(UnityEngine::Color(0.1f, 0.1f, 0.1f, 0.0f));

            GameObject* container = CreateScrollableSettingsContainer(bg->get_transform());

            StartCoroutine(SetupListElements(container->get_transform()));
        }
    }

    custom_types::Helpers::Coroutine ImageSelectionViewController::SetupListElements(Transform* list){
        std::vector<std::string> icons = FileUtils::getFiles(
            "/sdcard/ModData/com.beatgames.beatsaber/Mods/Breaktime/Icons/");
        int i = 0;
        while (!(i == icons.size()))
        {
            i++;
            auto image = icons.at(i - 1);

            HorizontalLayoutGroup* horizontal = CreateHorizontalLayoutGroup(list);            
            horizontal->set_childControlWidth(false);

            LayoutElement* elem = horizontal->GetComponent<LayoutElement*>();
            elem->set_minHeight(10);
            elem->set_minWidth(20);

            Sprite* sprite = FileToSprite(image.c_str());
            
            CreateImage(elem->get_transform(), sprite,
                Vector2(0.0f, 0.0f), Vector2(10.0f, 2.0f));

            CreateText(elem->get_transform(), 
                FileUtils::GetFileName(image.c_str(), false), true);

            elem->set_minWidth(1.0f);

            auto select = CreateUIButton(horizontal->get_transform(), "", [](){ });
            UIUtils::SkewButton(select, 0.0f);

            auto selectText = CreateText(select->get_transform(), "<color=#88ff88>select</color>", false);
            selectText->set_alignment(TextAlignmentOptions::Center);

            auto del = CreateUIButton(horizontal->get_transform(), "", [](){ });
            UIUtils::SkewButton(del, 0.0f);

            auto delText = CreateText(del->get_transform(), "<color=#ff8888>delete</color>", false);
            delText->set_alignment(TextAlignmentOptions::Center);

            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(0.1));
        }
        co_return;
    }
}