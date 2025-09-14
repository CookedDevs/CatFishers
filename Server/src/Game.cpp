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

void Game::ThrowBobber(int playerId, const CatCore::Vector3& pos) {
    Bobber bobber;
    bobber.sprite.SetPosition(pos);
    bobber.sprite.SetRotation(0);
    bobber.sprite.SetSize(0.1f);
    bobber.sprite.SetTexture("Resources/Images/bobber.png");

    int waitSec = 3 + (rand() % 6);
    bobber.biteTime = std::chrono::steady_clock::now() + std::chrono::seconds(waitSec);

    bobbers[playerId] = bobber;
    Server::AddSprite(bobber.sprite, "bobber" + playerId);
}

bool Game::ReelBobber(int playerId) {
    auto it = bobbers.find(playerId);
    if (it == bobbers.end()) return false;

    bool caught = it->second.ready;
    bobbers.erase(it);

    auto& sprites = Server::GetSprites();
    Server::RemoveSprite("bobber" + playerId);

    return caught;
}