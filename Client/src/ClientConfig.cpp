#include "ClientConfig.h"

#ifdef _ANDROID_
std::string path = "config.json";
#else // _ANDROID_
std::string path = "Resources/config.json";
#endif

#ifdef _ANDROID_
#include <raymob.h>
#include <android/log.h>
#define LOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, "ClientConfig", fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, "ClientConfig", fmt, ##__VA_ARGS__)
#else
#include <raylib.h>
#define LOGI(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#endif // _ANDROID_

void ClientConfig::Init()
{
    char* text = LoadFileText(path.c_str());
    if (text)
    {
        LOGE("whaat it found?");
        LOGE("%s", text);
        data = nlohmann::json::parse(text);
        name = data["name"].get<std::string>();
        ip = data["ip"].get<std::string>();
        UUID = data["UUID"].get<std::string>();
        UnloadFileText(text);
        return;
    }

    Save();
}

void ClientConfig::Save()
{
    data["name"] = name;
    data["ip"] = ip;
    data["UUID"] = UUID;

    bool ok = SaveFileText(path.c_str(), data.dump(4).c_str());
    if (ok) LOGE("OK");
    else LOGE("NOTOK");
}


