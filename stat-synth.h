#pragma once
/*  __ |  \   /   __|
      /     /    |   
    ___|  _/ _\ \___|  */
#ifdef __cplusplus
extern "C"
{
#endif
	inline unsigned long long rdrand64(void)
	{
		extern int _rdrand64_step(unsigned long long *);
		unsigned long long r;
		while (!_rdrand64_step(&r))
			;
		return r;
	}
	inline unsigned int rdrand32(void)
	{
		extern int _rdrand32_step(unsigned int *);
		unsigned int r;
		while (!_rdrand32_step(&r))
			;
		return r;
	}
#ifdef __cplusplus
} //extern_C
#endif
inline float u01f32(const int i)
{
	return 0.5f + (0.25f / (1l << 30)) * i;
}
inline double u01(const long long lli)
{
	return 0.5 + (0.25 / (1ll << 62)) * lli;
}
float n01u01s_r(const float r){
	return sqrtf(-2 * logf(1 - r));
}
float n01u01s_re(const float r, const float theta)
{
	const float pi = 3.14159265358979323846f;
	return (r * cosf(2 * pi * theta));
}
float n01u01s_im(const float r, const float theta)
{
	const float pi = 3.14159265358979323846f;
	return (r * sinf(2 * pi * theta));
}
double n01u01d_r(const double r){
	return sqrt(-2 * log(1 - r));
}
double n01u01d_re(const double r, const double theta)
{
	const double pi = 3.14159265358979323846;
	return (r * cos(2 * pi * theta));
}
double n01u01d_im(const double r, const double theta)
{
	const double pi = 3.14159265358979323846;
	return (r * sin(2 * pi * theta));
}
double expu01d(const double x, const double lambda)
{
	return -log(1 - x) / lambda;
}
int randp_2sphere(double xyzt[],
				  const double u01a, const double u01b)
{
	const double x1 = u01a * 2 - 1;
	const double x2 = u01b * 2 - 1;
	const double norm2 = x1 * x1 + x2 * x2;
	if (norm2 >= 1.0)
		return 0;
	const double s = 2 * sqrt(1 - norm2);
	xyzt[2] = 1 - 2 * norm2;
	xyzt[1] = x2 * s;
	xyzt[0] = x1 * s;
	return 1;
}

