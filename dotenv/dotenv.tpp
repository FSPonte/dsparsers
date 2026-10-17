#pragma once

// ----- Dependencies -----
#include <fstream>

dotenv::dotenv(void) noexcept(true)
: _map(std::unordered_map<std::string, std::string>())
{}

dotenv::dotenv(const std::string& file_name) noexcept(true)
{
	this->load(file_name);
}

void dotenv::load(const std::string& file_name) noexcept(false)
{
	std::ifstream file(file_name);
	
	if (!file)
		throw "[dotenv] Failed to open the file";

	std::string line;
	size_t index;
	std::string key, value;

	while (std::getline(file, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		index = line.find('=');

		if (index == std::string::npos)
			continue;

		key = line.substr(0, index);
		value = this->expand(line.substr(index + 1));

		this->_map[std::move(key)] = std::move(value);
	}
}

std::string dotenv::expand(const std::string& value) const noexcept
{
	std::string result;
	std::size_t i = 0;

	while (i < value.size())
	{
		if (value[i] != '$')
		{
			result += value[i++];
			continue;
		}

		const std::size_t start = i + 1;
		std::size_t end;
		std::string name;

		if (start < value.size() && value[start] == '{')
		{
			// ${VAR}
			end = value.find('}', start + 1);
			if (end == std::string::npos)
			{
				result += value[i++]; // unterminated, keep '$' literally
				continue;
			}
			name = value.substr(start + 1, end - start - 1);
			++end; // skip '}'
		}
		else
		{
			// $VAR
			end = start;
			while (end < value.size() &&
			       (std::isalnum(static_cast<unsigned char>(value[end])) || value[end] == '_'))
				++end;
			name = value.substr(start, end - start);
		}

		if (name.empty())
		{
			result += value[i++]; // lone '$', keep it literally
			continue;
		}

		auto it = this->_map.find(name);
		if (it != this->_map.end())
			result += it->second;
		else if (const char* env = std::getenv(name.c_str()))
			result += env;

		i = end;
	}

	return result;
}

inline size_t dotenv::size(void) const noexcept(true)
{
	return this->_map.size();
}

inline std::string dotenv::operator[](const std::string& key) noexcept(true)
{
	return this->_map[key];
}
