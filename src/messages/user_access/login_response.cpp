/*
    Land of the Rair
    Copyright (C) 2019 Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "login_response.h"
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>

using namespace lotr;
using namespace rapidjson;

login_response::login_response(vector<message_player> players) noexcept : players(move(players)) {

}

string login_response::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String("players");
    writer.StartArray();

    for(auto& player : players) {
        writer.StartObject();

        writer.String("name");
        writer.String(player.name.c_str(), player.name.size());

        writer.String("map_name");
        writer.String(player.map_name.c_str(), player.map_name.size());

        writer.String("x");
        writer.Int(player.x);

        writer.String("y");
        writer.Int(player.y);

        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
    return sb.GetString();
}

optional<login_response> login_response::deserialize(rapidjson::Document const &d) {
    if (!d.HasMember("players")) {
        spdlog::warn("[login_response] deserialize failed");
        return nullopt;
    }

    vector<message_player> players;
    auto &players_array = d["players"];
    if(!players_array.IsArray()) {
        spdlog::warn("[login_response] deserialize failed");
        return nullopt;
    }

    for(SizeType i = 0; i < players_array.Size(); i++) {
        if (!players_array[i].IsObject() ||
            !players_array[i].HasMember("name") ||
            !players_array[i].HasMember("map_name") ||
            !players_array[i].HasMember("x") ||
            !players_array[i].HasMember("y")) {
            spdlog::warn("[login_response] deserialize failed");
            return nullopt;
        }
        players.emplace_back(players_array[i]["name"].GetString(), players_array[i]["map_name"].GetString(), players_array[i]["x"].GetInt(), players_array[i]["y"].GetInt());
    }

    return login_response(players);
}
