#ifndef DISSCALC_TABLE_HPP_INCLUDED
#define DISSCALC_TABLE_HPP_INCLUDED

#include <concepts>
#include <functional>
#include <iostream>

namespace disscalc
{
/// Print a single row in a double table.
inline
void print_table_entry(std::ostream& out, double left, double right, char delimiter)
{
	out << left << delimiter << right << '\n';
}

/** Print a two-column table of doubles as delimiter separated values.
 *
 * The "table" comes in the form of a range of doubles and a function object
 * mapping them to their corresponding values.
 *
 * The output consists of values in the range `[first, last]` in increments of
 * `delta` on the left side, and the corresponding `func(x)` on the right for
 * each `x` on the left.
 *
 * Along with the range, any values from `extra_values` are also used on the
 * left side in there respective places in order. No two entries will have the
 * same left-side value.
 *
 * @param out the stream to which the table will be outputted.
 *
 * @param first the inclusive lower bound of the left side of the table.
 *
 * @param delta the step between each value on the left side of the table.
 *
 * @param last the inclusive upper bound of the left side of the table.
 *
 * @param func the function applied to each left side value.
 *
 * @param delimiter the delimiter separating the left and right of the table.
 *
 * @param extra_values a set of values possibly outside of the main range which
 * are also on the left side.
 */
void print_table_as_dsv(
	std::ostream& out,
	double first,
	double delta,
	double last,
	std::invocable<double> auto func,
	char delimiter,
	std::set<double> const& extra_values
) requires std::convertible_to<
	std::invoke_result_t<decltype(func), double>,
	double
>
{
	auto extra_begin = std::cbegin(extra_values);
	auto const extra_end = std::cend(extra_values);

	for (; first <= last; first += delta)
	{
		// Print and skip any extra values not yet printed.
		for (; extra_begin != extra_end; ++extra_begin)
		{
			// Skip already covered values.
			if (*extra_begin == first)
			{
				++extra_begin;
				break;
			}
			if (*extra_begin >= first)
			{
				break;
			}
			print_table_entry(
				out,
				*extra_begin,
				std::invoke(func, *extra_begin),
				delimiter
			);
		}
		print_table_entry(out, first, std::invoke(func, first), delimiter);
	}

	for (; extra_begin != extra_end; ++extra_begin)
	{
		print_table_entry(
			out,
			*extra_begin,
			std::invoke(func, *extra_begin),
			delimiter
		);
	}
}
} // namespace disscalc

#endif
