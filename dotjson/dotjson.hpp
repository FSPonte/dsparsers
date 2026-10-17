#pragma once

// ----- Dependencies -----
#include <string>
#include <unordered_map>

class dotjson
{
public:

	/**
	 * @brief Default constructor
	*/
	dotjson(void) noexcept(true);

	/**
	 * @brief Parameterized constructor
	 * @param file_name Name of the json file
	 * @throw Failed to open or parse the file
	*/
	dotjson(const std::string&) noexcept(false);

	/**
	 * @brief Load a json file
	 * @param file_name Name of the json file
	 * @throw Failed to open the file or invalid json
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
	inline std::string operator[](const std::string&) noexcept(true);

private:

	using map_t = std::unordered_map<std::string, std::string>;

	map_t _map;
	constexpr static size_t MAX_DEPTH = 256; // Maximum nesting depth accepted by the parser

	/**
	 * @brief Throw a parse error
	 * @param message Error description
	 * @param pos Offset in the text where the error was found
	 * @throw Always
	*/
	static void fail(const char[], size_t) noexcept(false);

	/**
	 * @brief Skip spaces, tabs and line breaks
	 * @param text Json text
	 * @param pos Current position (updated)
	*/
	static void skip_whitespace(const std::string&, size_t&) noexcept(true);

	/**
	 * @brief Parse any json value and store its leaves in out
	 * @param text Json text
	 * @param pos Current position (updated)
	 * @param path Flattened key of this value
	 * @param depth Current nesting depth
	 * @param out Destination map
	*/
	static void parse_value(const std::string&, size_t&, const std::string&, size_t, map_t&) noexcept(false);

	/**
	 * @brief Parse an object: { "key": value, ... }
	*/
	static void parse_object(const std::string&, size_t&, const std::string&, size_t, map_t&) noexcept(false);

	/**
	 * @brief Parse an array: [ value, ... ]
	*/
	static void parse_array(const std::string&, size_t&, const std::string&, size_t, map_t&) noexcept(false);

	/**
	 * @brief Parse a string (position must be on the opening quote)
	 * @return Unescaped string
	*/
	static std::string parse_string(const std::string&, size_t&) noexcept(false);

	/**
	 * @brief Parse a number
	 * @return Number as written in the file
	*/
	static std::string parse_number(const std::string&, size_t&) noexcept(false);

	/**
	 * @brief Parse a literal (true, false or null)
	 * @param literal Expected literal
	 * @return The literal
	*/
	static std::string parse_literal(const std::string&, size_t&, const std::string&) noexcept(false);

	/**
	 * @brief Parse 4 hexadecimal digits (after "\u")
	 * @return Code unit
	*/
	static unsigned parse_hex4(const std::string&, size_t&) noexcept(false);

	/**
	 * @brief Append a unicode code point to a string as UTF-8
	 * @param str Destination string
	 * @param code_point Unicode code point
	*/
	static void append_utf8(std::string&, unsigned) noexcept(false);
};

// Template file
#include "dotjson.tpp"
