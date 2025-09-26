#include "Player.h"

namespace CatCore
{
	void Player::ToJson(nlohmann::json& json)
	{
		inventory.ToJson(json["inventory"]);

		json["position"]["x"] = position.x;
		json["position"]["y"] = position.y;
		json["position"]["z"] = position.z;

		json["texture"] = texture;
		json["name"] = name;
	}

	void Player::FromJson(const nlohmann::json& json)
	{
		inventory.FromJson(json["inventory"]);

		position.x = json["position"]["x"];
		position.y = json["position"]["y"];
		position.z = json["position"]["z"];

		texture = json["texture"];
		name = json["name"];
	}
}