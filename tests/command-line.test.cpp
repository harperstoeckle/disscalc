#include "disscalc/command-line.hpp"

#include <catch2/catch.hpp>

#include <ranges>

[[nodiscard]] static
bool approx_equals(std::ranges::range auto&& a, std::ranges::range auto&& b)
{
	return std::ranges::equal(
		std::forward<decltype(a)>(a),
		std::forward<decltype(b)>(b),
		[](double a, double b)
		{
			return a == Approx(b);
		}
	);
}

TEST_CASE("Parse command line options", "[command-line]")
{
	using DVec = std::vector<double>;

	std::vector<char const*> v0 = {};
	auto o0 = disscalc::parse_options(v0.size(), v0.data());

	REQUIRE(o0.show_help == false);
	REQUIRE(!o0.output_file_name.has_value());
	REQUIRE(!o0.format.has_value());
	REQUIRE(o0.start == Approx(1.0));
	REQUIRE(o0.delta == Approx(0.01));
	REQUIRE(o0.end == Approx(2.0));
	REQUIRE(o0.partials.empty());
	REQUIRE(o0.amplitudes.empty());
	REQUIRE(o0.errors.empty());

	std::vector<char const*> v1 = {
		"--help"
	};
	auto o1 = disscalc::parse_options(v1.size(), v1.data());
	REQUIRE(o1.show_help == true);

	std::vector<char const*> v2 = {
		"--help", "--value=true", "other", "random", "-options"
	};
	auto o2 = disscalc::parse_options(v2.size(), v2.data());
	REQUIRE(o2.show_help == true);
	REQUIRE(o2.errors.size() == 4);

	// Long options.
	std::vector<char const*> v3 = {
		"--output=file.txt", "--format=csv",
		"--start=3.5", "--delta=0.3", "--end=25",
		"-p", "1", "2", "3", "-a", "4"
	};
	auto o3 = disscalc::parse_options(v3.size(), v3.data());
	REQUIRE(o3.output_file_name == "file.txt");
	REQUIRE(o3.format == "csv");
	REQUIRE(o3.start == Approx(3.5));
	REQUIRE(o3.delta == Approx(0.3));
	REQUIRE(o3.end == Approx(25.0));
	REQUIRE(approx_equals(o3.partials, DVec{1, 2, 3}));
	REQUIRE(approx_equals(o3.amplitudes, DVec{4}));

	// Short options.
	std::vector<char const*> v4 = {
		"-o", "a file", "-f", "csv",
		"-s", "-9.2", "-d", "2", "-e", "-88",
		"-p", "-a", "-88000", "22.5", "3.1415"
	};
	auto o4 = disscalc::parse_options(v4.size(), v4.data());
	REQUIRE(o4.output_file_name == "a file");
	REQUIRE(o4.format == "csv");
	REQUIRE(o4.start == Approx(-9.2));
	REQUIRE(o4.delta == Approx(2));
	REQUIRE(o4.end == Approx(-88));
	REQUIRE(approx_equals(o4.partials, DVec{}));
	REQUIRE(approx_equals(o4.amplitudes, DVec{-88000, 22.5, 3.1415}));

	std::vector<char const*> v5 = {
		"one", "two", "three"
	};
	auto o5 = disscalc::parse_options(v5.size(), v5.data());
	REQUIRE(o5.errors.size() == 3);
}
