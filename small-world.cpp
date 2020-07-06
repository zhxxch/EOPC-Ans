#include <iostream>
#include <cmath>
#include "Eigen/Dense"
#include "stat-synth.h"
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
double AvgMetrics(const MatrixXf &metrics) {
	const double L = (double)metrics.cols();
	return metrics.array().mean()
		* ((L * L) / (L * L - L));
}
int PathNextNode(const MatrixXf &metrics,
	const int srcID, const int dstID,
	const float metric) {
	VectorXf metrics_to_dst = metrics.col(dstID);
	VectorXf metrics_to_src = metrics.col(srcID);
	for(int i = 0; i < metrics_to_dst.rows(); i++) {
		if(metrics_to_src(i) == 1
			&& metrics_to_dst(i) == (metric - 1)) {
			return i;
		}
	}
	return -1;
}
VectorXf PathPassNodes(const MatrixXf &metrics,
	const int srcID, const int dstID) {
	const int NumNodes = (int)metrics.cols();
	VectorXf pass = VectorXf::Zero(NumNodes);
	int pass_node_id = srcID;
	for(float metric = metrics(srcID, dstID);
		pass_node_id != dstID; metric = metric - 1) {
		pass_node_id = PathNextNode(
			metrics, pass_node_id, dstID, metric);
		pass(pass_node_id) = 1;
	}
	return pass;
}
VectorXf Betweenness(const MatrixXf &metrics) {
	const int N = (int)metrics.cols();
	MatrixXf path_pass_count
		= MatrixXf::Zero(N, N * (N - 1) / 2);
	for(int src_id = 0, counter_id = 0; src_id < N;
		src_id++) {
		for(int dst_id = src_id + 1; dst_id < N;
			dst_id++) {
			path_pass_count.col(counter_id)
				= PathPassNodes(
					metrics, src_id, dst_id);
			counter_id++;
		}
	}
	return path_pass_count
		* VectorXf::Ones(N * (N - 1) / 2);
}
int js_nodes(FILE *out_stream, const int NumNodes) {
	fprintf(out_stream, "var nodes=[\n");
	for(int i = 0; i < NumNodes; i++) {
		fprintf(out_stream, "{id:\"%i\"},\n", i);
	}
	fprintf(out_stream, "];\n");
	return 0;
}
template<typename T>
int js_edges(FILE *out_stream,
	const Eigen::DenseBase<T> &edges,
	const int NumNodes) {
	fprintf(out_stream, "var edges=[\n");
	for(int dst = 0; dst < NumNodes; dst++) {
		for(int src = dst + 1; src < NumNodes; src++) {
			if(edges(dst, src) < INFINITY)
				fprintf(out_stream,
					"{source:\"%i\",target:\"%i\"},\n",
					src, dst);
		}
	}
	fprintf(out_stream, "];\n");
	return 0;
}
template<typename T>
int print_metrics_js(FILE *out_stream,
	const Eigen::DenseBase<T> &metrics) {
	fprintf(out_stream, "var metrics=[\n");
	for(int dst = 0; dst < metrics.cols(); dst++) {
		for(int src = dst + 1; src < metrics.rows();
			src++) {
			fprintf(
				out_stream, "%f,", metrics(dst, src));
		}
	}
	fprintf(out_stream, "];\n");
	return 0;
}
template<typename T>
int print_graph_js(FILE *out_stream,
	const Eigen::DenseBase<T> &edges,
	const int NumNodes) {
	js_nodes(out_stream, NumNodes);
	js_edges(out_stream, edges, NumNodes);
	return 0;
}
int PrintLPRelation(const int L, const int Z,
	const float p0, const float p1, const int num_iter,
	const int num_sample) {
	cout << "var l_w_p = [\n";
	for(float p_n = p0; p_n < p1;
		p_n += (p1 - p0) / num_sample) {
		double avg_l = 0;
#pragma omp parallel for num_threads(2) reduction(+ : avg_l)
		for(int i = 0; i < num_iter; i++) {
			const MatrixXf small_world
				= GenSmallWorld(L, Z, p_n);
			const MatrixXf Metric
				= GraphMetrics(small_world);
			avg_l += AvgMetrics(Metric) / num_iter;
		}
		cout << "[" << (p_n * L * Z / 2) << ","
			 << (avg_l * Z * 3.14159265359f / L)
			 << "],";
	}
	cout << "];\n";
	return 0;
}
int PrintBetweenness(const VectorXf &Betweenness) {
	IOFormat ArrFormat(
		StreamPrecision, DontAlignCols, "", ",");
	cout << "var betweenness = [\n"
		 << Betweenness.format(ArrFormat) << "];\n";
	return 0;
}
int main(int ac, char *av[]) {
	if(ac < 4) return -1;
	const int L = stoi(av[1]);
	const int Z = stoi(av[2]);
	const float p = stof(av[3]);
	const MatrixXf small_world0
		= GenSmallWorld(L, Z, p);
	const MatrixXf metric0
		= GraphMetrics(small_world0);
	cout << "var SmallWorld={L:" << L << ",Z:" << Z
		 << ",p:" << p << "};" << endl;
	print_graph_js(stdout, small_world0, L);
	print_metrics_js(stdout, metric0);
	double avg_l0 = AvgMetrics(metric0);
	//PrintLPRelation(L, Z, 0.001f, 1.0f, 20, 20);
	PrintBetweenness(Betweenness(metric0));
	return 0;
}