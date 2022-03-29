#include "Breaktime/BreaktimeModule.hpp"

#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"

#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/RenderTextureDescriptor.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/AudioType.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/Networking/DownloadHandlerAudioClip.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestMultimedia.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/DepthTextureMode.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "PluginConfig.hpp"
#include "main.hpp"

DEFINE_TYPE(Breaktime, BreaktimeModule);

using namespace QuestUI;
using namespace custom_types;

namespace Breaktime {
    void BreaktimeModule::ctor(BreaktimeManager* manager, AudioTimeSyncController* audioController){
        this->manager = manager;
        this->audioController = audioController;
        this->audioSource = get_gameObject()->AddComponent<AudioSource*>();
        get_gameObject()->SetActive(true);

        CreateScreen();
        screen->set_active(false);
    }

    Helpers::Coroutine BreaktimeModule::StartBreak(float time){
        get_gameObject()->SetActive(true);

        getLogger().info("Getting Audio");
        UnityEngine::Networking::UnityWebRequest* www =
        UnityEngine::Networking::UnityWebRequestMultimedia::GetAudioClip(
            il2cpp_utils::newcsstr("file://" + getPluginConfig().SoundPath.GetValue()), 
            AudioType::OGGVORBIS);

        co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(www->SendWebRequest()));
        getLogger().info("Got Audio");

