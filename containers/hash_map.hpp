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

		template<typename... Args>
		key_value_pair(key_type key, Args&&... args)
			: key{ key },
			  value{ std::forward<Args>(args)... } {}
	};

private:
	struct slot_type
	{
		using state_type = std::int8_t;

		static constexpr state_type none = 0;
		static constexpr state_type empty = 1;
		static constexpr state_type full = 2;
		static constexpr state_type deleted = 4;

		std::aligned_storage_t<sizeof(key_value_pair), alignof(key_value_pair)> content{};
		state_type state = empty;

		slot_type() = default;
		slot_type(slot_type&& other) noexcept = default;
		slot_type(const slot_type& other) = default;
		~slot_type() noexcept = default;

		template<typename... Args>
		void set(hash_map::key_type key, Args&&... args)
		{
			new(&content) key_value_pair(key, std::forward<Args>(args)...);
			state = full;
		}

		void release()
		{
			pair().~key_value_pair();
			state = deleted;
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

		const_iterator(typename hash_map::storage_type::const_iterator first) noexcept
			: slot(first) {}

		const_iterator(iterator other) noexcept
			: slot(other.slot) {}


		const_iterator& operator++() noexcept { return *this; }
		const_iterator operator++(int) noexcept { return *this; }

		reference operator*() const noexcept { return slot->pair(); }
		pointer operator->() const noexcept { return &slot->pair(); }

		bool operator==(const_iterator other) const noexcept { return slot == other.slot; }
		bool operator!=(const_iterator other) const noexcept { return !(*this == other); }
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
		using difference_type = std::ptrdiff_t;

	private:
		typename hash_map::storage_type::iterator slot;
		typename hash_map::storage_type::iterator end;

	public:
		iterator() = default;

		iterator(typename hash_map::storage_type::iterator first, typename hash_map::storage_type::iterator last)
			: slot(first), end(last) {}

		iterator& operator++() noexcept
		{
			do {
				++slot;
			} while (slot != end && slot->state != slot_type::full);

			return *this;
		}

		iterator operator++(int) noexcept
		{
			return *this;
		}

		reference operator*() const noexcept { return slot->pair(); }
		pointer operator->() const noexcept { return &slot->pair(); }

		bool operator==(iterator other) const noexcept { return slot == other.slot; }
		bool operator!=(iterator other) const noexcept { return !(*this == other); }
	};

private:

	storage_type storage;

	size_type count = 0;

public:
	hash_map() noexcept
		: storage(10) { }

	// mutators

	iterator insert(key_type key, const value_type& value) { return emplace(key, value); }
	iterator insert(key_type key, value_type&& value) { return emplace(key, std::move(value)); }

	template<typename... Args>
	iterator emplace(key_type key, Args&&... args)
	{
		auto match = find(key);
		if(match != end())
		{
			match.slot->set(key, std::forward<Args>(args)...);
		}
		else
		{
			if((size() + 1.0) / capacity() > max_load_factor())
			{
				rehash(capacity() * 2);
			}
			auto slot = std::next(storage.begin(), hash(key));
			slot = probe(slot, slot_type::deleted | slot_type::empty, slot_type::full);

			slot->set(key, std::forward<Args>(args)...);
			++count;
		}

		return begin();
	}

	void erase(const_iterator iter)
	{
		if(iter != end()) const_cast<slot_type&>(*iter.slot).release();
		else throw std::out_of_range{ "cannot delete out-of-range iterator" };
	}

	void erase(key_type key)
	{
		erase(find(key));
	}

	iterator find(key_type key)
	{
		if(empty()) return end();

		auto iter = std::next(storage.begin(), hash(key)); // storage.begin();

		iter = probe(iter, slot_type::full, slot_type::deleted);
		if(iter->state == slot_type::empty) return end();

		while(iter->pair().key != key)
		{
			iter = probe(std::next(iter), slot_type::full, slot_type::deleted);
			if(iter->state == slot_type::empty) return end();
		}

		return iterator{ iter , storage.end()};
	}

	const_iterator find(key_type) const noexcept { return {}; }

	// queries

	bool empty() const noexcept { return count == 0; }
	size_type size() const noexcept { return count; }
	size_type capacity() const noexcept { return storage.capacity(); }

	double load_factor() const noexcept { return double(size()) / capacity(); }
	double max_load_factor() const noexcept { return 0.5; }

	// iterator

	iterator begin() noexcept
	{
		return iterator{
			std::find_if(
				std::begin(storage),
				std::end(storage),
				[&](auto& slot) { return slot.state == slot_type::full; }),
			storage.end()
		};
	}

	const_iterator begin() const noexcept
	{
		return {
			std::find_if(
				std::begin(storage),
				std::end(storage),
				[&](auto& slot) { return slot.state == slot_type::full; })
		};
	}

	const_iterator cbegin() const noexcept { return begin(); }

	iterator end() noexcept { return iterator{ storage.end(), storage.end() }; }
	const_iterator end() const noexcept { return { storage.end() }; }
	const_iterator cend() const noexcept { return end(); }

private:
	int hash(key_type) const noexcept { return 0; }

	void rehash(size_type newCapacity)
	{
		auto copy = std::move(storage);
		storage.clear();
		count = 0;
		storage.resize(newCapacity);
		for(auto& slot : copy)
		{
			if(slot.state == slot_type::full)
			{
				insert(slot.pair().key, std::move(slot.pair().value));
			}
		}
	}

	typename storage_type::iterator probe(
		typename storage_type::iterator slot,
		slot_type::state_type expected,
		slot_type::state_type skip = slot_type::none)
	{
		if(slot == storage.end()) slot = storage.begin();
		auto initial = slot;

		if((slot->state & expected) == slot_type::none)
		{
			do
			{
				++slot;
				if(slot == storage.end()) slot = storage.begin();
				if(slot == initial) throw std::out_of_range("infinite probe");
			}
			while((slot->state & skip) != slot_type::none);
		}

		return slot;
	}
};
