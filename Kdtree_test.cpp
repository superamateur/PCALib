/*
 * Kdtree_test.cpp
 *
 *  Created on: 2017/06/03
 *      Author: work
 */
#include <cmath>
#include <algorithm>
#include "Kdtree.h"
#include "Logger.h"
#include "TestTools.h"

namespace NPCA {
bool test_Kd_tree_knn(void) {
	LOG_INFO("Started" << std::endl);
	StopWatch sw;
	const int data_num = 1024 * 1024;
	const int test_data_num = 100;
	const int test_data_k_num = 100;
	const int k = 2;
	const int tree_node_num = (int) std::sqrt((double) data_num);

	std::vector<NPCA::FloatType> data_set, test_set;
	CTestTools::gen_test_data(data_set, data_num, test_set, test_data_num, k);
	NPCA::CKdtree kt(k, data_num / tree_node_num);

	sw.Reset();
	kt.InsertNextPoints(data_set.data(), data_num);
	sw.Check(":: Inserting Point: ");

	kt.BuildTree();
	sw.Check(":: Building tree : ");

	std::vector<int> my_knn;
	for (int i = 0; i < test_data_num; ++i) {
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
			std::nth_element(d_array.begin(), d_array.begin() + test_data_k_num,
					d_array.end());
			std::sort(d_array.begin(), d_array.begin() + test_data_k_num);
			for (int i = 0; i < test_data_k_num; ++i) {
				naive_knn.push_back(d_array[i]._id);
			}
		}
	}

	sw.Check(":: Naive search : ");

	for (int i = 0; i < test_data_num; ++i) {
		for (int j = 0; j < test_data_k_num; ++j) {
			const auto my_knn_id = my_knn[i * test_data_k_num + j];
			const auto naive_knn_id = naive_knn[i * test_data_k_num + j];
			if (my_knn_id != naive_knn_id) {
				LOG_ERROR(
						"ERROR at " << i << "[ " << j <<"] . My Knn = " << my_knn_id << ". Naive Knn = " << naive_knn_id << std::endl);
				LOG_ERROR(
						"Input: " << test_set[i * k] << ", " << test_set[i * k + 1] << std::endl);
				LOG_ERROR(
						"my Knn: " << data_set[my_knn_id * k] << ", " << data_set[my_knn_id * k + 1] << ". hypot = " << std::hypot(data_set[my_knn_id * k] - test_set[i * k], data_set[my_knn_id * k + 1] - test_set[i * k + 1]) << std::endl);
				LOG_ERROR(
						"naive Knn: " << data_set[naive_knn_id * k] << ", " << data_set[naive_knn_id * k + 1] << ". hypot = " << std::hypot(data_set[naive_knn_id * k] - test_set[i * k], data_set[naive_knn_id * k + 1] - test_set[i * k + 1]) << std::endl);
				return false;
			}
		}
	}
	LOG_INFO("ALL MATCHED");
	return true;
}

bool test_Kd_tree_rnn(void) {
	LOG_INFO("Started" << std::endl);
	StopWatch sw;
	const int data_num = 1024 * 1024;
	const int test_data_num = 1000;
	const int R = 1.;
	const int k = 2;
	const int tree_node_num = (int) std::sqrt((double) data_num);

	std::vector<NPCA::FloatType> data_set, test_set;
	CTestTools::gen_test_data(data_set, data_num, test_set, test_data_num, k);
	NPCA::CKdtree kt(k, data_num / tree_node_num);

	sw.Reset();
	kt.InsertNextPoints(data_set.data(), data_num);
	sw.Check(":: Inserting Point: ");

	kt.BuildTree();
	sw.Check(":: Building tree : ");

	std::vector<std::vector<int>> my_rnn;
	for (int i = 0; i < test_data_num; ++i) {
		auto tmp = kt.rNNSearch(&test_set.data()[i * 2], R);
		std::sort(tmp.begin(), tmp.end());
		my_rnn.push_back(tmp);
	}
	sw.Check(":: NN search : ");

	// naive search
	std::vector<std::vector<int>> naive_rnn;
	{
		const double R2 = R * R;
		for (int j = 0; j < test_data_num; ++j) {
			std::vector<int> tmp;
			for (int i = 0; i < data_num; ++i) {
				const auto distance2 = (test_set[j * 2] - data_set[i * 2])
						* (test_set[j * 2] - data_set[i * 2])
						+ (test_set[j * 2 + 1] - data_set[i * 2 + 1])
								* (test_set[j * 2 + 1] - data_set[i * 2 + 1]);
				if(R2 >= distance2) {
					tmp.push_back(i);
				}
			}
			naive_rnn.push_back(tmp);
		}
	}

	sw.Check(":: Naive search : ");

	for (int i = 0; i < test_data_num; ++i) {
		for (int j = 0; j < std::min<int>(int(my_rnn[i].size()), int(naive_rnn[i].size())); ++j) {
			const auto my_rnn_id = my_rnn[i][j];
			const auto naive_rnn_id = naive_rnn[i][j];
			if (my_rnn_id != naive_rnn_id) {
				LOG_INFO("ERROR at " << i << "[ " << j <<"] . My Rnn = " << my_rnn_id << ". Naive Rnn = " << naive_rnn_id << std::endl);
				return false;
			}
		}
		if (my_rnn[i].size() != naive_rnn[i].size()) {
			LOG_INFO("ERROR Size ]" << i << "] not match: My = " << my_rnn[i].size() << ", Naive = " << naive_rnn[i].size() << std::endl);
			return false;
		}
	}
	LOG_INFO("ALL MATCHED" << std::endl);
	return true;
}
}
