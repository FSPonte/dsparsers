#pragma once

// ----- Dependencies -----
#include <string>
#include <unordered_map>

class dotenv
{
public:

	/**
	 * @brief Default constructor
	*/
	dotenv(void) noexcept(true);

	/**
	 * @brief Parameterized constructor
	 * @param file_name Name of the .env file
	*/
	dotenv(const std::string&) noexcept(true);

	/**
	 * @brief Load a .env file
	 * @param file_name Name of the .env file
	 * @throw Failed to open the file
	*/
	void load(const std::string&) noexcept(false);

	/**
	 * @brief Get the size
	 * @return Number of variables
	*/
	inline size_t size(void) const noexcept(true);

	/**
	 * @brief Get operator
	 * @param key Key
	 * @return Value
	*/
	inline std::string operator[](const std::string& key) noexcept(true);

private:

	std::unordered_map<std::string, std::string> _map;

	/**
	 * @brief Expand variables
	 * @param value Value
	 * @return Expanded value string
	*/
	std::string expand(const std::string& value) const noexcept(true);
};

// Template file
#include "dotenv.tpp"
