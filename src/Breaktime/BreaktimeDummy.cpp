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

#define StartCoroutine(method) GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(method))

namespace Breaktime {

    BreaktimeDummy::BreaktimeDummy(){

    }

    void BreaktimeDummy::SpawnDummy(){
        CreateScreen();

        StartCoroutine(PlaySound());
    }

    Helpers::Coroutine BreaktimeDummy::PlaySound(){
        
        while (!createdUI){
            co_yield nullptr;
        }
    
        UnityEngine::Networking::UnityWebRequest* www =
        UnityEngine::Networking::UnityWebRequestMultimedia::GetAudioClip(
            il2cpp_utils::newcsstr("file://" + getPluginConfig().SoundPath.GetValue()), 
            AudioType::OGGVORBIS);
    
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(www->SendWebRequest()));

        if (www->get_isDone()) 
        {
            AudioClip* clip = UnityEngine::Networking::DownloadHandlerAudioClip::GetContent(www);

            static ConstString goName = "Sound";        
            GameObject* audioClipGO = GameObject::New_ctor(goName);

            UnityEngine::AudioSource* source =
                audioClipGO->AddComponent<AudioSource*>();
            source->set_playOnAwake(false);
            
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

            if (getPluginConfig().SoundEnabled.GetValue()){
                source->PlayOneShot(clip);

                co_yield reinterpret_cast<System::Collections::IEnumerator*>(CRASH_UNLESS(
                    UnityEngine::WaitForSeconds::New_ctor(1.0f)));
            }

            UnityEngine::Object::Destroy(source);
            UnityEngine::Object::Destroy(audioClipGO);
 
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

            Object::Destroy(screen);
            Object::Destroy(image);
            Object::Destroy(text);

            delete this;
        }
    }

    void BreaktimeDummy::CreateScreen(){
        createdUI = false;

        screen = BeatSaberUI::CreateFloatingScreen({75, 75}, {0, 1.5f, 6}, Quaternion::get_identity().get_eulerAngles(), 0, false, false, 4);
        screen->get_gameObject()->SetActive(true);
        
        UI::VerticalLayoutGroup* vertical = BeatSaberUI::CreateVerticalLayoutGroup(screen->get_transform());
        vertical->set_childAlignment(TextAnchor::MiddleCenter);

        Sprite* sprite = BeatSaberUI::FileToSprite(getPluginConfig().ImagePath.GetValue());

        image = BeatSaberUI::CreateImage(vertical->get_transform(), sprite,
            {0, 0}, {0.0f, 0.0f});

        if (getPluginConfig().ScaleX.GetValue() == 
            getPluginConfig().ScaleY.GetValue()){
            image->set_preserveAspect(true);
        }
        
        UI::LayoutElement* elem = image->GetComponent<UI::LayoutElement*>();
        elem->set_preferredHeight(50.0f * getPluginConfig().ScaleY.GetValue());
        elem->set_preferredWidth(60.0f * getPluginConfig().ScaleX.GetValue());

        static ConstString Text = "0.00";        

        text = BeatSaberUI::CreateText(vertical->get_transform(), "0.00", false);
        text->set_alignment(TMPro::TextAlignmentOptions::Center);
        text->set_fontSize(20);

        if (!getPluginConfig().ShowText.GetValue())
        {
            text->get_gameObject()->SetActive(false);
        }

        Color color = getPluginConfig().ImageColor.GetValue();
        Color color1 = getPluginConfig().ImageColor.GetValue();

        if (getPluginConfig().FadeOut.GetValue()){
            image->set_color(Color(color.r, color.g, color.b, 0.0f));
            getLogger().info("e %s", string_format("%f", image->get_color().a).c_str());
            text->set_color(Color(color1.r, color1.g, color1.b, 0.0f));
        }else{
            image->set_color(Color(color.r, color.g, color.b, getPluginConfig().ImageOpacity.GetValue()));
            getLogger().info("e %s", string_format("%f", image->get_color().a).c_str());
            text->set_color(Color(color1.r, color1.g, color1.b, 1.0f));
        }

        createdUI = true;
    }
}