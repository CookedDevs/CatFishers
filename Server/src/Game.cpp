#include "Game.h"
#include "Player.h"
#include "Server.h"
#include <vector>
#include <iostream>
#include <cmath> 

static std::unordered_map<ENetPeer*, Bobber> bobbers;

void Game::UnInit() {
}

void Game::Init(){
}

void Game::Update() {
    auto now = std::chrono::steady_clock::now();

    for (auto& [peer, bobber] : bobbers) {
        if (!bobber.ready && now >= bobber.biteTime) {
            bobber.ready = true;
            bobber.floatStart = now;
            bobber.biteDuration = std::chrono::seconds(3 + (rand() % 3));
            bobber.lastYankStrength = 0.0f;
            bobber.yankStart = -999.0f;
            bobber.nextYankTime = 0.0f;
            std::cout << "Bobber ready to jank " << peer->connectID << "\n";
        }
        // bite bobbing
        else if (now >= bobber.biteTime) {
            float baseAmplitude = 4.0f;
            float baseFrequency = 1.0f;

            float rippleAmplitude = 0.5f;
            float rippleFrequency = 6.0f;

            std::chrono::duration<float> elapsed = now - bobber.floatStart;
            float timeSec = elapsed.count();
            float phase = bobber.randomPhase;
            float mainOffset = std::sin(timeSec * baseFrequency + phase) * baseAmplitude;
            float rippleOffset = std::sin(timeSec * rippleFrequency + phase * 2.0f) * rippleAmplitude;

            if (timeSec >= bobber.nextYankTime) {
                bobber.lastYankStrength = -((rand() % 5) + 4);
                bobber.yankStart = timeSec;
                bobber.nextYankTime = timeSec + ((rand() % 1000) / 1000.0f) * 2.0f + 0.5f;
            }

            float yankOffset = 0.0f;
            if (timeSec - bobber.yankStart < 0.6f) {
                float t = timeSec - bobber.yankStart;
                yankOffset = bobber.lastYankStrength * std::exp(-3.0f * t) * std::sin(12.0f * t);
            }
            float offset = mainOffset + rippleOffset + yankOffset;
            bobber.pos.y = bobber.basePos.y - offset;
            Server::GetSprite(bobber.spriteid)->SetPosition(bobber.pos);

            if (now - bobber.biteTime > bobber.biteDuration) {
                bobber.ready = false;
                int waitSec = 3 + (rand() % 6);
                bobber.biteTime = std::chrono::steady_clock::now() + std::chrono::seconds(waitSec);
                std::cout << "You lost the fish :(\n";
            }
        }
        // normal bobbing
        else {
            float idleAmplitude = 1.5f;
            float idleFrequency = 0.5f;

            std::chrono::duration<float> elapsed = now - bobber.floatStart;
            float timeSec = elapsed.count();
            float phase = bobber.randomPhase;

            float offset = std::sin(timeSec * idleFrequency + phase) * idleAmplitude;

            bobber.pos.y = bobber.basePos.y + offset;
            Server::GetSprite(bobber.spriteid)->SetPosition(bobber.pos);
        }

    }
}

void Game::ThrowBobber(ENetPeer* peer) {
    auto player = Server::GetPlayer(peer);
    if (!player) return;

    CatCore::Mouse mouse = player->GetMouse();

    Bobber bobber;
    bobber.pos = CatCore::Vector3{ (mouse.x) - 5.1f, (mouse.y) - 7.65f, 0.0f};
    bobber.spriteid = "bobber" + std::to_string(peer->connectID);
    CatCore::Sprite sprite;
    sprite.SetSize(0.1f);
    bobber.basePos = bobber.pos;
    sprite.SetPosition(bobber.pos);
    sprite.SetTexture("Resources/Images/bobber.png");
    bobber.randomPhase = (float)rand() / RAND_MAX * 6.283185f;

    std::cout << std::to_string(mouse.x) << "  " << std::to_string(mouse.y) <<"\n";

    int waitSec = 3 + (rand() % 6);
    bobber.biteTime = std::chrono::steady_clock::now() + std::chrono::seconds(waitSec);
    bobber.ready = false;

    bobbers[peer] = bobber;

    Server::AddSprite(sprite, bobber.spriteid);
    auto sprites = Server::GetSprites();
}

bool Game::ReelBobber(ENetPeer* peer) {
    auto it = bobbers.find(peer);
    if (it == bobbers.end()) {
        std::cout << "No bobber found for peer\n";
        return false;
    }

    if (std::chrono::steady_clock::now() >= it->second.biteTime) {
        Server::RemoveSprite(it->second.spriteid);
        bobbers.erase(it); 
        std::cout << "Fish caught!\n";
        return true;
    }

    Server::RemoveSprite(it->second.spriteid);
    std::cout << "Failed to catch\n";
    bobbers.erase(it);
    return false;
}

