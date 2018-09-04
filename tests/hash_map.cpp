#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "hash_map.hpp"

TEST_CASE("An empty hash map", "[hash_map]")
{
	hash_map map{};

	REQUIRE(map.empty());
	REQUIRE(map.size() == 0u);
	REQUIRE(map.capacity() >= 0u);

	REQUIRE(map.begin() == map.end());

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

		auto iter = map.find(2);

		REQUIRE(iter != map.end());
		REQUIRE(iter->key == 2);
		REQUIRE(iter->value == 4);
	}

	SECTION("Inserting an element returns an iterator to the newly inserted key-value-pair")
	{
		auto iter = map.insert(3, 5);

		REQUIRE(iter != map.end());
		REQUIRE(iter->key == 3);
		REQUIRE(iter->value == 5);
	}

	SECTION("Inserting second element with same key overwrites existing value")
	{
		const auto key = 2;
		const auto value_one = 4;
		const auto value_two = 5;

		map.insert(key, value_one);
		map.insert(key, value_two);

		REQUIRE(map.find(key)->value == value_two);
		REQUIRE(map.size() == 1);
	}
}

