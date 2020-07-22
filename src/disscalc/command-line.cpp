#include "disscalc/command-line.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <span>

namespace disscalc
{
namespace
{
// Contents of a single extracted flag and its values.
struct ParsedOption
{
	// Name of the flag. For example, in "-k value", "-k" is the flag.
	std::string_view flag;
	
	/*
	 * Sequence of values listed after the flag. For example, in "-a 1 2",
	 * "1" and "2" are the values.
	 */
	std::vector<std::string_view> values;

	// Set to true when `flag` is not formatted as a flag properly.
	bool invalid = false;
};

using ArgList = std::span<char const* const>;
}

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
auto extract_values(ArgList& args)
	-> std::vector<std::string_view>
{
	auto const value_args = ArgList(
		std::cbegin(args),
		std::ranges::find_if(args, is_flag)
	);

	std::vector<std::string_view> values;
	values.reserve(value_args.size());
	std::ranges::copy(value_args, std::back_inserter(values));

	args = args.subspan(value_args.size());

	return values;
}

// Extract the next option from the input.
[[nodiscard]] static
ParsedOption extract_next_option(ArgList& args)
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

[[nodiscard]] static
auto parse_double(std::string_view str) noexcept
	-> std::optional<double>
{
	char* end;
	double const result = std::strtod(str.data(), &end);

	// The entire string must match to be valid.
	if (end != str.data() + str.size())
	{
		return std::nullopt;
	}

	return result;
}

/*
 * Apply a parsed option to an existing `CommandLineOptions`. If any options
 * are invalid, add errors instead.
 */
static
void apply_option(
	ParsedOption const& parsed_option,
	CommandLineOptions& options
)
{
	std::string_view const flag = parsed_option.flag;

	/*
	 * Ensure there is only one value, adding errors and returning false if
	 * not.
	 */
	auto ensure_single_value = [&](void) mutable
	{
		if (parsed_option.values.empty())
		{
			options.errors.emplace_back(
				flag,
				CommandLineErrorType::no_value_provided
			);
			return false;
		}
		if (parsed_option.values.size() != 1)
		{
			options.errors.emplace_back(
				flag,
				CommandLineErrorType::too_many_values
			);
			return false;
		}
		return true;
	};

	/*
	 * Try to set `option` to the current option only if there is exactly
	 * one value. Otherwise, report the errors and leave `option` as-is.
	 */
	auto try_set_string_option = [&](auto& option) mutable
	{
		if (!ensure_single_value())
		{
			return;
		}

		option = parsed_option.values.front();
	};

	/*
	 * Try to set `option` to the current option only if there is exactly
	 * one value. If there is not exactly one value or if the value is not
	 * a valid double, report the errors and leave `option` as-is.
	 */
	auto try_set_double_option = [&](auto& option) mutable
	{
		if (!ensure_single_value())
		{
			return;
		}

		std::string_view const arg = parsed_option.values.front();
		if (auto value = parse_double(arg))
		{
			option = *value;
			return;
		}

		options.errors.emplace_back(
			arg,
			CommandLineErrorType::invalid_number
		);
	};

	/*
	 * Try to parse each value as a double and append it to `option`. Add
	 * errors for any values that are invalid.
	 */
	auto try_append_doubles = [&](auto& option) mutable
	{
		option.reserve(option.size() + parsed_option.values.size());

		for (auto arg : parsed_option.values)
		{
			if (auto value = parse_double(arg))
			{
				option.push_back(*value);
				continue;
			}
			
			options.errors.emplace_back(
				arg,
				CommandLineErrorType::invalid_number
			);
		}
	};

	/*
	 * Try to parse each value as a double and insert it into `option`. Add
	 * errors for any values that are invalid.
	 */
	auto try_insert_doubles = [&](auto& option) mutable
	{
		for (auto arg : parsed_option.values)
		{
			if (auto value = parse_double(arg))
			{
				option.insert(*value);
				continue;
			}

			options.errors.emplace_back(
				arg,
				CommandLineErrorType::invalid_number
			);
		}
	};

	if (parsed_option.invalid)
	{
		options.errors.emplace_back(
			flag,
			CommandLineErrorType::unknown_arg
		);
	}
	else if (flag == "--help" || flag == "-h")
	{
		options.show_help = true;
	}
	else if (flag == "--output" || flag == "-o")
	{
		try_set_string_option(options.output_file_name);
	}
	else if (flag == "--format" || flag == "-f")
	{
		try_set_string_option(options.format);
	}
	else if (flag == "--start" || flag == "-s")
	{
		try_set_double_option(options.start);
	}
	else if (flag == "--delta" || flag == "-d")
	{
		try_set_double_option(options.delta);
	}
	else if (flag == "--end" || flag == "-e")
	{
		try_set_double_option(options.end);
	}
	else if (flag == "-p")
	{
		try_append_doubles(options.partials);
	}
	else if (flag == "-a")
	{
		try_append_doubles(options.amplitudes);
	}
	else if (flag == "-x")
	{
		try_insert_doubles(options.extra_values);
	}
	else
	{
		options.errors.emplace_back(
			flag,
			CommandLineErrorType::unknown_arg
		);
	}
}

[[nodiscard]]
CommandLineOptions parse_options(int argc, char const* argv[])
{
	CommandLineOptions options;

	ArgList args(argv, static_cast<std::size_t>(argc));
	while (!args.empty())
	{
		apply_option(extract_next_option(args), options);
	}

	return options;
}
}
