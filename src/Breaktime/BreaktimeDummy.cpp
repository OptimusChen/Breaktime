#include "Breaktime/BreaktimeDummy.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"
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

using namespace GlobalNamespace;

#define StartCoroutine(method)                                               \
    GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine( \
        reinterpret_cast<System::Collections::IEnumerator*>(                 \
            custom_types::Helpers::CoroutineHelper::New(method)))

namespace Breaktime {

    BreaktimeDummy::BreaktimeDummy(){

    }

    void BreaktimeDummy::SpawnDummy(){
        CreateScreen();

        StartCoroutine(PlaySound());
    }

    Helpers::Coroutine BreaktimeDummy::PlaySound(){
        UnityEngine::Networking::UnityWebRequest* www =
        UnityEngine::Networking::UnityWebRequestMultimedia::GetAudioClip(
            il2cpp_utils::newcsstr("file://" + getPluginConfig().SoundPath.GetValue()), 
            AudioType::OGGVORBIS);
    
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(www->SendWebRequest()));

        if (www->get_isDone()) 
        {
            AudioClip* clip = UnityEngine::Networking::DownloadHandlerAudioClip::GetContent(www);

            static auto goName = il2cpp_utils::newcsstr("Sound");
            GameObject* audioClipGO = GameObject::New_ctor(goName);

            UnityEngine::AudioSource* source =
                audioClipGO->AddComponent<AudioSource*>();
            source->set_playOnAwake(false);

            if (getPluginConfig().SoundEnabled.GetValue()){
                source->PlayOneShot(clip);

                co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(
                    UnityEngine::WaitForSeconds::New_ctor(1.0f)));
            }

            UnityEngine::Object::Destroy(source);
            UnityEngine::Object::Destroy(audioClipGO);

            co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(
                UnityEngine::WaitForSeconds::New_ctor(1.0f)));
            Object::Destroy(screen);
            Object::Destroy(image);
            Object::Destroy(text);
            delete this;
        }
    }

    void BreaktimeDummy::CreateScreen(){
        screen = BeatSaberUI::CreateFloatingScreen({75, 75}, {0, 1.5f, 6}, Quaternion::get_identity().get_eulerAngles(), 0, false, false, 4);
        screen->get_gameObject()->SetActive(true);
        
        UI::VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(screen->get_transform());
        vertical->set_childAlignment(TextAnchor::MiddleCenter);

        Sprite* sprite = BeatSaberUI::FileToSprite(getPluginConfig().ImagePath.GetValue());

        image = BeatSaberUI::CreateImage(vertical->get_transform(), sprite,
            {0, 0}, {0.0f, 0.0f});
        
        UI::LayoutElement* elem = image->GetComponent<UI::LayoutElement*>();
        elem->set_preferredHeight(50.0f * getPluginConfig().ScaleY.GetValue());
        elem->set_preferredWidth(60.0f * getPluginConfig().ScaleX.GetValue());

        text = BeatSaberUI::CreateText(vertical->get_transform(), "0.00", false);
        text->set_alignment(TMPro::TextAlignmentOptions::Left);
        text->set_fontSize(20);

        if (!getPluginConfig().ShowText.GetValue())
        {
            text->get_gameObject()->SetActive(false);
        }

        Color color = getPluginConfig().ImageColor.GetValue();
        image->set_color(Color(color.r, color.g, color.b, 
            getPluginConfig().ImageOpacity.GetValue()));

        text->set_color(getPluginConfig().TextColor.GetValue());
    }
}