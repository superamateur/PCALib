/*
 * TestTools.cpp
 *
 *  Created on: 2017/06/03
 *      Author: work
 */
#include <random>

#include "TestTools.h"

namespace NPCA {

CTestTools& CTestTools::Create() {
	static CTestTools instance;
	return instance;
}

void CTestTools::gen_test_data(std::vector<NPCA::FloatType>& data, int data_num, std::vector<NPCA::FloatType>& test_set, int test_num, int dim)
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

CTestTools::~CTestTools() {
	// TODO Auto-generated destructor stub
}

} /* namespace NPCA */
