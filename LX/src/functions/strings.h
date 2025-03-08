#pragma once

std::string to_lower(const std::string& str);													// Function to convert a string to lower case
std::string to_upper(const std::string& str);													// Function to convert a string to upper case

std::vector<std::string> get_matches(std::string str, const std::string& ex);					// Function to get all matches of a regex expression
std::vector<uint64_t> find_all_occurrences(const std::string& str, const std::string& substr);	// Function to find all occurrences of a substring
std::string replace(const std::string& str, const std::string& from, const std::string& to);	// Function to replace a substring with another substring

std::string trim(std::string string, char character);											// Function to trim a string by a character
std::string trim_non_characters(const std::string& string);										// Function to trim a string by non characters
std::vector<std::string> split(const std::string& string, char split);							// Function to split a string by a character
std::string join(const std::vector<std::string>& vec, const std::string& substr);				// Function to join a vector of strings by a string
bool contains(std::vector<std::string> vec, const std::string& str);							// Function to check if a vector contains a string
std::string remove_zeros(float number);															// Function to remove trailing zeros from a string
bool contains_a_character(const std::string& str);												// Function to check if a string contains a character

// Function to check and convert a string to an int
template <typename T>
	requires std::is_arithmetic_v<T>
bool try_str_to_num(const std::string& str, T& out)
{
	try
	{
		if constexpr (std::is_same_v<T, int>)
			out = std::stoi(str);

		else if constexpr (std::is_same_v<T, float>)
			out = std::stof(str);

		else if constexpr (std::is_same_v<T, double>)
			out = std::stod(str);

		else if constexpr (std::is_same_v<T, long double>)
			out = std::stold(str);

		else if constexpr (std::is_same_v<T, long>)
			out = std::stol(str);

		else if constexpr (std::is_same_v<T, long long>)
			out = std::stoll(str);

		else if constexpr (std::is_same_v<T, unsigned long>)
			out = std::stoul(str);

		else if constexpr (std::is_same_v<T, unsigned long long>)
			out = std::stoull(str);

		else
		{
			assert(false && "Invalid Number Type");
			return false;
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

// Function to convert a float to a fixed amount of decimals
template <typename T>
	requires std::is_floating_point_v<T>
float to_fixed(T number, int amount = 2)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(amount) << number;

	if (T out; try_str_to_num(ss.str(), out))
		return out;

	return number;
}

// Function to convert a float to a fixed amount of decimals without trailing zeros
template <typename T>
	requires std::is_floating_point_v<T>
std::string to_fixed_no_zero(T number, int amount = 2)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(amount) << number;

	std::string str_number = ss.str();
	str_number.erase(str_number.find_last_not_of('0') + 1, std::string::npos);
	str_number.erase(str_number.find_last_not_of('.') + 1, std::string::npos);
	return str_number;
}
