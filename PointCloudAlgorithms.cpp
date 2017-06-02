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
	std::mt19937 gen(1); //Standard mersenne_twister_engine seeded with rd()
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

NPCA::FloatType variance(const std::vector<NPCA::FloatType>& data)
{
	NPCA::FloatType sum_1 = std::accumulate(data.begin(), data.end(), NPCA::FloatType(0), [&](NPCA::FloatType acc, NPCA::FloatType imm) {return acc + imm;}) / data.size();
	NPCA::FloatType sum_2 = std::accumulate(data.begin(), data.end(), NPCA::FloatType(0), [&](NPCA::FloatType acc, NPCA::FloatType imm) {return acc + imm * imm;}) / data.size();
	return std::sqrt(sum_2 - sum_1 * sum_1);
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
	const int data_num = 1024 * 1024;
	const int test_data_num = 100;
	const int test_data_k_num = 100;
	const int k = 2;
	const int tree_node_num = (int)std::sqrt((double)data_num);
	gen_test_data(data_set, data_num, test_set, test_data_num, k);
	NPCA::CKdtree kt(k, data_num / tree_node_num);

	sw.Reset();
	kt.InsertNextPoints(data_set.data(), data_num);
	sw.Check(":: Inserting Point: ");

	kt.BuildTree();
	sw.Check(":: Building tree : ");

	std::vector<int> my_knn;
	for(int i = 0; i < test_data_num; ++i) {
		auto tmp = kt.kNNSearch(&test_set.data()[i * 2], test_data_k_num);
		my_knn.insert(my_knn.end(), tmp.begin(), tmp.end());
	}
	sw.Check(":: NN search : ");

	// naive search
	std::vector<int> naive_knn;
	{
		for (int j = 0; j < test_data_num; ++j) {
			std::vector<NPCA::SQueryIndex> d_array(data_num);
			for (int i = 0; i < data_num; ++i) {
				d_array[i]._distance = (test_set[j * 2] - data_set[i * 2])
						* (test_set[j * 2] - data_set[i * 2])
						+ (test_set[j * 2 + 1] - data_set[i * 2 + 1])
								* (test_set[j * 2 + 1] - data_set[i * 2 + 1]);
				d_array[i]._id = i;
			}
			std::nth_element(d_array.begin(), d_array.begin() + test_data_k_num, d_array.end());
			std::sort(d_array.begin(), d_array.begin() + test_data_k_num);
			for(int i = 0; i < test_data_k_num; ++i) {
				naive_knn.push_back(d_array[i]._id);
			}
		}
	}

	sw.Check(":: Naive search : ");
#if 0
	cv::Mat cvdata(data_num, k, CV_32FC1, (void*)data_set.data());
	cv::flann::KDTreeIndexParams indexParams(tree_node_num);
	cv::flann::Index index(cvdata, indexParams, cvflann::FLANN_DIST_L2);
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
#endif

	for (int i = 0; i < test_data_num; ++i) {
		for (int j = 0; j < test_data_k_num; ++j) {
			const auto my_knn_id = my_knn[i * test_data_k_num + j];
			const auto naive_knn_id = naive_knn[i * test_data_k_num + j];
			if (my_knn_id != naive_knn_id) {
				DLOG("ERROR at " << i << "[ " << j <<"] . My Knn = " << my_knn_id << ". Naive Knn = " << naive_knn_id);
				DLOG("Input: "  << test_set[i * k] << ", " << test_set[i * k + 1]);
				DLOG("my Knn: " << data_set[my_knn_id * k] << ", " << data_set[my_knn_id * k + 1] << ". hypot = " << std::hypot(data_set[my_knn_id * k] - test_set[i * k], data_set[my_knn_id * k + 1] - test_set[i * k + 1]));
				DLOG("naive Knn: " << data_set[naive_knn_id * k] << ", " << data_set[naive_knn_id * k + 1] << ". hypot = " << std::hypot(data_set[naive_knn_id * k] - test_set[i * k], data_set[naive_knn_id * k + 1] - test_set[i * k + 1]));
				return -1;
			}
		}
	}
	DLOG("ALL MATCHED");
	return 0;
}
