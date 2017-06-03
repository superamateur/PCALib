/*
 * KdTreeNode.h
 *
 *  Created on: 2017/05/25
 *      Author: work
 */

#ifndef KDTREENODE_H_
#define KDTREENODE_H_
#include <memory>
#include <vector>
#include <tuple>

#include "PCAlg.h"

namespace NPCA {
struct SQueryIndex {
	FloatType _distance;
	int _id;
	bool operator < (const SQueryIndex& other) const { return _distance < other._distance; }
};
typedef std::vector<SQueryIndex> SQueryIndexArray;

class CKdTreeNode {
public:
	CKdTreeNode();
	CKdTreeNode(int k, int n, FloatMultiArrayTypePtr p,
			const std::vector<int>& i, const SBbox& b);
	virtual ~CKdTreeNode();
	void Build(void);

	std::tuple<SQueryIndexArray, bool, int> NNSearch(const FloatType* p, const int num);
	std::vector<int> rNNSearch(const FloatType* p, const FloatType R);

	FloatArrayType GetPointById(const int id);
	int ID(void) const { return _uuid; }
	int Rank(void) const { return _rank; }
	SBbox& BBox(void) { return _bbox; }
	void PrintBBox(void) const;

private:
	void RangeNNSearch(const FloatType* p, SQueryIndexArray& closest_points,
			const int closest_node_id);
	bool HasIntersectWithSphere(const FloatType* p, const FloatType R);
	bool HasInternalSphere(const FloatType* p, const FloatType R);

private:
	int _uuid;
	int _rank;
	int _k;
	int _node_size;
	int _split;
	FloatType _split_val;
	bool _has_sub_node;
	SBbox _bbox;

	FloatMultiArrayTypePtr _points;
	std::vector<int> _point_ids;
	std::shared_ptr<CKdTreeNode> _left_node;
	std::shared_ptr<CKdTreeNode> _right_node;
};

typedef std::shared_ptr<CKdTreeNode> CKdTreeNodePtr;

} /* namespace NPCA */

#endif /* KDTREENODE_H_ */
