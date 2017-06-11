/*
 * KdTreeNode.cpp
 *
 *  Created on: 2017/05/25
 *      Author: work
 */

#include <numeric>
#include <limits>
#include <algorithm>
#include <memory>
#include <cmath>
#include "KdTreeNode.h"
#include "Logger.h"

namespace NPCA {

CKdTreeNode::CKdTreeNode()
: _rank(0)
,_k(0)
, _node_size(0)
, _split(0)
, _split_val(FloatType(0))
, _has_sub_node(false)
{
	static int s_uuid = 0;
	_uuid = s_uuid++;
}

CKdTreeNode::CKdTreeNode(int k, int n, FloatMultiArrayTypePtr p,
		const std::vector<int>& i,
		const SBbox& b)
: CKdTreeNode()
{
	_k = k;
	_node_size = n;
	_points = p;
	_point_ids = i;
	_bbox = b;
}

CKdTreeNode::~CKdTreeNode() {
}

void CKdTreeNode::Build(void)
{
	if ((int) _point_ids.size() <= _node_size) {
		return;
	}

	// Calc variation along each dimension
	std::vector<FloatType> variations(_k);
	for (int i = 0; i < _k; ++i) {
		const auto& _arr = _points->at(i);
		double num_1 = 0., num_2 = 0.;
		for (const int j : _point_ids) {
			num_1 += _arr.at(j);
			num_2 += _arr.at(j) * _arr.at(j);
		}

		variations[i] = _point_ids.size() * num_2 - num_1 * num_1;
	}

	// split: the dimension with maximum variation
	_split = std::distance(variations.begin(),
			std::max_element(variations.begin(), variations.end()));
	const auto& _arr = _points->at(_split);
	FloatArrayType sub_arr(_point_ids.size());
	std::transform(_point_ids.begin(), _point_ids.end(),
			sub_arr.begin(), [&](int ids) {return _arr.at(ids);});

	// Calc median value
	std::nth_element(sub_arr.begin(), sub_arr.begin() + sub_arr.size() / 2,
			sub_arr.end());
	_split_val = sub_arr[sub_arr.size() / 2];

	// split the points about the median
	std::vector<int> left_point_ids, right_point_ids;
	for (const int i : _point_ids) {
		if (_arr[i] < _split_val) {
			left_point_ids.push_back(i);
		} else {
			right_point_ids.push_back(i);
		}
	}
	if (left_point_ids.empty() || right_point_ids.empty()) {
		return;
	}

	{
		auto left_bbox = _bbox;
		left_bbox._upper.at(_split) = _split_val;
		_left_node = std::make_shared<CKdTreeNode>(
				CKdTreeNode(_k, _node_size, _points, left_point_ids, left_bbox));
		_left_node->Build();
	}

	{
		auto right_bbox = _bbox;
		right_bbox._lower.at(_split) = _split_val;
		_right_node = std::make_shared<CKdTreeNode>(
				CKdTreeNode(_k, _node_size, _points, right_point_ids, right_bbox));
		_right_node->Build();
	}

	_has_sub_node = true;
	_rank = std::max<int>(_left_node->Rank(), _right_node->Rank()) + 1;
}

std::tuple<SQueryIndexArray, bool, int> CKdTreeNode::NNSearch(const FloatType* p, const int num)
 {
	if (num * 2 > (int)_point_ids.size() || !_has_sub_node) {
		const int effective_num = std::min<int>(num, (int)_point_ids.size());
		SQueryIndexArray min_distance_ids(_point_ids.size());
		for (int i = 0; i < (int)_point_ids.size(); ++i) {
			const auto q = GetPointById(_point_ids[i]);
			FloatType sum = 0.;
			for (int j = 0; j < _k; ++j) {
				sum += (q[j] - p[j]) * (q[j] - p[j]);
			}
			min_distance_ids[i] = SQueryIndex{sum, _point_ids[i]};
		}
		std::nth_element(min_distance_ids.begin(), min_distance_ids.begin() + effective_num,
				min_distance_ids.end());
		min_distance_ids.resize(effective_num);
		const auto max_R = std::sqrt(std::max_element(min_distance_ids.begin(), min_distance_ids.end())->_distance);
		return std::make_tuple(min_distance_ids, HasInternalSphere(p, max_R), _uuid);
	}

	SQueryIndexArray min_distance_ids;
	bool fully_internal;
	int closest_node_id;
	if (p[_split] < _split_val) {
		std::tie(min_distance_ids, fully_internal, closest_node_id) = _left_node->NNSearch(p, num);
	} else {
		std::tie(min_distance_ids, fully_internal, closest_node_id) = _right_node->NNSearch(p, num);
	}

	if(!fully_internal) {
		const auto max_R = std::sqrt(std::max_element(min_distance_ids.begin(), min_distance_ids.end())->_distance);
		if (HasInternalSphere(p, max_R)) {
			RangeNNSearch(p, min_distance_ids, closest_node_id);
			fully_internal = true;
		}
	}

	return std::make_tuple(min_distance_ids, fully_internal, closest_node_id);
}

std::vector<int> CKdTreeNode::rNNSearch(const FloatType* p, const FloatType R) {
	std::vector<int> return_point_ids;
	if (!_has_sub_node) {
		const auto R2 = R * R;
		for (int i = 0; i < (int) _point_ids.size(); ++i) {
			const auto q = GetPointById(_point_ids[i]);
			FloatType sum = 0.;
			for (int j = 0; j < _k; ++j) {
				sum += (q[j] - p[j]) * (q[j] - p[j]);
			}
			if(R2 >= sum) {
				return_point_ids.push_back(_point_ids[i]);
			}
		}
		return return_point_ids;
	}

	if (_left_node->HasIntersectWithSphere(p, R)) {
		return_point_ids = _left_node->rNNSearch(p, R);
	}
	if (_right_node->HasIntersectWithSphere(p, R)) {
		auto tmp = _right_node->rNNSearch(p, R);
		return_point_ids.insert(return_point_ids.end(), tmp.begin(), tmp.end());
	}
	return return_point_ids;
}

void CKdTreeNode::RangeNNSearch(const FloatType* p, SQueryIndexArray& closest_points, const int closest_node_id)
{
	if(closest_node_id == _uuid) {
		// all of the points in this node has been scanned previously
		return;
	}

	if (!_has_sub_node) {
		auto max_it = std::max_element(closest_points.begin(), closest_points.end());
		for (int i = 0; i < (int) _point_ids.size(); ++i) {
			const auto q = GetPointById(_point_ids[i]);
			FloatType sum = 0.;
			for (int j = 0; j < _k; ++j) {
				sum += (q[j] - p[j]) * (q[j] - p[j]);
			}
			if(sum < max_it->_distance) {
				max_it->_distance = sum;
				max_it->_id = _point_ids[i];
				max_it = std::max_element(closest_points.begin(), closest_points.end());
			}
		}
	} else {
		const auto max_R = std::sqrt(std::max_element(closest_points.begin(), closest_points.end())->_distance);
		if (_left_node->HasIntersectWithSphere(p, max_R)) {
			_left_node->RangeNNSearch(p, closest_points, closest_node_id);
		}
		if (_right_node->HasIntersectWithSphere(p, max_R)) {
			_right_node->RangeNNSearch(p, closest_points, closest_node_id);
		}
	}
}

FloatArrayType CKdTreeNode::GetPointById(const int id) {
	FloatArrayType ret(_k);
	for (int i = 0; i < _k; ++i) {
		ret[i] = _points->at(i).at(id);
	}
	return ret;
}

void CKdTreeNode::PrintBBox(void) const {
	LOG_INFO("Node " << _uuid << "---");
	LOG_INFO("Split axis : " << _split << ". Split val = " << _split_val);
	LOG_ARRAY(_bbox._lower, _k);
	LOG_ARRAY(_bbox._upper, _k);
	LOG_INFO("==END==");
}

bool CKdTreeNode::HasIntersectWithSphere(const FloatType* p,
		const FloatType R)
{
	FloatType d = (FloatType) 0;
	for (int i = 0; i < _k; ++i) {
		FloatType dmin = (FloatType) 0;
		if (p[i] < _bbox._lower[i]) {
			dmin = _bbox._lower[i] - p[i];
		} else if(p[i] > _bbox._upper[i]) {
			dmin = p[i] - _bbox._upper[i];
		}
		d += dmin * dmin;
	}
	return (R * R >= d);
}

bool CKdTreeNode::HasInternalSphere(const FloatType* p,
				const FloatType R)
{
	for (int i = 0; i < _k; ++i) {
		if (p[i] - R < _bbox._lower[i]) {
			return false;
		}
		if (p[i] + R >= _bbox._upper[i]) {
			return false;
		}
	}
	return true;
}
} /* namespace NPCA */
