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

	for (
		auto f_begin = std::cbegin(frequencies),
			f_end = std::cend(frequencies),
			a_begin = std::cbegin(amplitudes),
			a_end = std::cend(amplitudes);
		(f_begin != f_end) && (a_begin != a_end);
		++ f_begin, ++a_begin
	)
	{
		partials.emplace_back(*f_begin, *a_begin);
	}

	return partials;
}

[[nodiscard]]
auto ProgramOptions::partials(void) const noexcept -> std::vector<Partial>
{
	return create_partials(frequencies_, amplitudes_);
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

void ProgramOptions::apply_option(ParsedOption const& parsed_option)
{
	std::string_view const flag = parsed_option.flag;

	if (parsed_option.invalid)
	{
		errors_.emplace_back(
			flag,
			CommandLineErrorType::unknown_arg
		);
		valid_ = false;
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
		frequencies_.reserve(frequencies_.size() + parsed_option.values.size());
		try_insert_doubles(std::back_inserter(frequencies_), parsed_option);
	}
	else if (flag == "-a")
	{
		amplitudes_.reserve(amplitudes_.size() + parsed_option.values.size());
		try_insert_doubles(std::back_inserter(amplitudes_), parsed_option);
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
		errors_.emplace_back(
			flag,
			CommandLineErrorType::unknown_arg
		);
		valid_ = false;
	}
}

bool ProgramOptions::ensure_has_single_value(ParsedOption const& option)
{
	if (option.values.empty())
	{
		errors_.emplace_back(
			option.flag,
			CommandLineErrorType::no_value_provided
		);
		return false;
	}
	if (option.values.size() != 1)
	{
		errors_.emplace_back(
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
		errors_.emplace_back(
			"format must be csv or tsv",
			CommandLineErrorType::generic
		);
		valid_ = false;
	}
	if (delta_ <= 0.0)
	{
		errors_.emplace_back(
			"delta",
			CommandLineErrorType::not_positive
		);
		valid_ = false;
	}
	if (end_ <= 0.0)
	{
		errors_.emplace_back(
			"end interval",
			CommandLineErrorType::not_positive
		);
		valid_ = false;
	}

	auto const not_positive = [](double x) noexcept { return x <= 0.0; };
	if (std::ranges::any_of(frequencies_, not_positive))
	{
		errors_.emplace_back(
			"frequencies",
			CommandLineErrorType::list_not_positive
		);
		valid_ = false;
	}
	if (std::ranges::any_of(amplitudes_, not_positive))
	{
		errors_.emplace_back(
			"amplitudes",
			CommandLineErrorType::list_not_positive
		);
		valid_ = false;
	}
	if (std::ranges::any_of(extra_values_, not_positive))
	{
		errors_.emplace_back(
			"extra intervals",
			CommandLineErrorType::list_not_positive
		);
		valid_ = false;
	}

	if (frequencies_.size() != amplitudes_.size())
	{
		errors_.emplace_back(
			"frequencies and amplitudes must be the same size",
			CommandLineErrorType::generic
		);
		valid_ = false;
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
