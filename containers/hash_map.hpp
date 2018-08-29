#pragma once

#include <vector>
#include <iterator>
#include <type_traits>

class hash_map
{
	using key_type = int;
	using value_type = int;

	struct key_value_pair
	{
		key_type key;
		value_type type;
	};

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

		slot_type(slot_type&& other) noexcept;
		slot_type(const slot_type& other);
		~slot_type() noexcept;

		slot_type& operator=(slot_type&& other) noexcept;
		slot_type& operator=(const slot_type& other);

		key_value_pair& pair() const noexcept;
	};

	using storage_type = std::vector<slot_type>;
	using size_type = typename storage_type::size_type;

	class iterator;

	class const_iterator
	{
	public:
		friend class hash_map;
		friend class const_iterator;

		using iterator_category = std::forward_iterator_tag;
		using value_type = key_value_pair;
		using reference = key_value_pair & ;
		using pointer = key_value_pair * ;
		using differnce_type = std::ptrdiff_t;

	private:
		typename storage_type::const_iterator slot{};

	public:
		const_iterator() noexcept = default;
		const_iterator(typename storage_type::const_iterator first) noexcept;
		const_iterator(iterator other) noexcept;


		const_iterator& operator++() noexcept;
		const_iterator operator++(int) noexcept;

		reference operator*() const noexcept;
		pointer operator->() const noexcept;

		bool operator==(const_iterator other) const noexcept;
		bool operator!=(const_iterator other) const noexcept;
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
		typename storage_type::iterator slot;

	public:
		iterator() = default;
		iterator(typename storage_type::iterator first);

		iterator& operator++() noexcept;
		iterator operator++(int) noexcept;

		reference operator*() const noexcept;
		pointer operator->() const noexcept;

		bool operator==(iterator other) const noexcept;
		bool operator!=(iterator other) const noexcept;
	};

	storage_type storage;

public:
	hash_map() noexcept = default;

	// mutators

	iterator insert(key_type, const value_type&);
	iterator insert(key_type, value_type&&);

	template<typename... Args>
	iterator emplace(key_type, Args&&... args);

	void erase(const_iterator);
	void erase(key_type);

	iterator find(key_type) noexcept;
	const_iterator find(key_type) const noexcept;

	// queries

	bool empty() const noexcept;
	size_type size() const noexcept;
	size_type capacity() const noexcept;

	double load_factor() const noexcept;
	double max_load_factor() const noexcept;

	// iterator

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;

	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

private:
	int hash(key_type) const noexcept;
	void rehash(size_type);
};
