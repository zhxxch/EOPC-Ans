#include <iostream>
#include <chrono>
#include <vector>
#include <cstdint>
#include "iacaMarks.h"
#include "speck-ymm.h"
#include "speck.h"
#include "stat-synth.h"
using hires_clock = std::chrono::high_resolution_clock;
int main(void) {
	const long long N = 1024 * 1024 * 10;
	const long long Size = N * sizeof(uint64_t);
	const double MB = Size / 1000000.0;
	const double MiB = Size / (1024.0 * 1024.0);
	const double Gb = (Size * 8) / 1000000000.0;
	std::array<uint64_t, 4> CtLo = {0}, CtHi = {0};
	std::array<uint64_t, 32> RoundKeys;
	speckw128key32(RoundKeys.data(),
		0x0706050403020100ull, 0x0f0e0d0c0b0a0908ull);
	CtLo[0] = 0x7469206564616d20ull;
	CtHi[0] = 0x6c61766975716520ull;
	__m256i c0 = _mm256_setzero_si256();
	for(int i = 0; i < 4; i++) {
		speckw128u128iter(c0, CtLo.data(), CtHi.data(),
			RoundKeys.data());
	}
	CtLo[0] = 0x7469206564616d20ull;
	CtHi[0] = 0x6c61766975716520ull;
	speck128u128(CtLo.data(), CtHi.data(),
		0x0706050403020100ull, 0x0f0e0d0c0b0a0908ull);
	auto old_std_flag = std::cout.flags();
	std::cout << std::hex << CtHi[0] << ";" << CtLo[0]
			  << old_std_flag << std::endl;
	std::vector<uint64_t> Vec(N);
	auto speck_rng = Speck128RNG(
		0x0f0e0d0c0b0a0908ull, 0x0706050403020100ull);
	hires_clock::time_point t0 = hires_clock::now();
#if 1
	for(unsigned long long i = 0; i < N; i += 2) {
		IACA_VC64_START
		speck128u128(&Vec[i], &Vec[i + 1], i, i);
	}
	IACA_VC64_END
#endif
#if 0
	for(long long i = 0; i < N; i++) {
		IACA_VC64_START
		Vec[i] = speck64u96(i, 0xa, 0xb, 0xc);
	}
	IACA_VC64_END
#endif
#if 0
	for(int i = 0; i < N; i += 2) {
		auto v = speck_rng();
		Vec[i] = v[0];
		Vec[i + 1] = v[1];
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