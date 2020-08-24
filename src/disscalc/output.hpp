#ifndef DISSCALC_OUTPUT_HPP_INCLUDED
#define DISSCALC_OUTPUT_HPP_INCLUDED

#include "disscalc/options.hpp"
#include "disscalc/dissonance.hpp"

#include <iostream>
#include <span>
#include <string_view>

namespace disscalc
{
/// Print a command-line error.
void print_command_line_error(std::ostream& out, CommandLineError error);

/// Print a generic error.
void print_generic_error(std::ostream& out, std::string_view error);

/// Print the usage message to `out`.
void print_usage_message(std::ostream& out);
} // namespace disscalc

#endif
