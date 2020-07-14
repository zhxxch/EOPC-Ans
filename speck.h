#pragma once
/*   __ |  \   /   __|
 *     /     /    |
 *   ___|  _/ _\ \___|  */
#ifdef __cplusplus
extern "C" {
#endif
inline unsigned long long speck64u96(
	const unsigned long long Pt, unsigned int KeyLo,
	unsigned int KeyHiLo, unsigned int KeyHiHi) {
	extern unsigned int _rotl(unsigned int, int);
	extern unsigned int _rotr(unsigned int, int);
#define _SPECKXXR(x, y, k) \
	(y = _rotr(x ^ y, 3),  \
		x = _rotl(         \
			(unsigned long long)((x ^ (k)) - y), 8))
#define _SPECKXX(Hi, Lo, k)          \
	(Hi = (_rotr(Hi, 8) + Lo) ^ (k), \
		Lo = _rotl(Lo, 3) ^ Hi)
	unsigned int CtHi = Pt >> 32,
				 CtLo = Pt & 0xffffffffu;
	for(int i = 0; i < 26;)
		(_SPECKXX(CtHi, CtLo, KeyLo),
			_SPECKXX(KeyHiLo, KeyLo, i++),
			_SPECKXX(CtHi, CtLo, KeyLo),
			_SPECKXX(KeyHiHi, KeyLo, i++));
	return ((((unsigned long long)CtHi) << 32) | CtLo);
#undef _SPECKXXR
#undef _SPECKXX
}
inline void speck128u128(unsigned long long *Lo,
	unsigned long long *Hi, unsigned long long KeyLo,
	unsigned long long KeyHi) {
	extern unsigned __int64 _rotl64(
		unsigned __int64, int);
	extern unsigned __int64 _rotr64(
		unsigned __int64, int);
	typedef unsigned long long ull;
	ull hi = *Hi;
	ull lo = *Lo;
	for(unsigned long long i = 0; i < 32; i++) {
		hi = (_rotr64(hi, 8) + lo) ^ KeyLo;
		lo = _rotl64(lo, 3) ^ hi;
		KeyHi = (_rotr64(KeyHi, 8) + KeyLo) ^ i;
		KeyLo = _rotl64(KeyLo, 3) ^ KeyHi;
	}
	*Hi = hi;
	*Lo = lo;
}
#ifdef __cplusplus
}
#endif