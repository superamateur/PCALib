//============================================================================
// Name        : PointCloudAlgorithms.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <random>
#include <chrono>

// for testing againts opencv
#include "flann/flann.hpp"

#include "Logger.h"
#include "Algorithms.h"
#include "Kdtree.h"
using namespace std;

void gen_test_data(std::vector<NPCA::FloatType>& data, int data_num, std::vector<NPCA::FloatType>& test_set, int test_num, int dim)
{
	std::random_device rd; //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<NPCA::FloatType> dis(0., 100.);
	data.resize(data_num * dim);
	for (int i = 0; i < data_num * dim; ++i) {
		data[i] = dis(gen);
	}

	test_set.resize(test_num * dim);
	for (int i = 0; i < test_num * dim; ++i) {
		test_set[i] = dis(gen);
	}
}

struct StopWatch {
	std::chrono::time_point<std::chrono::system_clock> _start;
	StopWatch() : _start(std::chrono::system_clock::now()) {}
	double GetElapsedMS(void) { return std::chrono::duration<double>(std::chrono::system_clock::now() - _start).count() * 1000; }
	void Reset(void) { _start = std::chrono::system_clock::now(); }
	void Check(const std::string& msg) {
		cout << msg << ": " << GetElapsedMS() << " ms" << endl;
		Reset();
	}
};
int main() {
	DLOG("!!!Hello World!!!"); // prints !!!Hello World!!!
	std::vector<NPCA::FloatType> data_set, test_set;
	StopWatch sw;
	const int data_num = 100000;
	const int test_data_num = 1000;
	const int k = 2;
	const int tree_node_num = 1000;
	gen_test_data(data_set, data_num, test_set, test_data_num, k);
	NPCA::CKdtree kt(k, data_num / tree_node_num);

	sw.Reset();
	kt.InsertNextPoints(data_set.data(), data_num);
	sw.Check(":: Inserting Point: ");

	kt.BuildTree();
	sw.Check(":: Building tree : ");

	std::vector<int> my_knn(test_data_num);
	for(int i = 0; i < test_data_num; ++i) {
		my_knn[i] = kt.NNSearch(&test_set.data()[i * 2]);
	}
	sw.Check(":: NN search : ");

	// naive search
	std::vector<int> naive_knn(test_data_num);
	{
		for (int j = 0; j < test_data_num; ++j) {
			NPCA::FloatType min = std::numeric_limits<NPCA::FloatType>::max();
			int xNN = -1;
			for (int i = 0; i < data_num; ++i) {
				const auto d2 = (test_set[j * 2] - data_set[i * 2]) * (test_set[j * 2] - data_set[i * 2])
						+ (test_set[j * 2 + 1] - data_set[i * 2 + 1]) * (test_set[j * 2 + 1] - data_set[i * 2 + 1]);
				if (min > d2) {
					min = d2;
					xNN = i;
				}
			}
			naive_knn[j] = xNN;
		}
	}
	sw.Check(":: Naive search : ");

	cv::Mat cvdata(data_num, k, CV_32FC1, (void*)data_set.data());
	cv::flann::KDTreeIndexParams indexParams(4);
	cv::flann::Index index(cvdata, indexParams, cvflann::FLANN_DIST_EUCLIDEAN);
	sw.Check(":: CV tree building : ");

	cv::Mat qpoint(test_data_num, k, CV_32FC1, (void*)test_set.data());
	cv::Mat output_indices(test_data_num, 1, CV_32SC1);
	cv::Mat output_distance(test_data_num, 1, CV_32FC1);
	index.knnSearch(qpoint, output_indices, output_distance, test_data_num);
	sw.Check(":: CV NN search ");

	std::vector<int> cv_knn(test_data_num);
	for (int i = 0; i < test_data_num; ++i) {
		cv_knn[i] = output_indices.at<int>(i, 0);
	}

	for (int i = 0; i < test_data_num; ++i) {
		if(my_knn[i] != naive_knn[i] || naive_knn[i] != cv_knn[i] || cv_knn[i] != my_knn[i]) {
			DLOG("ERROR at " << i << ". My Knn = " << my_knn[i] << ". Naive Knn = " << naive_knn[i] <<". cv Knn = " << cv_knn[i]);
			return -1;
		}
	}
	DLOG("ALL MATCHED");
	return 0;
}
