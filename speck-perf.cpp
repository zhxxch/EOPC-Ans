#include <iostream>
#include <chrono>
#include <vector>
#include <cstdint>
#include "speck-ymm.h"
#include "stat-synth.h"
using hires_clock = std::chrono::high_resolution_clock;
int main(void) {
	const int N = 1024 * 1024 * 20;
	const int Size = N * sizeof(uint64_t);
	const double MB = Size / 1000000.0;
	const double MiB = Size / (1024.0 * 1024.0);
	const double Gb = (Size * 8) / 1000000000.0;
	std::array<uint64_t, 4> CtLo, CtHi;
	std::array<uint64_t, 32> RoundKeys;
	speckw128key32(RoundKeys.data(), 123432, 9087897);
	std::vector<uint64_t> Vec(N);
	auto speck_rng = Speck128RNG(123432, 9087897);
	hires_clock::time_point t0 = hires_clock::now();
#if 1
	for(int i = 0; i < N; i += 2) {
		auto v = speck_rng();
		Vec[i] = v[0];
		Vec[i+1] = v[1];
	}
#endif
#if 0
	for(int i = 0; i < N; i += 2) {
		CtHi[0] = PtLo++;
		speckw128u128(CtLo.data(), CtHi.data(),
			RoundKeys.data());
		Vec[i] = CtLo[0];
		Vec[i + 1] = CtHi[1];
	}
#endif
#if 0
for(int i = 0; i < N; i++) {
		Vec[i] = rdrand64();
	}
#endif
	hires_clock::time_point t1 = hires_clock::now();
	std::chrono::duration<double> dt
		= std::chrono::duration_cast<
			std::chrono::duration<double>>(t1 - t0);
	std::cout << (MiB / dt.count()) << "MiB/s; "
			  << (MB / dt.count()) << "MB/s; "
			  << (Gb / dt.count()) << "Gb/s"
			  << std::endl;
	return 0;
}