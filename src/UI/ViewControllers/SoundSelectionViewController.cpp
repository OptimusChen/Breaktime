#include "UI/ViewControllers/SoundSelectionViewController.hpp"

#include "Sprites.hpp"
#include "PluginConfig.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/FileUtils.hpp"
#include "HMUI/Touchable.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "GlobalNamespace/SharedCoroutineStarter.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include <map>

DEFINE_TYPE(Breaktime::UI, SoundSelectionViewController);

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

#define SetPreferredWidth(identifier, width)                                         \
    auto layout##identifier = identifier->get_gameObject()->GetComponent<LayoutElement*>(); \
    if (!layout##identifier)                                                                \
        layout##identifier = identifier->get_gameObject()->AddComponent<LayoutElement*>();  \
    layout##identifier->set_preferredWidth(width);                                          \

#define StartCoroutine(method) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(method))

namespace Breaktime::UI {

    void SoundSelectionViewController::DidActivate(bool firstActivation, bool addedToHierarchy,
                                        bool screenSystemEnabling) {
        if (firstActivation) {
            if (!get_gameObject()) return;

            get_gameObject()->AddComponent<Touchable*>();

            GameObject* mainLayout = GameObject::New_ctor();
            RectTransform* parent = mainLayout->AddComponent<RectTransform*>();
            parent->SetParent(get_transform(), false);
            parent->set_localPosition(Vector3(38.0f, 0.0f, 0.0f));

            HorizontalLayoutGroup* header = UIUtils::CreateHeader(get_transform());

            TextMeshProUGUI* text =
            CreateText(header->get_transform(), "Sound Selection");
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

    custom_types::Helpers::Coroutine SoundSelectionViewController::SetupListElements(Transform* list){
        std::vector<std::string> sounds = FileUtils::getFiles(
            "/sdcard/ModData/com.beatgames.beatsaber/Mods/Breaktime/Sounds/");
        std::map<Button*, std::string>* buttons = new std::map<Button*, std::string>();
        std::map<TextMeshProUGUI*, std::string>* texts = new std::map<TextMeshProUGUI*, std::string>();

        int i = 0;
        while (!(i == sounds.size()))
        {
            i++;
            auto sound = sounds.at(i - 1);
            std::string soundPath = "/" + std::string(sound.c_str());

            HorizontalLayoutGroup* horizontal = CreateHorizontalLayoutGroup(list);            
            horizontal->set_childControlWidth(false);        
            horizontal->set_childForceExpandWidth(false);

            LayoutElement* elem = horizontal->GetComponent<LayoutElement*>();
            elem->set_minHeight(10);
            elem->set_minWidth(20);

            Sprite* sprite = Base64ToSprite(Sound_Icon);
            
            auto preview = CreateImage(horizontal->get_transform(), sprite,
                Vector2(0.0f, 0.0f), Vector2(10.0f, 2.0f));

            std::string fileName = FileUtils::GetFileName(soundPath, false);

            auto name = CreateText(horizontal->get_transform(), 
                fileName, true);

            auto select = CreateUIButton(horizontal->get_transform(), "", [=](){
                getPluginConfig().SoundPath.SetValue(soundPath);

                for (std::pair<Button*, std::string> pair : *buttons){
                    std::string path = pair.second;
                    Button* button = pair.first;

                    if (path.compare(std::string(soundPath)) == 0){
                        button->set_interactable(false);
                        BeatSaberUI::AddHoverHint(button->get_gameObject(), "Already Selected!");
                    }else{
                        button->set_interactable(true);
                        BeatSaberUI::AddHoverHint(button->get_gameObject(), "");
                    }
                }

                for (std::pair<TextMeshProUGUI*, std::string> pair : *texts){
                    std::string path = pair.second;
                    std::string file = FileUtils::GetFileName(path, false);
                    TextMeshProUGUI* text = pair.first;

                    if (path.compare(soundPath) == 0){
                        text->SetText(il2cpp_utils::newcsstr("<i><color=\"green\">" + file + "</color></i>"));
                    }else{
                        text->SetText(il2cpp_utils::newcsstr("<i>" + file + "</i>"));
                    }
                }
            });


            UIUtils::SkewButton(select, 0.0f);

            if (getPluginConfig().SoundPath.GetValue().compare(std::string(soundPath)) == 0){
                select->set_interactable(false);
                BeatSaberUI::AddHoverHint(select->get_gameObject(), "Already Selected!");
                
                name->SetText(il2cpp_utils::newcsstr("<i><color=\"green\">" + fileName + "</color></i>"));
            }

            auto selectText = CreateText(select->get_transform(), "<color=#88ff88>Select</color>", false);
            selectText->set_alignment(TextAlignmentOptions::Center);
            
            texts->insert({name, soundPath});
            buttons->insert({select, soundPath});

            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(0.1));
        }
        co_return;
    }
}