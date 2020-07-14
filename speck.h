#pragma once
/*   __ |  \   /   __|
 *     /     /    |
 *   ___|  _/ _\ \___|  */
inline unsigned long long
speck64u96(const unsigned long long Pt,
		   unsigned int KeyLo, unsigned int KeyHiLo, unsigned int KeyHiHi)
{
	
#define _RR(x, r) (((x) >> r) | ((x) << ((sizeof(x) * 8) - r)))
#define _LL(x, r) (((x) << r) | ((x) >> ((sizeof(x) * 8) - r)))
#define _SPECKXXR(x, y, k) (y = _RR(x ^ y, 3), x = _LL((unsigned long long)((x ^ (k)) - y), 8))
#define _SPECKXX(Hi, Lo, k) (Hi = (_RR(Hi, 8) + Lo) ^ (k), Lo = _LL(Lo, 3) ^ Hi)
	unsigned int CtHi = Pt >> 32,
			 CtLo = Pt & 0xffffffffu;
	for (int i = 0; i < 26;)
		(_SPECKXX(CtHi, CtLo, KeyLo),
		 _SPECKXX(KeyHiLo, KeyLo, i++),
		 _SPECKXX(CtHi, CtLo, KeyLo),
		 _SPECKXX(KeyHiHi, KeyLo, i++));
	return ((((unsigned long long)CtHi) << 32) | CtLo);
#undef _RR
#undef _LL
#undef _SPECKXXR
#undef _SPECKXX
}
