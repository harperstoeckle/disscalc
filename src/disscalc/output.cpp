#include "disscalc/output.hpp"

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
	constexpr std::string_view usage_message =
		"Usage: disscalc [--help] [--output=<file>] [--format=<format>]\n"
		"                [--start=<number>] [--delta=<number>] [--quantity=<number>]\n"
		"                [--end=<number>] [-x <number>...] -p <number>... -a <number>...\n"
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
		"  -x <number>...                     Along with the normal range of numbers,\n"
		"                                     also compute dissonances for the intervals\n"
		"                                     specified in this option.\n"
		"  -p <number>...                     Specify the frequencies of the stationary\n"
		"                                     partials. If this option is given multiple\n"
		"                                     times, the partials are concatenated in\n"
		"                                     the order they were given. Each must be a\n"
		"                                     positive number.\n"
		"  -a <number>...                     Specify the amplitudes of the stationary\n"
		"                                     partials. If this option is given multiple\n"
		"                                     times, the amplitudes are concatenated in\n"
		"                                     the order they were given. Each must be a\n"
		"                                     positive number. The total number of\n"
		"                                     amplitudes must match the number of\n"
		"                                     partials, and each nth amplitude will be\n"
		"                                     matched with the nth partial.\n"
		"  -P <number>...                     Specify the frequencies of the mobile\n"
		"                                     partials. If not provided, use the\n"
		"                                     stationary frequencies by default.\n"
		"  -A <number>...                     Specify the frequencies of the mobile\n"
		"                                     amplitudes. If not provided, use the\n"
		"                                     stationary amplitudes by default. The\n"
		"                                     number of mobile amplitudes must be equal\n"
		"                                     to the number of mobile frequencies.\n";
	out << usage_message;
}
}
