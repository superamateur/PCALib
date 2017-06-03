/*
 * Kdtree.cpp
 *
 *  Created on: 2017/05/20
 *      Author: work
 */

#include <algorithm>
#include <iostream>
#include <sstream>
#include <numeric>
#include <limits>
#include <queue>

#include "Kdtree.h"
#include "Logger.h"
#include "Algorithms.h"

namespace NPCA {

CKdtree::CKdtree() :
		_k(0), _atomic_num(1)
{
	// TODO Auto-generated constructor stub

}

CKdtree::CKdtree(int k) :
		CKdtree() {
	// TODO Auto-generated constructor stub
	_k = k;
	_points.reset(new FloatMultiArrayType(_k));
}

CKdtree::CKdtree(int k, int a) :
		CKdtree(k) {
	// TODO Auto-generated constructor stub
	_atomic_num = a;
}

CKdtree::~CKdtree() {
	// TODO Auto-generated destructor stub
}

int CKdtree::InsertNextPoints(const FloatType* p, int num) {
	int count = 0;
	for (int i = 0; i < num; ++i) {
		for (int j = 0; j < _k; ++j, ++count) {
			_points->at(j).push_back(p[count]);
		}
	}
	return (int)_points->front().size();
}

void CKdtree::BuildTree() {
	SBbox bbox;
	bbox._lower.resize(_k);
	bbox._upper.resize(_k);
	for (int i = 0; i < _k; ++i) {
		bbox._lower[i] = std::numeric_limits<FloatType>::lowest();
		bbox._upper[i] = std::numeric_limits<FloatType>::max();
	}
	std::vector<int> point_ids(_points->front().size());
	std::iota(point_ids.begin(), point_ids.end(), 0);
	_root = std::make_shared < CKdTreeNode
			> (CKdTreeNode(_k, _atomic_num, _points, point_ids, bbox));
	_root->Build();
}

std::vector<int> CKdtree::kNNSearch(const FloatType* p, int num)
{
	const auto search_result = _root->NNSearch(p, num);
	auto qidx = std::get<0>(search_result);
	std::sort(qidx.begin(), qidx.end());
	std::vector<int> ret(qidx.size());
	std::transform(qidx.begin(), qidx.end(), ret.begin(), [](const SQueryIndex& u) { return u._id; });
	return ret;
}

std::vector<int> CKdtree::rNNSearch(const FloatType* p, const FloatType R) {
	return _root->rNNSearch(p, R);
}

void CKdtree::Debug() const {

}

} /* namespace NPCA */
