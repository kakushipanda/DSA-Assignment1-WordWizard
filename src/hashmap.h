#pragma once

#include <string>
#include <vector>
#include <list>
#include <array>
#include <fstream>

// Closed addressing hash map for storing anagram lists
class AnagramHashMap {
private:
	struct AnagramEntry {
		std::string value;

		bool operator==(std::string const& other) const {
			if (value.size() != other.size()) {
				return false;
			}

			// sorting would be simpler but less efficient
			// n log(n) vs n+26
			std::array<int, 26> freq1{}, freq2{};
			for (size_t i{}; i < value.size(); i++) {
				unsigned char idx1 = (value[i] | 32) - 'a';
				unsigned char idx2 = (other[i] | 32) - 'a';

				freq1[static_cast<unsigned>(-(idx1 < 26)) & idx1] += static_cast<unsigned>(-(idx1 < 26)) & 1;
				freq2[static_cast<unsigned>(-(idx2 < 26)) & idx2] += static_cast<unsigned>(-(idx2 < 26)) & 1;
			}
			return freq1 == freq2;
		}

		bool compareString(const std::string& other) const {
			return value == other;
		}
	};
private:

	constexpr static float load_factor = 0.75f;
	size_t size;
	// The hash table: each bucket holds a vector of [anagram, string] key-value pairs
	std::vector<std::list<AnagramEntry>> table;

	// deliberately cause collisions for anagram keys
	size_t anagramHash(const std::string& key) const {
		std::array<int, 26> freq{};
		for (unsigned char c : key) {
			c |= 32; // make lowercase
			
			// will underflow to a large value if c < 'a'
			c -= 'a';

			// Add 1 only if valid
			// if not valid adds 0 to freq[c]
			freq[static_cast<unsigned>(-(c < 26)) & c] += static_cast<unsigned>( - (c < 26)) & 1;
		}

		// Hash the frequency array
		std::size_t seed = 0;
		for (int count : freq)
		{
			seed ^= std::hash<int>{}(count)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}

public:
	AnagramHashMap() = delete;

	AnagramHashMap(std::string const& filename) : table() {
		std::ifstream input(filename);
		std::vector<std::string> values;
		while (input) {
			std::string buf;
			input >> buf;
			values.push_back(buf);
		}
		table.resize(static_cast<size_t>(values.size() * load_factor) + 1);
		for(std::string& val : values) {
			insert(val);
		}
	}

	void insert(const std::string& value) {
		size_t index = anagramHash(value) % table.size();
		table[index].push_back(AnagramEntry{ value });
		size++;
	}

	std::vector<std::string> getAnagramList(const std::string& key) const {
		size_t index = anagramHash(key) % table.size();
		std::vector<std::string> result;
		for(AnagramEntry entry: table[index]){
			if(entry==key) {
				result.push_back(entry.value);
			}
		}
		return result;
	}

	std::vector<std::string> operator[](const std::string& key) {
		return getAnagramList(key);
	}

	size_t count(const std::string& key) const {
		return size;
	}
};