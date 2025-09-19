#include "Game.h"
#include "Player.h"
#include "Server.h"

static std::unordered_map<int, Bobber> bobbers;

void Game::UnInit() {
}

void Game::Init(){
}

void Game::Update() {
    auto now = std::chrono::steady_clock::now();

    for (auto& [playerId, bobber] : bobbers) {
        if (!bobber.ready && now >= bobber.biteTime) {
            bobber.ready = true;
            std::cout << "Bobber ready to jank" << playerId << "\n";
        }
    }
}

void Game::ThrowBobber(ENetPeer* peer) {
    Bobber bobber;
    //bobber.sprite.SetPosition(pos);
    bobber.sprite.SetRotation(0);
    bobber.sprite.SetSize(0.1f);
    bobber.sprite.SetTexture("Resources/Images/bobber.png");
    //Server::GetPlayer(peer).Ge;

    int waitSec = 3 + (rand() % 6);
    bobber.biteTime = std::chrono::steady_clock::now() + std::chrono::seconds(waitSec);

    //bobbers[peer] = bobber;
    //Server::AddSprite(bobber.sprite, "bobber" + peer);
}

bool Game::ReelBobber() {
    //auto it = bobbers.find(peer);
    //if (it == bobbers.end()) return false;

    //bool caught = it->second.ready;
    //bobbers.erase(it);

    //auto& sprites = Server::GetSprites();
    //Server::RemoveSprite("bobber" + peer);

    //return caught;
    return true;
}