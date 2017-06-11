//============================================================================
// Name        : PointCloudAlgorithms.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
// for testing againts opencv
#include "Logger.h"
#include "Algorithms.h"
#include "Kdtree.h"
using namespace std;

namespace NPCA {
bool test_Kd_tree_knn(void);
bool test_Kd_tree_rnn(void);
bool test_matrix_multiplication(void);
}

int main() {
	LOG_INFO("!!!Hello World!!!" << std::endl); // prints !!!Hello World!!!
//	NPCA::test_Kd_tree_knn();
//	NPCA::test_Kd_tree_rnn();
	NPCA::test_matrix_multiplication();
	return 0;
}
