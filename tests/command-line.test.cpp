#include "disscalc/options.hpp"

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

// Get view of frequencies in a list of partials.
[[nodiscard]] static
auto freqs_view(std::ranges::borrowed_range auto&& partials)
{
	return partials
		| std::views::transform(
			[](disscalc::Partial p) { return p.frequency; }
		);
}

// Get view of amplitudes in a list of partials.
[[nodiscard]] static
auto amps_view(std::ranges::borrowed_range auto&& partials)
{
	return partials
		| std::views::transform(
			[](disscalc::Partial p) { return p.amplitude; }
		);
}

TEST_CASE("Parse command line options", "[command-line]")
{
	using DVec = std::vector<double>;
	using DSet = std::set<double>;

	std::vector<char const*> v0 = {};
	disscalc::ProgramOptions o0(v0.size(), v0.data());

	REQUIRE(o0.is_valid());
	REQUIRE(o0.should_show_help() == false);
	REQUIRE(!o0.output_file_name().has_value());
	REQUIRE(o0.delimiter() == ',');
	REQUIRE(o0.start() == Approx(1.0));
	REQUIRE(o0.delta() == Approx(0.01));
	REQUIRE(o0.end() == Approx(2.0));
	REQUIRE(o0.partials().empty());
	REQUIRE(o0.errors().empty());

	std::vector<char const*> v1 = {
		"disscalc",
		"--help"
	};
	disscalc::ProgramOptions o1(v1.size(), v1.data());
	REQUIRE(o1.is_valid());
	REQUIRE(o1.should_show_help() == true);

	std::vector<char const*> v2 = {
		"disscalc",
		"--help", "--value=true", "other", "random", "-options"
	};
	disscalc::ProgramOptions o2(v2.size(), v2.data());
	REQUIRE(!o2.is_valid());
	REQUIRE(o2.should_show_help() == true);

	// Long options.
	std::vector<char const*> v3 = {
		"disscalc",
		"--output=file.txt", "--format=csv",
		"--start=3.5", "--delta=0.3", "--end=25",
		"-p", "1", "2", "3", "-a", "4"
	};
	disscalc::ProgramOptions o3(v3.size(), v3.data());
	REQUIRE(!o3.is_valid());
	REQUIRE(o3.output_file_name() == "file.txt");
	REQUIRE(o3.delimiter() == ',');
	REQUIRE(o3.start() == Approx(3.5));
	REQUIRE(o3.delta() == Approx(0.3));
	REQUIRE(o3.end() == Approx(25.0));

	// Short options.
	std::vector<char const*> v4 = {
		"disscalc",
		"-o", "a file", "-f", "tsv",
		"-s", "-9.2", "-d", "2", "-e", "-88",
		"-p", "-a", "-88000", "22.5", "3.1415"
	};
	disscalc::ProgramOptions o4(v4.size(), v4.data());
	REQUIRE(!o4.is_valid());
	REQUIRE(o4.output_file_name() == "a file");
	REQUIRE(o4.delimiter() == '\t');
	REQUIRE(o4.start() == Approx(-9.2));
	REQUIRE(o4.delta() == Approx(2));
	REQUIRE(o4.end() == Approx(-88));

	std::vector<char const*> v5 = {
		"disscalc",
		"one", "two", "three"
	};
	disscalc::ProgramOptions o5(v5.size(), v5.data());
	REQUIRE(!o5.is_valid());

	std::vector<char const*> v6 = {
		"disscalc",
		"-x", "3.2", "9.5", "5.7"
	};
	disscalc::ProgramOptions o6(v6.size(), v6.data());
	REQUIRE(approx_equals(o6.extra_values(), DSet{3.2, 9.5, 5.7}));

	std::vector<char const*> v7 = {};
	disscalc::ProgramOptions o7(v7.size(), v7.data());

	REQUIRE(o7.should_show_help() == false);
	REQUIRE(!o7.output_file_name().has_value());
	REQUIRE(o7.delimiter() == ',');
	REQUIRE(o7.start() == Approx(1.0));
	REQUIRE(o7.delta() == Approx(0.01));
	REQUIRE(o7.end() == Approx(2.0));
	REQUIRE(o7.partials().empty());
	REQUIRE(o7.errors().empty());

	std::vector<char const*> v8 = {
		"disscalc",
		"-p", "1.0", "3.5", "10078.5",
		"-a", "8.7", "5.2", "9999"
	};
	disscalc::ProgramOptions o8(v8.size(), v8.data());
	auto p8 = o8.partials();
	REQUIRE(approx_equals(freqs_view(p8), DVec{1, 3.5, 10078.5}));
	REQUIRE(approx_equals(amps_view(p8), DVec{8.7, 5.2, 9999}));
}
