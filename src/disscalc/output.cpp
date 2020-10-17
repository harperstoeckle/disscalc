#include "disscalc/output.hpp"

#include <generated/usage.hpp>

#include <iomanip>

namespace disscalc
{
// Get the start of an error line.
[[nodiscard]] static
std::string_view get_error_header(void)
{
	return "Disscalc Error: ";
}

void print_command_line_error(std::ostream& out, CommandLineError error)
{
	out << get_error_header();

	switch (error.type)
	{
	case CommandLineErrorType::unknown_arg:
		out << "unrecognized flag: " << std::quoted(error.argument_text);
		break;
		
	case CommandLineErrorType::invalid_number:
		out << "not a valid number: " << std::quoted(error.argument_text);
		break;

	case CommandLineErrorType::no_value_provided:
		out << "no argument provided for "
			<< std::quoted(error.argument_text);
		break;
	
	case CommandLineErrorType::too_many_values:
		out << "too many argument provided for "
			<< std::quoted(error.argument_text);
		break;

	case CommandLineErrorType::not_positive:
		out << error.argument_text << " must be greater than zero";
		break;

	case CommandLineErrorType::list_not_positive:
		out << error.argument_text << " must all be greater than zero";
		break;

	case CommandLineErrorType::generic:
		out << error.argument_text;
		break;

	default:
		out << "Unknown internal error";
	}

	out << '\n';
}

void print_generic_error(std::ostream& out, std::string_view error)
{
	out << get_error_header() << error << '\n';
}

void print_usage_message(std::ostream& out)
{
	out << usage_message;
}
}
