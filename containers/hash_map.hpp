#pragma once

#include <vector>
#include <iterator>
#include <type_traits>

class hash_map
{
public:
	using key_type = int;
	using value_type = int;

	struct key_value_pair
	{
		key_type key;
		value_type value;
	};

private:

	struct slot_type
	{
		enum class state_type
		{
			full,
			empty,
			deleted
		};

		std::aligned_storage_t<sizeof(key_value_pair), alignof(key_value_pair)> content{};
		state_type state = state_type::empty;

		slot_type() = default;
		slot_type(slot_type&& other) noexcept = default;
		slot_type(const slot_type& other) = default;
		~slot_type() noexcept = default;

		template<typename... Args>
		void set(hash_map::key_type key, Args&&... args)
		{
			pair().key = key;
			pair().value = hash_map::value_type(std::forward<Args>(args)...);
			state = state_type::full;
		}

		slot_type& operator=(slot_type&& other) noexcept { return *this; }
		slot_type& operator=(const slot_type& other) { return *this; }

		key_value_pair& pair() noexcept { return reinterpret_cast<key_value_pair&>(content); };
		const key_value_pair& pair() const noexcept { return reinterpret_cast<const key_value_pair&>(content); };
	};

	using storage_type = std::vector<slot_type>;

public:
	using size_type = typename storage_type::size_type;

	class iterator;

	class const_iterator
	{
	public:
		friend class hash_map;
		friend class const_iterator;

		using iterator_category = std::forward_iterator_tag;
		using value_type = key_value_pair;
		using reference = const key_value_pair&;
		using pointer = const key_value_pair*;
		using differnce_type = std::ptrdiff_t;

	private:
		typename hash_map::storage_type::const_iterator slot;

	public:
		const_iterator() = default;
		const_iterator(typename hash_map::storage_type::const_iterator first) noexcept {}
		const_iterator(iterator other) noexcept {}


		const_iterator& operator++() noexcept { return *this; }
		const_iterator operator++(int) noexcept { return *this; }

		reference operator*() const noexcept { return slot->pair(); }
		pointer operator->() const noexcept { return &slot->pair(); }

		bool operator==(const_iterator other) const noexcept { return false; }
		bool operator!=(const_iterator other) const noexcept { return false; }
	};

	class iterator
	{
	public:
		friend class hash_map;
		friend class const_iterator;

		using iterator_category = std::forward_iterator_tag;
		using value_type = key_value_pair;
		using reference = key_value_pair&;
		using pointer = key_value_pair*;
		using differnce_type = std::ptrdiff_t;

	private:
		typename hash_map::storage_type::iterator slot;

	public:
		iterator() = default;
		iterator(typename hash_map::storage_type::iterator first) : slot(first) {}

		iterator& operator++() noexcept { return *this; }
		iterator operator++(int) noexcept {
			return {};
		}

		reference operator*() const noexcept { return slot->pair(); }
		pointer operator->() const noexcept { return &slot->pair(); }

		bool operator==(iterator other) const noexcept { return slot == other.slot; }
		bool operator!=(iterator other) const noexcept { return !(*this == other); }
	};

private:

	storage_type storage;

public:
	hash_map() noexcept = default;

	// mutators

	iterator insert(key_type key, const value_type& value) { return emplace(key, value); }
	iterator insert(key_type key, value_type&& value) { return emplace(key, std::move(value)); }

	template<typename... Args>
	iterator emplace(key_type key, Args&&... args)
	{
		auto slot = slot_type{};
		slot.set(key, std::forward<Args>(args)...);
		storage.push_back(slot);
		return begin();
	}

	void erase(const_iterator) {}
	void erase(key_type) {}

	iterator find(key_type) noexcept { return { --storage.end() }; }
	const_iterator find(key_type) const noexcept { return {}; }

	// queries

	bool empty() const noexcept { return storage.empty(); }
	size_type size() const noexcept { return storage.size(); }
	size_type capacity() const noexcept { return storage.capacity(); }

	double load_factor() const noexcept { return 0.0; }
	double max_load_factor() const noexcept { return 0.0; }

	// iterator

	iterator begin() noexcept { return {storage.begin()}; }
	const_iterator begin() const noexcept { return {}; }
	const_iterator cbegin() const noexcept { return {}; }

	iterator end() noexcept { return {storage.end()}; }
	const_iterator end() const noexcept { return {}; }
	const_iterator cend() const noexcept { return {}; }

private:
	int hash(key_type) const noexcept { return 0; }
	void rehash(size_type) {}
};
