#include "disscalc/output.hpp"

#include <iomanip>

namespace disscalc
{
void print_command_line_error(std::ostream& out, CommandLineError error)
{
	out << get_error_header();

	switch (error.type)
	{
	case CommandLineErrorType::unknown_arg:
		out << "Unrecognized flag: " << std::quoted(error.argument_text);
		break;
		
	case CommandLineErrorType::invalid_number:
		out << "Not a valid number: " << std::quoted(error.argument_text);
		break;

	case CommandLineErrorType::no_value_provided:
		out << "No argument provided for "
			<< std::quoted(error.argument_text);
		break;
	
	case CommandLineErrorType::too_many_values:
		out << "Too many argument provided for "
			<< std::quoted(error.argument_text);
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
	constexpr std::string_view usage_message =
		"Usage: disscalc [--help] [--output=<file>] [--format=<format>]\n"
		"                [--start=<number>] [--delta=<number>] [--quantity=<number>]\n"
		"                [--end=<number>] -p <number>... -a <number>...\n"
		"\n"
		"Generate a dissonance curve for the given timbre.\n"
		"\n"
		"Options:\n"
		"  -h, --help                         Display this usage and exit.\n"
		"  -o <file>, --output=<file>         Send output to the specified file.\n"
		"  -f <format>, --format=<format>     Use the specified output format. The\n"
		"                                     available formats are csv and tsv.\n"
		"  -s <number>, --start=<number>      Use the given positive number as the\n"
		"                                     (inclusive) lower bound of intervals\n"
		"                                     tested. The default is 1.0.\n"
		"  -d <number>, --delta=<number>      Use the given positive number as the\n"
		"                                     difference between each interval tested.\n"
		"                                     The default is 0.01.\n"
		"  -e <number>, --end=<number>        Use the given positive number as the\n"
		"                                     (inclusive) upper bound of intervals\n"
		"                                     tested. The default is 2.0.\n"
		"  -p <number>...                     Specify the frequencies of the partials.\n"
		"                                     If this option is given multiple times,\n"
		"                                     the partials are concatenated in the order\n"
		"                                     they were given. Each must be a positive\n"
		"                                     number.\n"
		"  -a <number>...                     Specify the amplitudes of the partials. If\n"
		"                                     this option is given multiple times, the\n"
		"                                     amplitudes are concatenated in the order\n"
		"                                     they were given. Each must be a positive\n"
		"                                     number. The total number of amplitudes\n"
		"                                     must match the number of partials, and\n"
		"                                     each nth amplitude will be matched with\n"
		"                                     the nth partial.\n";
	out << usage_message;
}

[[nodiscard]]
std::string_view get_error_header(void)
{
	return "Disscalc Error: ";
}
}
