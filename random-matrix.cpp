#include <iostream>
#include <cmath>
#include <cstdint>
#include <string>
#include "Eigen/Dense"
#include "stat-synth.h"

using namespace Eigen;

double eigenvalue_split(const int N) {
	const uint32_t KeyHi = rdrand32();
	const uint32_t KeyLo = rdrand32();
	const MatrixXd AsymNormal = MatrixXd::NullaryExpr(
		N, N, [=](const Index row, const Index col) {
			const double ud64_1 = u01(speck64u96(row,
				(const uint32_t)col, KeyHi, KeyLo));
			const double ud64_2
				= u01(speck64u96((1000 + col),
					(const uint32_t)(1000 + row),
					KeyLo, KeyHi));
			const double n01 = n01u01d_re(
				n01u01d_r(ud64_1), ud64_2);
			return n01;
		});
	const MatrixXd SymNormal
		= AsymNormal + AsymNormal.transpose();
	SelfAdjointEigenSolver<MatrixXd> Solver(SymNormal);
	if(Solver.info() != Success) {
		std::cout << "Solver Error" << std::endl;
		return -1;
	}
	const VectorXd eigenvalues = Solver.eigenvalues();
	return eigenvalues[N / 2] - eigenvalues[N / 2 - 1];
}

double eigenvalue_split_01(const int N) {
	const uint32_t KeyHi = rdrand32();
	const uint32_t KeyLo = rdrand32();
	const MatrixXd AsymNormal = MatrixXd::NullaryExpr(
		N, N, [=](const Index row, const Index col) {
			const uint64_t u01 = speck64u96(row,
				(const uint32_t)col, KeyHi, KeyLo);
				return (u01&1)?1:-1;
		});
	const MatrixXd SymNormal
		= AsymNormal + AsymNormal.transpose();
	SelfAdjointEigenSolver<MatrixXd> Solver(SymNormal);
	if(Solver.info() != Success) {
		std::cout << "Solver Error" << std::endl;
		return -1;
	}
	const VectorXd eigenvalues = Solver.eigenvalues();
	return eigenvalues[N / 2] - eigenvalues[N / 2 - 1];
}

int main(int ac, char *av[]) {
	if(ac < 2) { return 1; }
	std::ios::sync_with_stdio(false);
	const int N = std::stoi(av[1]);
	const int M = std::stoi(av[2]);
	VectorXd Splits = VectorXd::NullaryExpr(
		M, [=](void) { return eigenvalue_split_01(N); });
	IOFormat CsvFormat(
		StreamPrecision, DontAlignCols, "", "\n");
	std::cout << "Eigenvalues" << std::endl
			  << Splits.format(CsvFormat) << std::endl;
	return 0;
}