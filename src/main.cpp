#include "disscalc/command-line.hpp"
#include "disscalc/dissonance.hpp"
#include "disscalc/output.hpp"
#include "disscalc/table.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

// Return false if any options cannot be used and print relevant errors.
static
bool validate_options(disscalc::CommandLineOptions const& options)
{
	bool valid = true;

	if (
		options.format.has_value()
		&& options.format != "csv"
		&& options.format != "tsv"
	)
	{
		disscalc::print_generic_error(
			std::cerr,
			"Format must be csv or tsv"
		);
		valid = false;
	}
	if (options.start <= 0.0)
	{
		disscalc::print_generic_error(
			std::cerr,
			"Start must be greater than zero"
		);
		valid = false;
	}
	if (options.delta <= 0.0)
	{
		disscalc::print_generic_error(
			std::cerr,
			"Delta must be greater than zero"
		);
		valid = false;
	}
	if (options.end <= 0.0)
	{
		disscalc::print_generic_error(
			std::cerr,
			"End must be greater than zero"
		);
		valid = false;
	}

	auto const not_positive = [](double x) noexcept { return x <= 0.0; };
	if (std::ranges::any_of(options.partials, not_positive))
	{
		disscalc::print_generic_error(
			std::cerr,
			"Partials must be greater than zero"
		);
		valid = false;
	}
	if (std::ranges::any_of(options.amplitudes, not_positive))
	{
		disscalc::print_generic_error(
			std::cerr,
			"Amplitudes must be greater than zero"
		);
		valid = false;
	}
	if (std::ranges::any_of(options.extra_values, not_positive))
	{
		disscalc::print_generic_error(
			std::cerr,
			"Extra intervals must be greater than zero"
		);
		valid = false;
	}

	if (options.partials.size() != options.amplitudes.size())
	{
		disscalc::print_generic_error(
			std::cerr,
			"There must be the same number of partials and amplitudes"
		);
		valid = false;
	}

	return valid;
}

// Get the separator character based on the format name.
static
char get_separator_from_format(std::string_view format)
{
	assert(format == "csv" || format == "tsv");

	if (format == "csv")
	{
		return ',';
	}
	if (format == "tsv")
	{
		return '\t';
	}
	return '?';
}

// Output the data based on the options given. Return false on failure.
static
bool output_table(disscalc::CommandLineOptions const& options)
{
	auto const partials =
		disscalc::create_partials(options.partials, options.amplitudes);
	char const separator = options.format.has_value()
		? get_separator_from_format(*options.format)
		: ',';
	
	// Compute the dissonance of an interval given the current context.
	auto const compute_this_dissonance = [&partials](double d) noexcept
	{
		return disscalc::compute_dissonance(partials, d);
	};

	if (!options.output_file_name.has_value())
	{
		disscalc::print_table_as_dsv(
			std::cout,
			options.start,
			options.delta,
			options.end,
			compute_this_dissonance,
			separator,
			options.extra_values
		);
		return true;
	}

	// Must be converted to std::string to be compatible with std::ofstream.
	std::ofstream output_file(std::string(*options.output_file_name));
	if (!output_file)
	{
		disscalc::print_generic_error(
			std::cerr,
			"Could not open output file"
		);
		return false;
	}

	disscalc::print_table_as_dsv(
		output_file,
		options.start,
		options.delta,
		options.end,
		compute_this_dissonance,
		separator,
		options.extra_values
	);
	return true;
}

int main(int argc, char const* argv[])
{
	auto options = disscalc::parse_options(argc - 1, argv + 1);

	if (options.show_help)
	{
		disscalc::print_usage_message(std::cout);
		return 0;
	}

	if (!options.errors.empty())
	{
		for (auto error : options.errors)
		{
			disscalc::print_command_line_error(std::cerr, error);
		}
		return 1;
	}

	if (!validate_options(options))
	{
		return 2;
	}

	if (!output_table(options))
	{
		return 3;
	}
	
	return 0;
}
