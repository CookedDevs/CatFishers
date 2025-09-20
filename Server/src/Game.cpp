#include "Game.h"
#include "Player.h"
#include "Server.h"
#include <vector>
#include <iostream>

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
            std::cout << "Bobber ready to jank " << peer->connectID << "\n";
        }
    }
}

void Game::ThrowBobber(ENetPeer* peer) {
    std::cout << "Throwbobber recieved" << "\n";
    auto player = Server::GetPlayer(peer);
    if (!player) return;

    CatCore::Mouse mouse = player->GetMouse();

    Bobber bobber;
    bobber.sprite.SetSize(0.1f);
    bobber.sprite.SetPosition(CatCore::Vector3{(mouse.x) - 5.1f, (mouse.y) - 7.65f, 0.0f});
    bobber.sprite.SetTexture("Resources/Images/bobber.png");

    std::cout << std::to_string(mouse.x) << "  " << std::to_string(mouse.y) <<"\n";

    int waitSec = 3 + (rand() % 6);
    bobber.biteTime = std::chrono::steady_clock::now() + std::chrono::seconds(waitSec);
    bobber.ready = false;

    bobbers[peer] = bobber;

    Server::AddSprite(bobber.sprite, "bobber" + std::to_string(peer->connectID));
    std::cout << "Throwbobber ended" << "\n";
}

bool Game::ReelBobber(ENetPeer* peer) {
    std::cout << "Reelbobber recieved\n";

    auto it = bobbers.find(peer);
    if (it == bobbers.end()) {
        std::cout << "No bobber found for peer\n";
        return false;
    }

    if (std::chrono::steady_clock::now() >= it->second.biteTime) {
        std::cout << "Erasing bobber\n";
        Server::RemoveSprite("bobber" + std::to_string(peer->connectID));
        bobbers.erase(it); 
        std::cout << "Reelbobber ended\n";
        return true;
    }

    std::cout << "Reelbobber still active\n";
    return false;
}

