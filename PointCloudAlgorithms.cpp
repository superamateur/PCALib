//============================================================================
// Name        : PointCloudAlgorithms.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>

#include "Logger.h"
#include "Algorithms.h"
#include "Kdtree.h"
using namespace std;

int main() {
	DLOG("!!!Hello World!!!"); // prints !!!Hello World!!!
	NPCA::CKdtree kt(2);
	DLOG("--");
	NPCA::FloatType arr[] = {2, 2, 3, 1, 3, 4};
	kt.InsertNextPoints(arr, 3);
	DLOG("--");
	kt.BuildTree();
	DLOG("--");
	NPCA::FloatType p[2] = {-2, -2};
	int nn = kt.NNSearch(p);
	DLOG("--");
	DLOG("NN = " << nn << ". Closet point = " << arr[nn * 2] << ", " << arr[nn * 2 + 1]);
	return 0;
}
