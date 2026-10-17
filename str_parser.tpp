#pragma once

inline bool is_hspace(const char ch) noexcept(true)
{
	return (
		ch == ' ' ||
		ch == '\t' ||
		ch == '\r'
	);
}
