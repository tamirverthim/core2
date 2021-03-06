/*
    Land of the Rair
    Copyright (C) 2019  Michael de Lang

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

#include <catch2/catch.hpp>
#include <spdlog/spdlog.h>
#include "test_helpers/startup_helper.h"
#include "../src/fov.h"
#include <ecs/components.h>

using namespace std;
using namespace lotr;

TEST_CASE("fov tests") {
    SECTION("no obstacles") {
        const uint32_t map_size = 9;
        vector<map_layer> map_layers;
        vector<uint32_t> wall_data(map_size*map_size);
        vector<map_object> object_data(map_size*map_size);

        for(uint32_t i = 0; i < map_size*map_size; i++) {
            wall_data.push_back(0);
            object_data.emplace_back(0, 0, 0, 0, 0, 0, "", "", vector<map_property>{}, nullopt);
        }

        map_layers.emplace_back(0, 0, map_size, map_size, "Walls"s, "tilelayer"s, vector<map_object>{}, wall_data);
        map_layers.emplace_back(0, 0, map_size, map_size, "OpaqueDecor"s, "objectgroup"s, object_data, vector<uint32_t>{});

        map_component m(map_size, map_size, "test"s, {}, map_layers);

        auto fov = compute_fov_restrictive_shadowcasting(m, 4, 4, false);

        REQUIRE(fov.all());
    }

    SECTION("walls encage player") {
        const uint32_t map_size = 9;
        vector<map_layer> map_layers;
        vector<uint32_t> wall_data(map_size*map_size);
        vector<map_object> object_data(map_size*map_size);

        for(uint32_t i = 0; i < map_size*map_size; i++) {
            wall_data.push_back(0);
            object_data.emplace_back(0, 0, 0, 0, 0, 0, "", "", vector<map_property>{}, nullopt);
        }

        wall_data[3+3*map_size] = 1;
        wall_data[4+3*map_size] = 1;
        wall_data[5+3*map_size] = 1;
        wall_data[3+4*map_size] = 1;
        wall_data[5+4*map_size] = 1;
        wall_data[3+5*map_size] = 1;
        wall_data[4+5*map_size] = 1;
        wall_data[5+5*map_size] = 1;

        map_layers.emplace_back(0, 0, map_size, map_size, "Walls"s, "tilelayer"s, vector<map_object>{}, wall_data);
        map_layers.emplace_back(0, 0, map_size, map_size, "OpaqueDecor"s, "objectgroup"s, object_data, vector<uint32_t>{});

        map_component m(map_size, map_size, "test"s, {}, map_layers);

        auto fov = compute_fov_restrictive_shadowcasting(m, 4, 4, false);

        for(int y = 0; y < fov_diameter; y++) {
            for(int x = 0; x < fov_diameter; x++) {
                if(x == 4 && y == 4) {
                    REQUIRE(fov[x + y * fov_diameter] == true);
                } else {
                    REQUIRE(fov[x + y * fov_diameter] == false);
                }
            }
        }
    }

    SECTION("opaque encage player") {
        const uint32_t map_size = 9;
        vector<map_layer> map_layers;
        vector<uint32_t> wall_data(map_size*map_size);
        vector<map_object> object_data(map_size*map_size);

        for(uint32_t i = 0; i < map_size*map_size; i++) {
            wall_data.push_back(0);
            object_data.emplace_back(0, 0, 0, 0, 0, 0, "", "", vector<map_property>{}, nullopt);
        }

        object_data[3+3*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);
        object_data[4+3*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);
        object_data[5+3*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);
        object_data[3+4*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);
        object_data[5+4*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);
        object_data[3+5*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);
        object_data[4+5*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);
        object_data[5+5*map_size] = map_object(1, 0, 3, 3, 64, 64, "test"s, "test"s, vector<map_property>{}, nullopt);

        map_layers.emplace_back(0, 0, map_size, map_size, "Walls"s, "tilelayer"s, vector<map_object>{}, wall_data);
        map_layers.emplace_back(0, 0, map_size, map_size, "OpaqueDecor"s, "objectgroup"s, object_data, vector<uint32_t>{});

        map_component m(map_size, map_size, "test"s, {}, map_layers);

        auto fov = compute_fov_restrictive_shadowcasting(m, 4, 4, false);

        for(uint32_t y = 0; y < fov_diameter; y++) {
            for(uint32_t x = 0; x < fov_diameter; x++) {
                if(x == 4 && y == 4) {
                    REQUIRE(fov[x + y * fov_diameter] == true);
                } else {
                    REQUIRE(fov[x + y * fov_diameter] == false);
                }
            }
        }
    }

    SECTION("single object") {
        const uint32_t map_size = 9;
        vector<map_layer> map_layers;
        vector<uint32_t> wall_data(map_size*map_size);
        vector<map_object> object_data(map_size*map_size);

        for(uint32_t i = 0; i < map_size*map_size; i++) {
            wall_data.push_back(0);
            object_data.emplace_back(0, 0, 0, 0, 0, 0, "", "", vector<map_property>{}, nullopt);
        }

        wall_data[4+3*map_size] = 1;

        map_layers.emplace_back(0, 0, map_size, map_size, "Walls"s, "tilelayer"s, vector<map_object>{}, wall_data);
        map_layers.emplace_back(0, 0, map_size, map_size, "OpaqueDecor"s, "objectgroup"s, object_data, vector<uint32_t>{});

        map_component m(map_size, map_size, "test"s, {}, map_layers);

        auto fov = compute_fov_restrictive_shadowcasting(m, 4, 4, false);

        bitset<power(fov_diameter)> check_fov("111111111111111111111111111111111111111111111111101111111101111111000111111000111"s);

        REQUIRE(fov == check_fov);
    }

    SECTION("corners") {
        const uint32_t map_size = 9;
        vector<map_layer> map_layers;
        vector<uint32_t> wall_data(map_size*map_size);
        vector<map_object> object_data(map_size*map_size);

        for(uint32_t i = 0; i < map_size*map_size; i++) {
            wall_data.push_back(0);
            object_data.emplace_back(0, 0, 0, 0, 0, 0, "", "", vector<map_property>{}, nullopt);
        }

        map_layers.emplace_back(0, 0, map_size, map_size, "Walls"s, "tilelayer"s, vector<map_object>{}, wall_data);
        map_layers.emplace_back(0, 0, map_size, map_size, "OpaqueDecor"s, "objectgroup"s, object_data, vector<uint32_t>{});

        map_component m(map_size, map_size, "test"s, {}, map_layers);

        auto fov = compute_fov_restrictive_shadowcasting(m, 0, 0, false);
        bitset<power(fov_diameter)> check_fov("111110000111110000111110000111110000111110000000000000000000000000000000000000000"s);
        REQUIRE(fov == check_fov);

        fov = compute_fov_restrictive_shadowcasting(m, 8, 0, false);
        check_fov = bitset<power(fov_diameter)>("000011111000011111000011111000011111000011111000000000000000000000000000000000000"s);
        REQUIRE(fov == check_fov);

        fov = compute_fov_restrictive_shadowcasting(m, 0, 8, false);
        check_fov = bitset<power(fov_diameter)>("000000000000000000000000000000000000111110000111110000111110000111110000111110000"s);
        REQUIRE(fov == check_fov);

        fov = compute_fov_restrictive_shadowcasting(m, 8, 8, false);
        check_fov = bitset<power(fov_diameter)>("000000000000000000000000000000000000000011111000011111000011111000011111000011111"s);
        REQUIRE(fov == check_fov);
    }
}
