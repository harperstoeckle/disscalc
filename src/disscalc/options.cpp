#include "disscalc/options.hpp"

#include <algorithm>
#include <cassert>

namespace disscalc
{
[[nodiscard]]
char ProgramOptions::delimiter(void) const noexcept
{
	if (!format_.has_value() || format_ == "csv")
	{
		return ',';
	}
	if (format_ == "tsv")
	{
		return '\t';
	}
	return '?';
}

[[nodiscard]] static
auto create_partials(
	std::span<double const> frequencies,
	std::span<double const> amplitudes
) -> std::vector<Partial>
{
	assert(frequencies.size() == amplitudes.size());

	std::vector<Partial> partials;
	partials.reserve(frequencies.size());

	std::ranges::transform(
		frequencies,
		amplitudes,
		std::back_inserter(partials),
		[](double f, double a) noexcept
		{
			return Partial{f, a};
		}
	);
	return partials;
}

[[nodiscard]]
auto ProgramOptions::stable_partials(void) const -> std::vector<Partial>
{
	return create_partials(stable_frequencies_, stable_amplitudes_);
}

[[nodiscard]]
auto ProgramOptions::mobile_partials(void) const -> std::vector<Partial>
{
	if (mobile_frequencies_.empty())
	{
		/*
		 * TODO: Avoid doing this again when no mobile partials are
		 * specified.
		 */
		return stable_partials();
	}
	return create_partials(mobile_frequencies_, mobile_amplitudes_);
}

[[nodiscard]]
auto parse_double(std::string_view str) noexcept -> std::optional<double>
{
	char* end_;
	double const result = std::strtod(str.data(), &end_);

	// The entire string must match to be valid.
	if (end_ != str.data() + str.size())
	{
		return std::nullopt;
	}

	return result;
}

void ProgramOptions::add_error(std::string_view text, CommandLineErrorType type)
{
	errors_.emplace_back(text, type);
	valid_ = false;
}

void ProgramOptions::apply_option(ParsedOption const& parsed_option)
{
	std::string_view const flag = parsed_option.flag;

	if (parsed_option.invalid)
	{
		add_error(
			flag,
			CommandLineErrorType::unknown_arg
		);
	}
	else if (flag == "--help" || flag == "-h")
	{
		show_help_ = true;
	}
	else if (flag == "--output" || flag == "-o")
	{
		try_set_string_option(output_file_name_, parsed_option);
	}
	else if (flag == "--format" || flag == "-f")
	{
		try_set_string_option(format_, parsed_option);
	}
	else if (flag == "--start" || flag == "-s")
	{
		try_set_double_option(start_, parsed_option);
	}
	else if (flag == "--delta" || flag == "-d")
	{
		try_set_double_option(delta_, parsed_option);
	}
	else if (flag == "--end" || flag == "-e")
	{
		try_set_double_option(end_, parsed_option);
	}
	else if (flag == "-p")
	{
		stable_frequencies_.reserve(
			stable_frequencies_.size() + parsed_option.values.size()
		);
		try_insert_doubles(
			std::back_inserter(stable_frequencies_),
			parsed_option
		);
	}
	else if (flag == "-a")
	{
		stable_amplitudes_.reserve(
			stable_amplitudes_.size() + parsed_option.values.size()
		);
		try_insert_doubles(
			std::back_inserter(stable_amplitudes_),
			parsed_option
		);
	}
	else if (flag == "-P")
	{
		mobile_frequencies_.reserve(
			mobile_frequencies_.size() + parsed_option.values.size()
		);
		try_insert_doubles(
			std::back_inserter(mobile_frequencies_),
			parsed_option
		);
	}
	else if (flag == "-A")
	{
		mobile_amplitudes_.reserve(
			mobile_amplitudes_.size() + parsed_option.values.size()
		);
		try_insert_doubles(
			std::back_inserter(mobile_amplitudes_),
			parsed_option
		);
	}
	else if (flag == "-x")
	{
		try_insert_doubles(
			std::inserter(extra_values_, std::end(extra_values_)),
			parsed_option
		);
	}
	else
	{
		add_error(
			flag,
			CommandLineErrorType::unknown_arg
		);
	}
}

bool ProgramOptions::ensure_has_single_value(ParsedOption const& option)
{
	if (option.values.empty())
	{
		add_error(
			option.flag,
			CommandLineErrorType::no_value_provided
		);
		return false;
	}
	if (option.values.size() != 1)
	{
		add_error(
			option.flag,
			CommandLineErrorType::no_value_provided
		);
		return false;
	}

	return true;
}

void ProgramOptions::validate(void)
{
	if (
		format_.has_value()
		&& format_ != "csv"
		&& format_ != "tsv"
	)
	{
		add_error(
			"format must be csv or tsv",
			CommandLineErrorType::generic
		);
	}
	if (delta_ <= 0.0)
	{
		add_error(
			"delta",
			CommandLineErrorType::not_positive
		);
	}
	if (end_ <= 0.0)
	{
		add_error(
			"end interval",
			CommandLineErrorType::not_positive
		);
	}

	auto const not_positive = [](double x) noexcept { return x <= 0.0; };
	if (std::ranges::any_of(stable_frequencies_, not_positive))
	{
		add_error(
			"stable frequencies",
			CommandLineErrorType::list_not_positive
		);
	}
	if (std::ranges::any_of(stable_amplitudes_, not_positive))
	{
		add_error(
			"stable amplitudes",
			CommandLineErrorType::list_not_positive
		);
	}

	if (std::ranges::any_of(mobile_frequencies_, not_positive))
	{
		add_error(
			"mobile frequencies",
			CommandLineErrorType::list_not_positive
		);
	}
	if (std::ranges::any_of(mobile_amplitudes_, not_positive))
	{
		add_error(
			"mobile amplitudes",
			CommandLineErrorType::list_not_positive
		);
	}

	if (std::ranges::any_of(extra_values_, not_positive))
	{
		add_error(
			"extra intervals",
			CommandLineErrorType::list_not_positive
		);
	}

	if (stable_frequencies_.size() != stable_amplitudes_.size())
	{
		add_error(
			"stable frequencies and amplitudes must be the same size",
			CommandLineErrorType::generic
		);
	}

	if (mobile_frequencies_.size() != mobile_amplitudes_.size())
	{
		add_error(
			"mobile frequencies and amplitudes must be the same size",
			CommandLineErrorType::generic
		);
	}
}

ProgramOptions::ProgramOptions(int argc, char const* argv[])
{
	assert(argc >= 0);

	std::span<char const* const> args(
		argv + 1,
		static_cast<std::size_t>(argc == 0 ? 0 : argc - 1)
	);
	while (!args.empty())
	{
		apply_option(extract_next_option(args));
	}

	if (!valid_)
	{
		return;
	}

	validate();
}
} // namespace disscalc
