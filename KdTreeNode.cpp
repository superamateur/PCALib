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
: _k(0)
, _node_size(0)
, _split(0)
, _split_val(FloatType(0))
, _has_sub_node(false)
{
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
		variations[i] = num_2 - _arr.size() * num_1 * num_1;
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

	_has_sub_node = true;
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

}

std::tuple<int, FloatType, bool> CKdTreeNode::NNSearch(const FloatType* p)
 {
	if (!_has_sub_node) {
		// Find minimum distance from points inside this node
		FloatType min_distance = std::numeric_limits<FloatType>::max();
		int min_distance_id = 0;
		for (const int i : _point_ids) {
			const auto q = GetPointById(i);
			double sum = 0.;
			for (int j = 0; j < _k; ++j) {
				sum += (q[j] - p[j]) * (q[j] - p[j]);
			}
			if (sum < min_distance) {
				min_distance = sum;
				min_distance_id = i;
			}
		}
		min_distance = std::sqrt(min_distance);

		return std::make_tuple(min_distance_id, min_distance,
				HasInternalSphere(p, min_distance));
	}

	int min_distance_id;
	FloatType min_distance;
	bool fully_internal;

	if (p[_split] < _split_val) {
		std::tie(min_distance_id, min_distance, fully_internal) = _left_node->NNSearch(p);
	} else {
		std::tie(min_distance_id, min_distance, fully_internal) = _right_node->NNSearch(p);
	}

	if(!fully_internal) {
		/* NNSearch from sub nodes return a close point, but may not be the closest
		 * So, search inside other nodes that may contain a closer point
		 * */
		if(HasInternalSphere(p, min_distance)) {
			NNSearchRadius(p, min_distance_id, min_distance);
			fully_internal = true;
		}
	}
	return std::make_tuple(min_distance_id, min_distance, fully_internal);
}

void CKdTreeNode::NNSearchRadius(const FloatType* p, int& closest_id, FloatType& min_radius)
{
	if (!_has_sub_node) {
		auto min_radius_2 = min_radius * min_radius;
		for (const int i : _point_ids) {
			const auto q = GetPointById(i);
			double sum = 0.;
			for (int j = 0; j < _k; ++j) {
				sum += (q[j] - p[j]) * (q[j] - p[j]);
			}
			if (sum < min_radius_2) {
				min_radius_2 = sum;
				closest_id = i;
			}
		}
		min_radius = std::sqrt(min_radius_2);
	} else {
		if (_left_node->HasIntersectWithSphere(p, min_radius)) {
			_left_node->NNSearchRadius(p, closest_id, min_radius);
		}
		if (_right_node->HasIntersectWithSphere(p, min_radius)) {
			_right_node->NNSearchRadius(p, closest_id, min_radius);
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
