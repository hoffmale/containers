#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "hash_map.hpp"
#include <algorithm>

using hash_map_impl = hash_map<int, int, std::hash<int>>;

TEST_CASE("An empty hash map", "[hash_map]")
{
	hash_map_impl map{};
	const auto& cmap = map;

	REQUIRE(map.empty());
	REQUIRE(map.size() == 0u);
	REQUIRE(map.capacity() >= 0u);

	REQUIRE(map.begin() == map.end());
	REQUIRE(cmap.begin() == cmap.end());

	REQUIRE(map.cbegin() == cmap.begin());
	REQUIRE(map.cend() == cmap.end());

	REQUIRE(map.load_factor() == 0.0);
	REQUIRE(map.max_load_factor() >= map.load_factor());

	SECTION("cannot find values in empty map") {
		REQUIRE(map.find(1) == map.end());
	}

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

	SECTION("inserting an element increases the load factor")
	{
		map.insert(1, 2);

		REQUIRE(map.load_factor() == Approx(1.0 / map.capacity()));
		REQUIRE(map.max_load_factor() >= map.load_factor());
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
		REQUIRE(typename hash_map_impl::const_iterator{iter} != cmap.end());
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

	SECTION("Inserting multiple elements returns correct iterators")
	{
		static const auto key_one = 1;
		static const auto value_one = 2;
		static const auto key_two = 3;
		static const auto value_two = 4;

		auto iter_one = map.insert(key_one, value_one);
		auto iter_two = map.insert(key_two, value_two);

		REQUIRE(iter_one != iter_two);

		REQUIRE(iter_one->key == key_one);
		REQUIRE(iter_one->value == value_one);

		REQUIRE(iter_two->key == key_two);
		REQUIRE(iter_two->value == value_two);
	}
	SECTION("Inserting multiple elements means either can be found")
	{
		static const auto key_one = 1;
		static const auto value_one = 2;
		static const auto key_two = 3;
		static const auto value_two = 4;

		map.insert(key_one, value_one);
		map.insert(key_two, value_two);

		auto iter_one = map.find(key_one);
		auto iter_two = map.find(key_two);

		REQUIRE(iter_one != iter_two);

		REQUIRE(iter_one->key == key_one);
		REQUIRE(iter_one->value == value_one);

		REQUIRE(iter_two->key == key_two);
		REQUIRE(iter_two->value == value_two);
	}

	SECTION("hash map grows storage if load grows too large")
	{
		auto max_load = int( map.max_load_factor() * map.capacity() );
		auto cap = map.capacity();

		for(auto i = 0; i < max_load; ++i)
		{
			map.insert(i, 1);
			CHECK(map.load_factor() <= Approx(map.max_load_factor()));
			CHECK(map.capacity() == cap);
		}

		map.insert(max_load + 1, 1);
		REQUIRE(map.load_factor() <= Approx(map.max_load_factor()));
		REQUIRE(map.capacity() > cap);
	}

	SECTION("hash map grows storage if load grows too large")
	{
		auto max_load = int(map.max_load_factor() * map.capacity());
		auto cap = map.capacity();

		for (auto i = 0; i < max_load; ++i)
		{
			map.insert(i, 1);
			CHECK(map.load_factor() <= Approx(map.max_load_factor()));
			CHECK(map.capacity() == cap);
		}

		map.erase(0);
		map.insert(0, 2);

		REQUIRE(map.capacity() == cap);
	}
}

TEST_CASE("Non empty hash map", "[hash_map]")
{
	hash_map_impl map{};
	map.insert(0, 1);
	map.insert(1, 2);
	map.insert(2, 3);
	map.insert(3, 4);
	map.insert(4, 5);
	map.insert(5, 6);

	SECTION("Removing an element decreases size")
	{
		auto size = map.size();
		map.erase(3);

		REQUIRE(map.size() == size - 1);
	}

	SECTION("Remove element cannot be found again")
	{
		map.erase(2);
		REQUIRE(map.find(2) == map.end());
	}

	SECTION("remove element by iterator")
	{
		auto match = map.find(4);
		map.erase(match);

		REQUIRE(map.find(4) == map.end());
	}

	SECTION("remove a nonexistant element")
	{
		REQUIRE_THROWS(map.erase(7));
	}

	SECTION("iteration only traverses existing elements")
	{
		auto dist = std::distance(map.begin(), map.end());
		REQUIRE(dist == map.size());
	}

	SECTION("Find all keys iterated")
	{
		int arr[6] = { 0,0,0,0,0,0 };
		for (auto &slot: map) {
			++arr[slot.key];
		}
		for (int i = 0; i < 6; i++) {
			REQUIRE(arr[i] == 1);
		}
	}

	SECTION("Delete key, and reinsert the same key with the value")
	{
		map.erase(5);
		map.insert(5, 6);
	}
}

static int counter = 0;

struct my_class
{
	my_class() { ++counter; }
	my_class(my_class&&) = default;
	my_class(const my_class&) = default;
	~my_class() { --counter; }

	my_class& operator=(const my_class&) = default;
	my_class& operator=(my_class&&) = default;
};

TEST_CASE("generic hash map")
{
	REQUIRE(counter == 0);

	SECTION("calls allocators and deallocators correctly")
	{
		{
			hash_map<int, my_class> map{};

			map.emplace(1);
			REQUIRE(counter == 1);
		}

		REQUIRE(counter == 0);
	}
}
