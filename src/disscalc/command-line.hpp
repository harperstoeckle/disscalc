#ifndef DISSCALC_COMMAND_LINE_HPP_INCLUDED
#define DISSCALC_COMMAND_LINE_HPP_INCLUDED

#include <optional>
#include <set>
#include <string_view>
#include <vector>

/*
 * Note: In this case, the following words are used:
 * - "argument" refers to the individual command-line arguments regardless of
 *   their actual purposes. They are just text that has been passed into the
 *   program and has yet to be parsed.
 * - "flag" refers to the arguments that usually start with one or two dashes.
 *   For example, "-a" and "--help" are flags.
 * - "value" refers to a value associated with a flag. For example, in "-k
 *   value" or "--key=value", "value" is the value in both cases.
 * - "option" refers to the whole construct of a flag and multiple values.
 *
 * In this program, flags may have multiple values following them until the
 * next flag. There are no arguments not associated with a flag. For example,
 * "disscalc value" is invalid because "value" has no flag attached to it.
 *
 * Additionally, flags that take multiple options can only be short,
 * single-letter flags. If a flag is specified in the long form, such as
 * "--key=value" or "--key", it may only have zero or one values.
 */

namespace disscalc
{
enum struct CommandLineErrorType
{
	unknown_arg, ///< Argument not recognized in its context.
	invalid_number, ///< Incorrectly formatted number.
	no_value_provided, ///< No value provided for a flag expecting one.
	too_many_values ///< Too many values provided to a flag.
};

/// Error caused by invalid command line option.
struct CommandLineError
{
	/// Text of the argument causing the error.
	std::string_view argument_text;
	CommandLineErrorType type;
};

/// Contains values of program options.
struct CommandLineOptions
{
	bool show_help = false;

	std::optional<std::string_view> output_file_name;
	std::optional<std::string_view> format;

	double start = 1.0;
	double delta = 0.01;
	double end = 2.0;

	std::vector<double> partials;
	std::vector<double> amplitudes;

	std::set<double> extra_values;
	
	std::vector<CommandLineError> errors;
};

/// Parse command-line options.
[[nodiscard]]
CommandLineOptions parse_options(int argc, char const* argv[]);
} // namespace disscalc

#endif
