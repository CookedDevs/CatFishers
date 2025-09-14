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

void Game::ThrowBobber(int playerId, const Vector3& pos) {
    Bobber bobber;
    bobber.sprite.position = pos;
    bobber.sprite.rotation = 0;
    bobber.sprite.size = 0.1f;
    bobber.sprite.texture = "Resources/Images/bobber.png";

    int waitSec = 3 + (rand() % 6);
    bobber.biteTime = std::chrono::steady_clock::now() + std::chrono::seconds(waitSec);

    bobbers[playerId] = bobber;
    Server::GetSprites().push_back(bobber.sprite);
}

bool Game::ReelBobber(int playerId) {
    auto it = bobbers.find(playerId);
    if (it == bobbers.end()) return false;

    bool caught = it->second.ready;
    bobbers.erase(it);

    auto& sprites = Server::GetSprites();
    sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
        [&](auto& s) { return s.texture == "Resources/Images/bobber.png"; }), sprites.end());

    return caught;
}