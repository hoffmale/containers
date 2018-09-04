#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "hash_map.hpp"

TEST_CASE("An empty hash map", "[hash_map]")
{
	hash_map map{};

	REQUIRE(map.empty());
	REQUIRE(map.size() == 0u);
	REQUIRE(map.capacity() >= 0u);

	/*SECTION("insert an element")
	{
		auto iter = map.insert(1, 2);

		REQUIRE(iter != map.end());
		REQUIRE(iter->key == 1);
		REQUIRE(iter->value == 2);

		REQUIRE(!map.empty());
		REQUIRE(map.size() == 1u);
		REQUIRE(map.capacity() >= 1u);

	}*/

	SECTION("inserting an element makes the map not empty")
	{
		map.insert(2, 3);

		REQUIRE(!map.empty());
	}

	SECTION("inserting an element increases the size")
	{
		map.insert(1, 2);

		REQUIRE(map.size() == 1u);
	}

	SECTION("Inserting an element means it can be retrieved")
	{
		map.insert(2, 4);
		REQUIRE(map.find(2) != map.end());
	}
}

