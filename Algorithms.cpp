/*
 * Algorithms.cpp
 *
 *  Created on: 2017/05/21
 *      Author: work
 */
#include <vector>
#include <algorithm>
#include <memory>

#include "Logger.h"
#include "Algorithms.h"

namespace NPCA {
namespace Algorithms {
template <class T>
T FindMedian(const T* arr, const int num) {
	std::vector<T> tmp(arr, arr + num);
	std::nth_element(tmp.begin(), tmp.begin() + num / 2, tmp.end());
	return tmp[num / 2];
}

template int FindMedian <int> (const int* arr, const int num);
template float FindMedian<float>(const float* arr, const int num);
template double FindMedian<double>(const double* arr, const int num);

} /* namespace Algorithms */
}/* namespace NPCA */