        if (www->get_isDone()) 
        {
            AudioClip* clip = UnityEngine::Networking::DownloadHandlerAudioClip::GetContent(www);
            audioController->StartCoroutine(Helpers::CoroutineHelper::New(HandleBreak(time, 
            {BeatSaberUI::FileToSprite(getPluginConfig().ImagePath.GetValue()),
            clip})));
            getLogger().info("Handling Break");
        }
        co_return;
    }

    Helpers::Coroutine BreaktimeModule::HandleBreak(float timeUnitlEnd, std::pair<Sprite*, AudioClip*> assets){
        if (!breakHappening)
        {
            getLogger().info("Starting Break");
            CreateScreen();
            getLogger().info("Created Screen");
            breakHappening = true;
            auto endPoint = timeUnitlEnd + audioController->dyn__songTime();
            get_gameObject()->SetActive(true);
            SetupVisuals(assets);
            getLogger().info("Setup Visuals");
            audioController->StartCoroutine(Helpers::CoroutineHelper::New(UpdateText(endPoint)));
            ModifyVisuals();
            getLogger().info("Colored Icons");
            screen->get_gameObject()->SetActive(true);

            if (getPluginConfig().FadeOut.GetValue()){
                getLogger().info("%s", string_format("%f", image->get_color().a).c_str());
                float o = 0.0f;
                while (image->get_color().a < getPluginConfig().ImageOpacity.GetValue()) {
                    co_yield nullptr;
                    Color color = getPluginConfig().ImageColor.GetValue();
                    image->set_color(Color(color.r, color.g, color.b, 
                        0.0f + o));
                    Color color1 = getPluginConfig().TextColor.GetValue();
                    text->set_color(Color(color1.r, color1.g, color1.b, 0.0f + o));
                    o = o + 0.025f;
                    getLogger().info("%s", string_format("%f", image->get_color().a).c_str());
                }
            }
            
            while (!(audioController->dyn__songTime() > endPoint - 2))
            {
                co_yield nullptr;
            }
            
            if (getPluginConfig().FadeOut.GetValue()){
                getLogger().info("%s", string_format("%f", image->get_color().a).c_str());
                float o = 0.0f;
                while (image->get_color().a > 0.0f) {
                    co_yield nullptr;
                    Color color = getPluginConfig().ImageColor.GetValue();
                    image->set_color(Color(color.r, color.g, color.b, 
                        getPluginConfig().ImageOpacity.GetValue() - o));
                    Color color1 = getPluginConfig().TextColor.GetValue();
                    text->set_color(Color(color1.r, color1.g, color1.b, 1.0f - o));
                    o = o + 0.025f;
                    getLogger().info("%s", string_format("%f", image->get_color().a).c_str());
                }
            }

            screen->get_gameObject()->SetActive(false);
            breakHappening = false;
            getLogger().info("Break Finished");
        }
        co_return;
    }

    void BreaktimeModule::ModifyVisuals(){
        if (image)
        {
            Color color = getPluginConfig().ImageColor.GetValue();

            if (getPluginConfig().FadeOut.GetValue()){
                image->set_color(Color(color.r, color.g, color.b, 
                    0.0f));
            }else{
                image->set_color(Color(color.r, color.g, color.b, 
                    getPluginConfig().ImageOpacity.GetValue()));
            }
        }

        if (text)
        {
            Color color = getPluginConfig().TextColor.GetValue();
            
            if (getPluginConfig().FadeOut.GetValue()){
                text->set_color(Color(color.r, color.g, color.b, 
                    0.0f));
            }else{
                text->set_color(Color(color.r, color.g, color.b, 
                    1.0f));
            }
        }
    }

    Helpers::Coroutine BreaktimeModule::UpdateText(float endPoint){
        while (breakHappening){
            timeText = string_format("%.2f", ((endPoint - audioController->dyn__songTime()) / audioController->dyn__timeScale()));
            text->set_text(il2cpp_utils::newcsstr(timeText));
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(0.075f));
        }
        co_return;
    } 

    void BreaktimeModule::SetupVisuals(std::pair<Sprite*, AudioClip*> assets){
        auto sprite = assets.first;
        auto audio = assets.second;

        if (sprite)
        {
            sprite->get_texture()->set_wrapMode(TextureWrapMode::Clamp);
            image->set_sprite(sprite);
        }
        if (audio && getPluginConfig().SoundEnabled.GetValue() && screen->get_active())
        {
            audioSource->PlayOneShot(audio);
        }
    }

    void BreaktimeModule::CreateScreen()
    {
        static ConstString ComboPanel = "ComboPanel";
        GameObject* comboPanel = GameObject::Find(ComboPanel);

        if (!screen)
        {
            screen = BeatSaberUI::CreateFloatingScreen({75, 75}, {0, 1.5f, 6}, Quaternion::get_identity().get_eulerAngles(), 0, false, false, 4);
            screen->get_gameObject()->SetActive(false);
        }else{
            screen->get_gameObject()->SetActive(true);
        }

        if (!image)
        {
            UI::VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(screen->get_transform());
            vertical->set_childAlignment(TextAnchor::MiddleCenter);

            Sprite* sprite = BeatSaberUI::FileToSprite(getPluginConfig().ImagePath.GetValue());

            image = BeatSaberUI::CreateImage(vertical->get_transform(), sprite,
                {0, 0}, {0.0f, 0.0f});

            if (round(getPluginConfig().ScaleX.GetValue()) == 
            round(getPluginConfig().ScaleY.GetValue())){
                image->set_preserveAspect(true);
            }

            UI::LayoutElement* elem = image->GetComponent<UI::LayoutElement*>();
            elem->set_preferredHeight(50.0f * getPluginConfig().ScaleY.GetValue());
            elem->set_preferredWidth(70.0f * getPluginConfig().ScaleX.GetValue());

            text = BeatSaberUI::CreateText(vertical->get_transform(), timeText, false, {0, 0});
            text->set_alignment(TMPro::TextAlignmentOptions::Center);
            text->set_fontSize(20);

            if (!getPluginConfig().ShowText.GetValue())
            {
                text->get_gameObject()->SetActive(false);
            }
        }else{
            image->get_gameObject()->SetActive(true);
            if (getPluginConfig().ShowText.GetValue())
            {
                text->get_gameObject()->SetActive(true);
            }
        }

        auto settings = Object::FindObjectOfType<PlayerDataModel*>()->dyn__playerData()->get_playerSpecificSettings();

        if (settings->get_noTextsAndHuds()){
            screen->SetActive(false);
        }
    }

    void BreaktimeModule::Start(){
        manager->breakDetected = std::function([=](float f){
            getLogger().info("yo yo poggers break");
            this->BreakDetected(f);
        });
    }

    void BreaktimeModule::BreakDetected(float timeForNextNote){
        audioController->StartCoroutine(Helpers::CoroutineHelper::New(StartBreak(timeForNextNote - audioController->dyn__songTime())));
    }
}
