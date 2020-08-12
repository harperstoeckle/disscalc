#include "disscalc/dissonance.hpp"
#include "disscalc/options.hpp"
#include "disscalc/output.hpp"
#include "disscalc/table.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

// Output the data based on the options given. Return false on failure.
static
bool output_table(disscalc::ProgramOptions const& options)
{
	auto const stable_partials = options.stable_partials();
	auto const mobile_partials = options.mobile_partials();
	char const separator = options.delimiter();

	auto const compute_this_dissonance = [&](double d) noexcept
	{
		return disscalc::compute_dissonance(
			stable_partials,
			mobile_partials,
			d
		);
	};

	if (!options.output_file_name().has_value())
	{
		disscalc::print_table_as_dsv(
			std::cout,
			options.start(),
			options.delta(),
			options.end(),
			compute_this_dissonance,
			separator,
			options.extra_values()
		);
		return true;
	}

	// Must be converted to std::string to be compatible with std::ofstream.
	std::ofstream output_file(std::string(*options.output_file_name()));
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
		options.start(),
		options.delta(),
		options.end(),
		compute_this_dissonance,
		separator,
		options.extra_values()
	);
	return true;
}

int main(int argc, char const* argv[])
{
	disscalc::ProgramOptions options(argc, argv);

	if (options.should_show_help())
	{
		disscalc::print_usage_message(std::cout);
		return 0;
	}
	if (!options.is_valid())
	{
		for (auto error : options.errors())
		{
			disscalc::print_command_line_error(std::cerr, error);
		}
		return 1;
	}

	if (!output_table(options))
	{
		return 2;
	}

	return 0;
}
