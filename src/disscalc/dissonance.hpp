#ifndef DISSCALC_DISSONANCE_HPP_INCLUDED
#define DISSCALC_DISSONANCE_HPP_INCLUDED

#include <span>
#include <vector>

namespace disscalc
{
/// Data associated with a partial; that is, its frequency and amplitude.
struct Partial
{
	double frequency;
	double amplitude;
};

/// Compute dissonance of a frequency based on a list of partials.
[[nodiscard]]
double compute_dissonance(
	std::span<Partial const> partials, double interval
) noexcept;
} // namespace disscalc

#endif
