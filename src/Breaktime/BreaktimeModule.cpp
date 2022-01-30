#include "Breaktime/BreaktimeModule.hpp"

#include "UnityEngine/AudioType.hpp"
#include "UnityEngine/Networking/DownloadHandlerAudioClip.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestMultimedia.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
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
            audioController->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator*>(
            Helpers::CoroutineHelper::New(HandleBreak(time, 
            {BeatSaberUI::FileToSprite(getPluginConfig().ImagePath.GetValue()),
            clip}))));
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
            auto endPoint = timeUnitlEnd + audioController->songTime;
            get_gameObject()->SetActive(true);
            screen->get_gameObject()->SetActive(true);
            SetupVisuals(assets);
            getLogger().info("Setup Visuals");
            audioController->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator*>(Helpers::CoroutineHelper::New(UpdateText(endPoint))));
            ModifyVisuals();
            getLogger().info("Colored Icons");
            while (!(audioController->songTime > endPoint - 2))
            {
                co_yield nullptr;
            }
            screen->get_transform()->set_localPosition(Vector3(0, 1.5f, 4.0f));
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
            image->set_color(Color(color.r, color.g, color.b, 
                getPluginConfig().ImageOpacity.GetValue()));
        }

        if (text)
        {
            text->set_color(getPluginConfig().TextColor.GetValue());
        }
    }

    Helpers::Coroutine BreaktimeModule::UpdateText(float endPoint){
        while (breakHappening){
            timeText = string_format("%.1f", ((endPoint - audioController->songTime) / audioController->timeScale));
            text->set_text(il2cpp_utils::newcsstr(timeText));
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(0.05f));
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
        if (audio && getPluginConfig().SoundEnabled.GetValue())
        {
            audioSource->PlayOneShot(audio);
        }
    }

    void BreaktimeModule::CreateScreen(){
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
            image = BeatSaberUI::CreateImage(vertical->get_transform(), 
                BeatSaberUI::FileToSprite(getPluginConfig().ImagePath.GetValue()),
                {0, 0}, {0.0f, 0.0f});
            
            UI::LayoutElement* elem = image->GetComponent<UI::LayoutElement*>();
            elem->set_preferredHeight(50.0f);
            elem->set_preferredWidth(60.0f);

            text = BeatSaberUI::CreateText(vertical->get_transform(), timeText, false);
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
    }

    void BreaktimeModule::Start(){
        manager->breakDetected = std::function([=](float f){
            getLogger().info("yo yo poggers break");
            this->BreakDetected(f);
        });
    }

    void BreaktimeModule::BreakDetected(float timeForNextNote){
        audioController->StartCoroutine(reinterpret_cast<System::Collections::IEnumerator*>
        (Helpers::CoroutineHelper::New(StartBreak(timeForNextNote - audioController->songTime))));
    }
}
