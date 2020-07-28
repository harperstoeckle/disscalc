#ifndef DISSCALC_OPTIONS_HPP_INCLUDED
#define DISSCALC_OPTIONS_HPP_INCLUDED

#include "disscalc/args-parsing.hpp"
#include "disscalc/dissonance.hpp"

#include <concepts>
#include <iterator>
#include <optional>
#include <set>
#include <string_view>
#include <vector>

namespace disscalc
{
enum struct CommandLineErrorType
{
	unknown_arg, ///< Argument not recognized in its context.
	invalid_number, ///< Incorrectly formatted number.
	no_value_provided, ///< No value provided for a flag expecting one.
	too_many_values, ///< Too many values provided to a flag.

	not_positive, ///< A single value is not positive and should be.
	list_not_positive, ///< Not all in a list of values are positive.

	generic, ///< Miscellaneous errors.
};

/// Error caused by invalid command line option.
struct CommandLineError
{
	/// Text of the argument causing the error.
	std::string_view argument_text;
	CommandLineErrorType type;
};

// Try to parse `str` as a double, returning an empty optional on failure.
[[nodiscard]]
auto parse_double(std::string_view str) noexcept -> std::optional<double>;

/// Contains values of program options.
class ProgramOptions
{
public:
	/// Parse command-line options to construct this.
	ProgramOptions(int argc, char const* argv[]);

	/** Indicate whether these options are valid.
	 *
	 * If not, no other member functions can safely be called except
	 * `should_show_help`.
	 */
	[[nodiscard]]
	bool is_valid(void) const noexcept
	{
		return valid_;
	}

	[[nodiscard]]
	auto errors(void) const noexcept -> std::vector<CommandLineError> const&
	{
		return errors_;
	}

	[[nodiscard]]
	auto output_file_name(void) const noexcept
		-> std::optional<std::string_view>
	{
		return output_file_name_;
	}

	/// Indicate whether help should be displayed.
	[[nodiscard]]
	bool should_show_help(void) const noexcept
	{
		return show_help_;
	}

	[[nodiscard]]
	double start(void) const noexcept
	{
		return start_;
	}
	
	[[nodiscard]]
	double delta(void) const noexcept
	{
		return delta_;
	}

	[[nodiscard]]
	double end(void) const noexcept
	{
		return end_;
	}

	/// Combine frequencies and amplitudes into partials.
	[[nodiscard]]
	auto partials(void) const noexcept -> std::vector<Partial>;

	[[nodiscard]]
	auto extra_values(void) const noexcept -> std::set<double> const&
	{
		return extra_values_;
	}

	/// Get delimiter for DSV table output.
	[[nodiscard]]
	char delimiter(void) const noexcept;

private:
	/*
	 * Try to apply the given option, adding any errors and setting
	 * `valid_` to false on failure.
	 */
	void apply_option(ParsedOption const& parsed_option);

	/*
	 * Ensure there is a single value for the given option, adding errors
	 * and returning false if not.
	 */
	bool ensure_has_single_value(ParsedOption const& option);

	/*
	 * Try to set `to_set` to the value of a string option with a single
	 * value, reporting relevant errors.
	 */
	void try_set_string_option(
		std::assignable_from<std::string_view> auto&& to_set,
		ParsedOption const& option
	)
	{
		if (!ensure_has_single_value(option))
		{
			return;
		}

		to_set = option.values.front();
	}

	/*
	 * Try to set `to_set` to the value of a double option with a single
	 * value, reporting errors if there is not exactly one value or if it
	 * is not a valid double.
	 */
	void try_set_double_option(
		std::assignable_from<double> auto&& to_set,
		ParsedOption const& option
	)
	{
		if (!ensure_has_single_value(option))
		{
			return;
		}

		std::string_view const arg = option.values.front();
		if (auto value = parse_double(arg))
		{
			to_set = *value;
			return;
		}

		errors_.emplace_back(
			arg,
			CommandLineErrorType::invalid_number
		);
	}

	/*
	 * For each value in `option`, try to parse it as a double and insert
	 * it using the output iterator `dest`, reporting relevant errors.
	 */
	void try_insert_doubles(
		std::output_iterator<double> auto dest,
		ParsedOption const& option
	)
	{
		for (auto arg : option.values)
		{
			if (auto value = parse_double(arg))
			{
				*dest++ = *value;
				continue;
			}

			errors_.emplace_back(
				arg,
				CommandLineErrorType::invalid_number
			);
		}
	}

	/*
	 * Once they are parsed, validate the options and add necessary errors.
	 * On failure, set `valid_` to false.
	 */
	void validate(void);

	bool valid_ = true;

	bool show_help_ = false;

	std::optional<std::string_view> output_file_name_;
	std::optional<std::string_view> format_;

	double start_ = 1.0;
	double delta_ = 0.01;
	double end_ = 2.0;

	std::vector<double> frequencies_;
	std::vector<double> amplitudes_;

	std::set<double> extra_values_;
	
	std::vector<CommandLineError> errors_;
};
} // namespace disscalc

#endif
