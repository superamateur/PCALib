/*
 * TestTools.h
 *
 *  Created on: 2017/06/03
 *      Author: work
 */

#ifndef TESTTOOLS_H_
#define TESTTOOLS_H_

#include <vector>
#include <chrono>
#include <iostream>

#include "PCAlg.h"

namespace NPCA {
class CTestTools {
public:
	CTestTools(const CTestTools&) = delete;
	CTestTools& operator=(const CTestTools&) = delete;

	static CTestTools& Create();
	~CTestTools();

	static void gen_test_data(std::vector<NPCA::FloatType>& data, int data_num, std::vector<NPCA::FloatType>& test_set, int test_num, int dim);
private:
	CTestTools() = default;
};


struct StopWatch {
	std::chrono::time_point<std::chrono::system_clock> _start;
	StopWatch() : _start(std::chrono::system_clock::now()) {}
	double GetElapsedMS(void) { return std::chrono::duration<double>(std::chrono::system_clock::now() - _start).count() * 1000; }
	void Reset(void) { _start = std::chrono::system_clock::now(); }
	void Check(const std::string& msg) {
		std::cout << msg << ": " << GetElapsedMS() << " ms" << std::endl;
		Reset();
	}
};
} /* namespace NPCA */

#endif /* TESTTOOLS_H_ */
