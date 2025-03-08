#include "strings.h"

std::string to_lower(const std::string& str)
{
	std::string result{ str };
	std::ranges::transform(result, result.data(), [](const char c) { return tolower(c); });
	return result;
}

std::string to_upper(const std::string& str)
{
	std::string result{ str };
	std::ranges::transform(result, result.data(), [](const char c) { return toupper(c); });
	return result;
}


std::vector<std::string> get_matches(std::string str, const std::string& ex)
{
	std::vector<std::string> matches{};
	const std::regex expression{ ex };
	std::sregex_iterator iterator{ str.begin(), str.end(), expression };
	const std::sregex_iterator end{};
	while (iterator != end)
	{
		matches.push_back(iterator->str());
		++iterator;
	}
	return matches;
}

std::vector<uint64_t> find_all_occurrences(const std::string& str, const std::string& substr)
{
	std::vector<uint64_t> indexes{};
	uint64_t index{};
	while ((index = str.find(substr, index)) != std::string::npos)
	{
		indexes.push_back(index);
		index += substr.length();
	}
	return indexes;
}

std::string replace(const std::string& str, const std::string& from, const std::string& to)
{
	std::string result{ str };
	size_t start_pos = 0;
	while ((start_pos = result.find(from, start_pos)) != std::string::npos)
	{
		result.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return result;
}


std::string trim(std::string string, const char character)
{
	string.erase(std::ranges::remove(string, character).begin(), string.end());
	return string;
}

std::string trim_non_characters(const std::string& string)
{
	auto start = std::ranges::find_if(string, [](char c) { return std::isalnum(c); });
	auto end = std::ranges::find_if(string | std::views::reverse, [](char c) { return std::isalnum(c); });

	return std::string(start, end.base());
}

std::vector<std::string> split(const std::string& string, const char split)
{
	std::vector<std::string> output{};
	size_t previous_position{};
	size_t position = string.find(split);
	while (position != std::string::npos)
	{
		std::string substring{ string.substr(previous_position, position - previous_position) };
		output.push_back(substring);
		previous_position = position + 1;
		position = string.find(split, previous_position);
	}
	const std::string last_substring{ string.substr(previous_position) };
	output.push_back(last_substring);
	return output;
}

std::string join(const std::vector<std::string>& vec, const std::string& substr)
{
	std::string result{};
	for (size_t i = 0; i < vec.size(); i++)
	{
		result += vec[i];
		if (i != vec.size() - 1)
			result += substr;
	}
	return result;
}

bool contains(std::vector<std::string> vec, const std::string& str)
{
	return std::ranges::find(vec, str) != vec.end();
}

bool contains_a_character(const std::string& str)
{
	return std::ranges::any_of(str, [](const char c) { return std::isalpha(c); });
}

std::string remove_zeros(float number)
{
	std::string str = std::to_string(number);
	str.erase(str.find_last_not_of('0') + 1, std::string::npos);
	str.erase(str.find_last_not_of('.') + 1, std::string::npos);
	return str;
}
