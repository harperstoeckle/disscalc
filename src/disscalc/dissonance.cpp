#include "disscalc/dissonance.hpp"

#include <cassert>
#include <cmath>

namespace disscalc
{
// Compute the dissonance between two partials.
[[nodiscard]] static constexpr
double compute_dissonance_between_partials(Partial a, Partial b) noexcept
{
	// Magic numbers not explained in the original program.
	constexpr double dstar = 0.24;
	constexpr double s1 = 0.0207;
	constexpr double s2 = 18.96;
	constexpr double c1 = 5.0;
	constexpr double c2 = -5.0;
	constexpr double a1 = -3.51;
	constexpr double a2 = -5.75;

	double const least_amp = std::min(a.amplitude, b.amplitude);
	double const least_freq = std::min(a.frequency, b.frequency);
	double const freq_diff = std::abs(b.frequency - a.frequency);

	double const s = dstar / (s1 * least_freq + s2);
	double const arg1 = a1 * s * freq_diff;
	double const arg2 = a2 * s * freq_diff;

	double const exp1 = arg1 < -88 ? 0 : std::exp(arg1);
	double const exp2 = arg2 < -88 ? 0 : std::exp(arg2);

	return least_amp * (c1 * exp1 + c2 * exp2);
}

[[nodiscard]]
double compute_dissonance(
	std::span<Partial const> partials, double interval
) noexcept
{
	double dissonance;
	for (auto base_partial : partials)
	{
		for (auto raised_partial : partials)
		{
			// Partial when raised by the interval.
			raised_partial.frequency *= interval;
			dissonance += compute_dissonance_between_partials(
				base_partial,
				raised_partial
			);
		}
	}

	return dissonance;
}
} // namespace disscalc
