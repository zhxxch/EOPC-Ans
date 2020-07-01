#include <iostream>
#include <cmath>
#include "Eigen/Dense"
#include "stat-synth.h"
#include "small-world-json.h"
using namespace std;
using namespace Eigen;
MatrixXf GenRandomEdge(const MatrixXf &EdgesLocal,
	const int num_random_edge) {
	MatrixXf REdges = MatrixXf::Zero(
		EdgesLocal.rows(), EdgesLocal.cols());
	for(int i = 0; i < num_random_edge;) {
		const int row = rdrand32() % EdgesLocal.rows();
		const int col = rdrand32() % EdgesLocal.cols();
		if(row < col || EdgesLocal(row, col) > 0)
			continue;
		REdges(row, col) = 1;
		i++;
	}
	return REdges;
}
MatrixXf GenSmallWorld(
	const int L, const int Z, const float p) {
	const int NumNodes = L;
	const int Radius = Z / 2;
	const int NumExtraEdges = (int)(p * L * Radius);
	const MatrixXf EdgesL = MatrixXf::NullaryExpr(
		NumNodes, NumNodes,
		[=](const Index row,
			const Index col) -> float {
			return (((row > col && row - col <= Radius)
						|| (row < col
							&& NumNodes - (col - row)
								<= Radius))
					? 0.5f
					: 0.f);
		});
	const MatrixXf EdgesLocal
		= EdgesL + EdgesL.transpose();
	const MatrixXf RandomEdges
		= GenRandomEdge(EdgesLocal, NumExtraEdges);
	const MatrixXf EdgesSym
		= (EdgesLocal + EdgesLocal.transpose()
			+ RandomEdges + RandomEdges.transpose())
			  .unaryExpr([](float v) -> float {
				  return v > 0 ? v : INFINITY;
			  });
	return EdgesSym;
}
MatrixXf GraphMetrics(const MatrixXf &EdgesSym) {
	MatrixXf metric = EdgesSym;
	for(int i = 0; i < EdgesSym.cols(); i++) {
		metric(i, i) = 0;
	}
	for(int n = 0; n < EdgesSym.cols(); n++) {
		for(int src = 0; src < EdgesSym.rows();
			src++) {
			for(int dst = 0; dst < EdgesSym.cols();
				dst++) {
				const float metric_pass_n
					= metric(src, n) + metric(n, dst);
				metric(src, dst)
					= (metric(src, dst) > metric_pass_n
							? metric_pass_n
							: metric(src, dst));
			}
		}
	}
	return metric;
}
double AvgMetrics(const MatrixXf metrics) {
	const double L = (double)metrics.cols();
	return metrics.array().mean()
		* ((L * L) / (L * L - L));
}
int main(int ac, char *av[]) {
	if(ac < 4) return -1;
	const int L = stoi(av[1]);
	const int Z = stoi(av[2]);
	const float p = stof(av[3]);
	const MatrixXf small_world0
		= GenSmallWorld(L, Z, 0);
	const MatrixXf metric0
		= GraphMetrics(small_world0);
	print_graph_js(stdout, small_world0, L);
	print_metrics_js(stdout, metric0);
	double avg_l0 = AvgMetrics(metric0);
	
	const int NumIter = 50;
	cout << "var l_w_p = [\n";
	for(float p_n = 0.001f; p_n < 0.2; p_n += p) {
		double avg_l = 0;
#pragma omp parallel for num_threads(2) reduction(+ : avg_l)
		for(int i = 0; i < NumIter; i++) {
			const MatrixXf small_world
				= GenSmallWorld(L, Z, p_n);
			const MatrixXf Metric
				= GraphMetrics(small_world);
			avg_l += AvgMetrics(Metric) / NumIter;
		}
		cout << "[" << p_n << "," << avg_l
			 << "],";
	}
	cout << "];\n";
	
	return 0;
}