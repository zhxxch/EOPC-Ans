#pragma once
/*  __ |  \   /   __|
 *    /     /    |
 *  ___|  _/ _\ \___|  */
#include <immintrin.h>
#include <stdint.h>
#ifndef _speck128u128_round
#define _speck128u128_round 32
#endif
inline void speckw128u128iter(__m256i counter,
	uint64_t PtLo[4], uint64_t PtHi[4],
	const uint64_t Keys[32]) {
	__m256i CtLo
		= _mm256_loadu_si256((const __m256i *)PtLo);
	__m256i CtHi
		= _mm256_loadu_si256((const __m256i *)PtHi);
	CtLo = _mm256_xor_si256(counter, CtLo);
	for(int i = 0; i < _speck128u128_round; i += 4) {
		CtHi = _mm256_or_si256(
			_mm256_srli_epi64(CtHi, 8),
			_mm256_slli_epi64(CtHi, 56));
		CtHi = _mm256_add_epi64(CtHi, CtLo);
		CtHi = _mm256_xor_si256(CtHi,
			_mm256_loadu_si256(
				(const __m256i *)(Keys + i)));
		CtLo = _mm256_or_si256(
			_mm256_srli_epi64(CtLo, 61),
			_mm256_slli_epi64(CtLo, 3));
		CtLo = _mm256_xor_si256(CtLo, CtHi);
	}
	CtLo = _mm256_permute4x64_epi64(CtLo, 0x93);
	CtHi = _mm256_permute4x64_epi64(CtHi, 0x93);
	_mm256_storeu_si256((__m256i *)PtLo, CtLo);
	_mm256_storeu_si256((__m256i *)PtHi, CtHi);
}
inline void speck4x128u128(uint64_t Ct[8],
	const uint64_t Pt[8], const uint64_t Key[32]) {
	__m256i CtSr
		= _mm256_loadu_si256((const __m256i *)Pt);
	__m256i CtSl = _mm256_loadu_si256(
		(const __m256i *)(Pt + 4));
	__m256i CtHi = _mm256_unpackhi_epi64(CtSr, CtSl);
	__m256i CtLo = _mm256_unpacklo_epi64(CtSr, CtSl);
	for(int i = 0; i < _speck128u128_round; i++) {
		CtSr = _mm256_srli_epi64(CtHi, 8);
		CtSl = _mm256_slli_epi64(CtHi, 56);
		CtHi = _mm256_xor_si256(CtSr, CtSl);
		CtHi = _mm256_add_epi64(CtHi, CtLo);
		CtHi = _mm256_xor_si256(
			CtHi, _mm256_set1_epi64x(Key[i]));
		CtSr = _mm256_srli_epi64(CtLo, 61);
		CtSl = _mm256_slli_epi64(CtLo, 3);
		CtLo = _mm256_xor_si256(CtSr, CtSl);
		CtLo = _mm256_xor_si256(CtLo, CtHi);
	}
	CtSr = _mm256_unpacklo_epi64(CtLo, CtHi);
	CtSl = _mm256_unpackhi_epi64(CtLo, CtHi);
	_mm256_storeu_si256((__m256i *)Ct, CtSr);
	_mm256_storeu_si256((__m256i *)(Ct + 4), CtSl);
}
inline void speckinv4x128u128(uint64_t Pt[8],
	const uint64_t Ct[8], const uint64_t Key[32]) {
	__m256i CtSr
		= _mm256_loadu_si256((const __m256i *)Ct);
	__m256i CtSl = _mm256_loadu_si256(
		(const __m256i *)(Ct + 4));
	__m256i CtHi = _mm256_unpackhi_epi64(CtSr, CtSl);
	__m256i CtLo = _mm256_unpacklo_epi64(CtSr, CtSl);
	for(int i = _speck128u128_round; i--;) {
		CtLo = _mm256_xor_si256(CtLo, CtHi);
		CtSr = _mm256_srli_epi64(CtLo, 3);
		CtSl = _mm256_slli_epi64(CtLo, 61);
		CtLo = _mm256_xor_si256(CtSr, CtSl);
		CtHi = _mm256_xor_si256(
			CtHi, _mm256_set1_epi64x(Key[i]));
		CtHi = _mm256_sub_epi64(CtHi, CtLo);
		CtSr = _mm256_srli_epi64(CtHi, 56);
		CtSl = _mm256_slli_epi64(CtHi, 8);
		CtHi = _mm256_xor_si256(CtSr, CtSl);
	}
	CtSr = _mm256_unpacklo_epi64(CtLo, CtHi);
	CtSl = _mm256_unpackhi_epi64(CtLo, CtHi);
	_mm256_storeu_si256((__m256i *)Pt, CtSr);
	_mm256_storeu_si256((__m256i *)(Pt + 4), CtSl);
}
inline void speck128key32(uint64_t RoundKeys[32],
	uint64_t KeyLo, uint64_t KeyHi) {
	for(uint64_t i = 0; i < 32; i++) {
		RoundKeys[i] = KeyLo;
		KeyHi = (KeyHi >> 8) | (KeyHi << 56);
		KeyHi = (KeyHi + KeyLo) ^ i;
		KeyLo = (KeyLo << 3) | (KeyLo >> 61);
		KeyLo = KeyLo ^ KeyHi;
	}
}
inline void speck128key32T(uint64_t keys_out[32],
	const uint64_t keys_in[32]) {
	uint64_t t[32];
	for(int i = 0; i < 32; i++) { t[i] = keys_in[i]; }
	for(int i = 0; i < 8; i++) {
		for(int j = 0; j < 4; j++) {
			keys_out[i * 4 + j] = t[j * 8 + i];
		}
	}
}
inline void speckw128key32(uint64_t RoundKeys[32],
	uint64_t KeyLo, uint64_t KeyHi) {
	speck128key32(RoundKeys, KeyLo, KeyHi);
	speck128key32T(RoundKeys, RoundKeys);
}
#ifdef __cplusplus
#include <array>
auto Speck128RNG(uint64_t KeyLo, uint64_t KeyHi) {
	std::array<uint64_t, 4> CtLo, CtHi;
	__m256i Counter = _mm256_set1_epi32(0);
	std::array<uint64_t, 32> RoundKeys;
	speckw128key32(RoundKeys.data(), KeyLo, KeyHi);
	for(int i = 0; i < 4; i++) {
		Counter = _mm256_add_epi64(
			Counter, _mm256_set_epi64x(1, 0, 0, 0));
		speckw128u128iter(Counter, CtLo.data(),
			CtHi.data(), RoundKeys.data());
	}
	return
		[CtLo, CtHi, Counter, RoundKeys](
			void) mutable -> std::array<uint64_t, 2> {
			Counter = _mm256_add_epi64(Counter,
				_mm256_set_epi64x(1, 0, 0, 0));
			speckw128u128iter(Counter, CtLo.data(),
				CtHi.data(), RoundKeys.data());
			return std::array<uint64_t, 2>{
				CtLo[0], CtHi[0]};
		};
}
#endif