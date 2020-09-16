#include "disscalc/args-parsing.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>

namespace disscalc
{
[[nodiscard]] static constexpr
bool is_long_flag(std::string_view arg) noexcept
{
	return arg.starts_with("--");
}

[[nodiscard]] static constexpr
bool is_short_flag(std::string_view arg) noexcept
{
	return arg.starts_with("-")
		&& arg.size() == 2
		&& arg[1] != '-';
}

[[nodiscard]] static constexpr
bool is_flag(std::string_view arg) noexcept
{
	return is_long_flag(arg) || is_short_flag(arg);
}

/*
 * Parse a long-form option such as "--key" or "--key=value". It is expected
 * that the provided string start with "--".
 */
[[nodiscard]] static
ParsedOption parse_long_option(std::string_view arg)
{
	assert(is_long_flag(arg));
	
	std::size_t equals_pos = arg.find('=');
	if (equals_pos == std::string_view::npos)
	{
		return ParsedOption{
			arg,
			{}
		};
	}
	return ParsedOption{
		arg.substr(0, equals_pos),
		{arg.substr(equals_pos + 1)}
	};
}

// Extract values until the end of arguments or the next flag.
[[nodiscard]] static
auto extract_values(std::span<char const* const>& args)
	-> std::vector<std::string_view>
{
	auto const value_args = std::span<char const* const>(
		std::cbegin(args),
		std::ranges::find_if(args, is_flag)
	);

	std::vector<std::string_view> values(
		std::cbegin(value_args),
		std::cend(value_args)
	);

	args = args.subspan(value_args.size());

	return values;
}

// Extract the next option from the input.
[[nodiscard]]
ParsedOption extract_next_option(std::span<char const* const>& args)
{
	std::string_view cur_arg = args.front();
	args = args.subspan(1);

	if (is_long_flag(cur_arg))
	{
		return parse_long_option(cur_arg);
	}
	if (is_short_flag(cur_arg))
	{
		return ParsedOption{cur_arg, extract_values(args)};
	}
	
	return ParsedOption{cur_arg, {}, true};
}
}
