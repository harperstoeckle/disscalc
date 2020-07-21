#ifndef DISSCALC_TABLE_HPP_INCLUDED
#define DISSCALC_TABLE_HPP_INCLUDED

#include <concepts>
#include <functional>
#include <iostream>

namespace disscalc
{
/** Print a two-column table of doubles as delimiter separated values.
 *
 * The "table" comes in the form of a range of doubles and a function object
 * mapping them to their corresponding values.
 *
 * The output consists of values in the range `[first, last]` in increments of
 * `delta` on the left side, and the corresponding `func(x)` on the right for
 * each `x` on the left.
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
 */
void print_table_as_dsv(
	std::ostream& out,
	double first,
	double delta,
	double last,
	std::invocable<double> auto func,
	char delimiter
) requires std::convertible_to<
	std::invoke_result_t<decltype(func), double>,
	double
>
{
	for (; first <= last; first += delta)
	{
		out << first << delimiter <<
			static_cast<double>(std::invoke(func, first))
			<< '\n';
	}
}
} // namespace disscalc

#endif
