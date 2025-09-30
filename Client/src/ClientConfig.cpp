#include "ClientConfig.h"
#include "CatLog.h"

#ifdef _ANDROID_
#include <raymob.h>
std::string path = "config.json";
#else // _ANDROID_
#include <raylib.h>
std::string path = "Resources/config.json";
#endif


void ClientConfig::Init()
{
    char* text = LoadFileText(path.c_str());
    if (text)
    {
        CatCore::LogInfo(std::string("loaded: \n") + text);
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
    if (ok) CatCore::LogInfo("Saved");
    else CatCore::LogError("Not saved");
}


