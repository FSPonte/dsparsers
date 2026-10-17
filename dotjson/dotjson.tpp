#pragma once

// ----- Dependencies -----
#include <fstream>
#include <sstream>

dotjson::dotjson(void) noexcept(true)
: _map(map_t())
{}

dotjson::dotjson(const std::string& file_name) noexcept(false)
{
	this->load(file_name);
}

void dotjson::load(const std::string& file_name) noexcept(false)
{
	std::ifstream file(file_name, std::ios::binary);

	if (!file)
		throw std::runtime_error("[dotjson] Failed to open the file");

	std::ostringstream buffer;
	buffer << file.rdbuf();

	const std::string text = buffer.str();
	size_t pos = 0;

	// Skip UTF-8 BOM
	if (text.compare(0, 3, "\xEF\xBB\xBF") == 0)
		pos = 3;

	// Parse into a temporary map so a broken file doesn't leave partial data
	map_t parsed;

	dotjson::parse_value(text, pos, std::string(), 0, parsed);
	dotjson::skip_whitespace(text, pos);

	if (pos != text.size())
		dotjson::fail("Unexpected data after the json value", pos);

	for (auto& pair : parsed)
		this->_map[pair.first] = std::move(pair.second);
}

void dotjson::fail(const char* message, size_t pos) noexcept(false)
{
	throw std::runtime_error(std::string("[dotjson] ") + message + " at offset " + std::to_string(pos));
}

void dotjson::skip_whitespace(const std::string& text, size_t& pos) noexcept(true)
{
	while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t' || text[pos] == '\n' || text[pos] == '\r'))
		++pos;
}

void dotjson::parse_value(const std::string& text, size_t& pos, const std::string& path, size_t depth, map_t& out) noexcept(false)
{
	dotjson::skip_whitespace(text, pos);

	if (pos >= text.size())
		dotjson::fail("Unexpected end of input", pos);

	if (depth > MAX_DEPTH)
		dotjson::fail("Maximum nesting depth exceeded", pos);

	const char c = text[pos];

	if (c == '{')
		dotjson::parse_object(text, pos, path, depth, out);
	else if (c == '[')
		dotjson::parse_array(text, pos, path, depth, out);
	else if (c == '"')
		out[path] = dotjson::parse_string(text, pos);
	else if (c == 't')
		out[path] = dotjson::parse_literal(text, pos, "true");
	else if (c == 'f')
		out[path] = dotjson::parse_literal(text, pos, "false");
	else if (c == 'n')
		out[path] = dotjson::parse_literal(text, pos, "null");
	else if (c == '-' || (c >= '0' && c <= '9'))
		out[path] = dotjson::parse_number(text, pos);
	else
		dotjson::fail("Unexpected character", pos);
}

void dotjson::parse_object(const std::string& text, size_t& pos, const std::string& path, size_t depth, map_t& out) noexcept(false)
{
	++pos; // skip '{'
	dotjson::skip_whitespace(text, pos);

	if (pos < text.size() && text[pos] == '}')
	{
		++pos;
		return;
	}

	while (true)
	{
		dotjson::skip_whitespace(text, pos);

		if (pos >= text.size() || text[pos] != '"')
			dotjson::fail("Expected a string key", pos);

		const std::string key = dotjson::parse_string(text, pos);

		dotjson::skip_whitespace(text, pos);

		if (pos >= text.size() || text[pos] != ':')
			dotjson::fail("Expected ':'", pos);

		++pos; // skip ':'

		dotjson::parse_value(text, pos, path.empty() ? key : path + "." + key, depth + 1, out);
		dotjson::skip_whitespace(text, pos);

		if (pos >= text.size())
			dotjson::fail("Unterminated object", pos);

		if (text[pos] == ',')
		{
			++pos;
			continue;
		}

		if (text[pos] == '}')
		{
			++pos;
			return;
		}

		dotjson::fail("Expected ',' or '}'", pos);
	}
}

void dotjson::parse_array(const std::string& text, size_t& pos, const std::string& path, size_t depth, map_t& out) noexcept(false)
{
	++pos; // skip '['
	dotjson::skip_whitespace(text, pos);

	if (pos < text.size() && text[pos] == ']')
	{
		++pos;
		return;
	}

	for (size_t i = 0; true; ++i)
	{
		dotjson::parse_value(text, pos, path + "[" + std::to_string(i) + "]", depth + 1, out);
		dotjson::skip_whitespace(text, pos);

		if (pos >= text.size())
			dotjson::fail("Unterminated array", pos);

		if (text[pos] == ',')
		{
			++pos;
			continue;
		}

		if (text[pos] == ']')
		{
			++pos;
			return;
		}

		dotjson::fail("Expected ',' or ']'", pos);
	}
}

