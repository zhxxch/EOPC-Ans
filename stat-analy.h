#pragma once
/*  __ |  \   /   __|
      /     /    |   
    ___|  _/ _\ \___|  */
const double Avogadro = 6.022140857e23;
const double Boltzmanm = 1.38064852e-23;
typedef const double Avogadro;
typedef const double Boltzmanm;
Avogadro N_A = 6.022140857e23;
Boltzmanm k_B = 1.38064852e-23;
#include <math.h>
inline double ks_cdf(const double MaxDiffXsqrtN, const int Num)
{
	double p = 0, t = MaxDiffXsqrtN * sqrt(Num);
	if (Num > 100)
		return (1 - exp(-2. * MaxDiffXsqrtN * MaxDiffXsqrtN));
	for (int k = (int)ceil(t); k <= Num; k++)
	{
		p += tgamma(Num + 1) / (tgamma(k + 1) * tgamma(Num - k + 1)) * pow(k - t, k) * pow(t + Num - k, Num - k - 1);
	}
	return (1 - t * p / pow(Num, Num));
}
inline double ks_test_eval(double SamplesCDF[], const int Num, const int is_upper)
{
	double k = 0;
	for (int j = 0; j < Num; j++)
		k = fmax(k, is_upper ? (SamplesCDF[j] - (double)j / Num)
							 : ((double)(j + 1) / Num - SamplesCDF[j]));
	return k * sqrt(Num);
}
inline double sum_kahan(double t[1], const double sum, const double x)
{
	const double c = x - t[0];
	const double s0 = sum + x;
	t[0] = (s0 - sum) - x;
	return s0;
}
inline double lil_k(const double p, const double n)
{
	/* Almost surely, for all e>0, there exist infinitely many n such that:
	S_n - np > (1-e)sqrt(2np(1-p)ln(ln(n)));
	almost surely, for all e>0, for every n larger than a threshold value N:
	S_n - np < (1+e)sqrt(2np(1-p)ln(ln(n)));
	 */
	return sqrt(2 * n * p * (1 - p) * log(log(n)));
}
inline double lil_hw(const double variance, const double n)
{
	/* lim sup S_n / sqrt(2n*V*ln(ln(n))) = 1 ,n->inf */
	return sqrt(2 * n * variance * log(log(n)));
}
inline double normal_cdf(const double x)
{
	/* Phi(x) = 0.5 * erf(-x / sqrt(2)) */
	const double inv_sqrt2 = 0.7071067811865475244;
	return (1 + erf(-x * inv_sqrt2)) / 2;
}
inline double beta(double z, double w)
{
	return (exp(tgamma(z) + tgamma(w) - tgamma(z + w)));
}
inline double exp_cdf(const double x, const double lambda)
{
	return (1.0 - exp(-lambda * x));
}
inline double incomplete_gamma(const double n, const double x)
{
	/* Gamma(n, x) = (n-1)!*exp(-x)*Sigma_(k=0)^(n-1)(x^k/k!)
	   = tgamma(n)*exp(-x)**Sigma_(k=0)^(n-1)(pow(x, k)/tgamma(k+1))*/
	double icmp_gamma = 0;
	for (double k = 0; k < n; k++)
		icmp_gamma += pow(x, k) / tgamma(k + 1);
	return (icmp_gamma * tgamma(n) * exp(-x));
}
inline double chi2_cdf(const double x, const double Freedom)
{
	/* lower_incomplete_gamma(freedom/2, x/2)/Gamma(freedom/2) */
	return ((tgamma(Freedom / 2) - incomplete_gamma(Freedom / 2, x / 2)) / tgamma(Freedom / 2));
}
inline double chi2_test(const double Obs[], const double Epc[], const int Num)
{
	double r = 0;
	for (int i = 0; i < Num; i++)
	{
		double sq = Obs[i] - Epc[i];
		r += sq * sq / Epc[i];
	}
	return r;
}
inline double chi2_test_grad(const double Obs, const double Epc, const double dObs, const double dEpc)
{
	/* = D\chi^2 */
	double diff = Obs - Epc;
	return ((2 * diff * (dObs - dEpc) - diff * diff * dEpc) / (Epc * Epc));
}
inline double g_test(const double Obs[], const double Epc[], const int Num)
{
	double r = 0;
	for (int i = 0; i < Num; i++)
		r += log(Obs[i] / Epc[i]) * Obs[i];
	return 2 * r;
}

inline void gap_test(const double Obs[], const int Num,
					 const double Interval[2], int GapFreq[], const int GapMaxLen)
{
	for (long i = 0, GapLength = 0; i < Num; i++)
	{
		if (Obs[i] < Interval[1] && Obs[i] >= Interval[0])
		{
			if (GapLength >= GapMaxLen)
				GapFreq[GapMaxLen]++;
			else
				GapFreq[GapLength]++;
			GapLength = 0;
		}
		else
			GapLength++;
	}
}
inline void gap_test_pdf(double Pdf[],
						 const double IntervalMeasure, const int GapMaxLen)
{
	for (int GapLength = 0; GapLength < GapMaxLen; GapLength++)
	{
		Pdf[GapLength] = IntervalMeasure * pow(1.0 - IntervalMeasure, GapLength);
	}
	Pdf[GapMaxLen] = pow(1.0 - IntervalMeasure, GapMaxLen);
}
void run_test(const double obs[], const long num,
			  long run_freq[], const long run_max_len)
{
	for (long i = 1, run_len = 1; i < num; i++)
	{
		if (obs[i] >= obs[i - 1])
		{
			run_len++;
			continue;
		}
		else if (run_len >= run_max_len)
			run_freq[run_max_len]++;
		else
			run_freq[run_len]++;
		run_len = 1;
	}
}

void run_isolated_test(const double obs[], const long num,
					   long run_freq[], const long run_max_len)
{
	for (long i = 1, run_len = 1; i < num; i++)
	{
		if (obs[i] >= obs[i - 1])
		{
			run_len++;
			continue;
		}
		else if (run_len >= run_max_len)
			run_freq[run_max_len]++;
		else
			run_freq[run_len]++;
		run_len = 1;
		i++;
	}
}
