#include <iostream>
#include <cmath>
#include "Eigen/Dense"
#include "stat-synth.h"
#include "small-world-json.h"
using namespace std;
using namespace Eigen;

int main(int ac, char *av[]) {
	if(ac<4)return -1;
	const int L = stoi(av[1]);
	const int Z = stoi(av[2]);
	const float p = stof(av[3]);
	const int NumNodes = L;
	const int Radius = Z/2;
	const int NumExtraEdges = p*L*Radius;
	const MatrixXf EdgesL = MatrixXf::NullaryExpr(
		NumNodes, NumNodes,
		[=](const Index row,
			const Index col) -> float {
			return (((row > col && row - col <= Radius)
						|| (row < col
							&& NumNodes - (col - row)
								<= Radius))
					? 1.0
					: 0.0);
		});
	const MatrixXf RandomEdges = EdgesL.unaryExpr(
[=](const float v,) -> float {
			if(v)
		});
	const MatrixXf EdgesSym
		= (EdgesL + EdgesL.transpose())
			  .unaryExpr([](float v) -> float {
				  return v > 0 ? v : INFINITY;
			  });
	
	print_graph_js(stdout, EdgesSym, NumNodes);
	return 0;
}