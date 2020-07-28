#ifndef DISSCALC_ARGS_PARSING_HPP_INCLUDED
#define DISSCALC_ARGS_PARSING_HPP_INCLUDED

#include <span>
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

/** Extract the next command-line option.
 *
 * Produce the next option, consisting of a flag and one or more values. `args`
 * is shortened so that it no longer contains the arguments for that option.
 */
[[nodiscard]]
ParsedOption extract_next_option(std::span<char const* const>& args);
} // namespace disscalc

#endif
