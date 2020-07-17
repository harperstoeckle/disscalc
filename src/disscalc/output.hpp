#ifndef DISSCALC_OUTPUT_HPP_INCLUDED
#define DISSCALC_OUTPUT_HPP_INCLUDED

#include "disscalc/command-line.hpp"
#include "disscalc/dissonance.hpp"

#include <iostream>
#include <span>
#include <string_view>

namespace disscalc
{
/// Print a list of command-line parsing errors to `out`.
void print_command_line_errors(
	std::ostream& out, std::span<CommandLineError const> errors
);

/// Print a generic error.
void print_error(std::ostream& out, std::string_view error);

/** Print the dissonance table.
 *
 * Print to `out`. The interval and dissonance are separated by `separator`.
 * `partials` are the partials, and `start`, `delta`, and `end` represent the
 * lower bound, distance between tested intervals, and upper bound
 * respectively.
 */
void print_dissonance_table(
	std::ostream& out,
	char separator,
	std::span<Partial const> partials,
	double start,
	double delta,
	double end
);

/// Get the usage message.
[[nodiscard]]
std::string_view get_usage_message(void);

/// Get the start of an error line
[[nodiscard]]
std::string_view get_error_header(void);
} // namespace disscalc

#endif