std::string dotjson::parse_string(const std::string& text, size_t& pos) noexcept(false)
{
	++pos; // skip opening '"'

	std::string result;

	while (true)
	{
		if (pos >= text.size())
			dotjson::fail("Unterminated string", pos);

		char c = text[pos++];

		if (c == '"')
			return result;

		if (static_cast<unsigned char>(c) < 0x20)
			dotjson::fail("Control character in string", pos - 1);

		if (c != '\\')
		{
			result += c;
			continue;
		}

		if (pos >= text.size())
			dotjson::fail("Unterminated string", pos);

		c = text[pos++];

		switch (c)
		{
			case '"':  result += '"';  break;
			case '\\': result += '\\'; break;
			case '/':  result += '/';  break;
			case 'b':  result += '\b'; break;
			case 'f':  result += '\f'; break;
			case 'n':  result += '\n'; break;
			case 'r':  result += '\r'; break;
			case 't':  result += '\t'; break;
			case 'u':
			{
				unsigned code_point = dotjson::parse_hex4(text, pos);

				if (code_point >= 0xD800 && code_point <= 0xDBFF)
				{ // high surrogate, must be followed by a low surrogate
					if (pos + 1 >= text.size() || text[pos] != '\\' || text[pos + 1] != 'u')
						dotjson::fail("Unpaired surrogate", pos);

					pos += 2;

					const unsigned low = dotjson::parse_hex4(text, pos);

					if (low < 0xDC00 || low > 0xDFFF)
						dotjson::fail("Invalid low surrogate", pos);

					code_point = 0x10000 + ((code_point - 0xD800) << 10) + (low - 0xDC00);
				}
				else if (code_point >= 0xDC00 && code_point <= 0xDFFF)
					dotjson::fail("Unpaired surrogate", pos);

				dotjson::append_utf8(result, code_point);
				break;
			}
			default:
				dotjson::fail("Invalid escape sequence", pos - 1);
		}
	}
}

std::string dotjson::parse_number(const std::string& text, size_t& pos) noexcept(false)
{
	const size_t start = pos;

	auto is_digit = [&text](size_t i) { return i < text.size() && text[i] >= '0' && text[i] <= '9'; };

	if (text[pos] == '-')
		++pos;

	if (pos < text.size() && text[pos] == '0')
		++pos;
	else if (is_digit(pos))
		while (is_digit(pos))
			++pos;
	else
		dotjson::fail("Invalid number", pos);

	if (pos < text.size() && text[pos] == '.')
	{
		++pos;

		if (!is_digit(pos))
			dotjson::fail("Invalid number", pos);

		while (is_digit(pos))
			++pos;
	}

	if (pos < text.size() && (text[pos] == 'e' || text[pos] == 'E'))
	{
		++pos;

		if (pos < text.size() && (text[pos] == '+' || text[pos] == '-'))
			++pos;

		if (!is_digit(pos))
			dotjson::fail("Invalid number", pos);

		while (is_digit(pos))
			++pos;
	}

	return text.substr(start, pos - start);
}

std::string dotjson::parse_literal(const std::string& text, size_t& pos, const std::string& literal) noexcept(false)
{
	if (text.compare(pos, literal.size(), literal) != 0)
		dotjson::fail("Invalid literal", pos);

	pos += literal.size();

	return literal;
}

unsigned dotjson::parse_hex4(const std::string& text, size_t& pos) noexcept(false)
{
	if (pos + 4 > text.size())
		dotjson::fail("Invalid unicode escape", pos);

	unsigned value = 0;

	for (int i = 0; i < 4; ++i)
	{
		const char c = text[pos++];

		value <<= 4;

		if (c >= '0' && c <= '9')
			value |= static_cast<unsigned>(c - '0');
		else if (c >= 'a' && c <= 'f')
			value |= static_cast<unsigned>(c - 'a' + 10);
		else if (c >= 'A' && c <= 'F')
			value |= static_cast<unsigned>(c - 'A' + 10);
		else
			dotjson::fail("Invalid unicode escape", pos - 1);
	}

	return value;
}

void dotjson::append_utf8(std::string& str, unsigned code_point) noexcept(false)
{
	if (code_point < 0x80)
		str += static_cast<char>(code_point);
	else if (code_point < 0x800)
	{
		str += static_cast<char>(0xC0 | (code_point >> 6));
		str += static_cast<char>(0x80 | (code_point & 0x3F));
	}
	else if (code_point < 0x10000)
	{
		str += static_cast<char>(0xE0 | (code_point >> 12));
		str += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
		str += static_cast<char>(0x80 | (code_point & 0x3F));
	}
	else
	{
		str += static_cast<char>(0xF0 | (code_point >> 18));
		str += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
		str += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
		str += static_cast<char>(0x80 | (code_point & 0x3F));
	}
}

inline size_t dotjson::size(void) const noexcept(true)
{
	return this->_map.size();
}

inline std::string dotjson::operator[](const std::string& key) noexcept(true)
{
	return this->_map[key];
}
