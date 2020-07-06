#include <iostream>
#include <cmath>
#include <set>
#include <tuple>
#include "Eigen/Dense"
#include "stat-synth.h"

using Z2_t = Eigen::Vector2i;
auto Z2_less = [](const Z2_t &a, const Z2_t &b) {
	if(a(0) < b(0)) return true;
	if(a(1) < b(1)) return true;
	return false;
};
using polymer_t = std::set<Z2_t, decltype(Z2_less)>;
std::tuple<float, float> polymerization_iter(
	polymer_t Polymer, const Z2_t end,
	const float length, const float r) {
	Polymer.insert(end);
	const unsigned int uZ = rdrand32();
	const int ds = ((uZ & 1) ? 1 : -1);
	const int a = ((uZ & 2) ? 1 : 0);
	const int dx = ds * a;
	const int dy = ds * (a - 1);
	const Z2_t delta(dx, dy);
	const Z2_t new_end = end + delta;
	if(Polymer.count(new_end)
		|| new_end == Z2_t::Zero()) {
		return std::tuple<float, float>(length, r);
	}
	const float new_r = new_end.cast<float>().norm();
	return polymerization_iter(
		Polymer, new_end, length + 1, new_r);
}

int main(void) {
	std::ios::sync_with_stdio(false);
	const int N = 1000;
	float avg_v = 0;
	float len, r, v;
	for(int i = 0; i < N; i++) {
		std::tie(len, r) = polymerization_iter(
			polymer_t(Z2_less), Z2_t::Zero(), 0, 0);
		v = log(r) / log(len);
		if(v >= 0) { avg_v += v / N; }
	}
	std::cout << "L_N=" << len << "; R_N=" << r
			  << "; R=L^" << avg_v << std::endl;
	return 0;
}