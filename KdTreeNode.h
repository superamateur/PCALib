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

namespace NPCA {
typedef float FloatType;
typedef std::vector<FloatType> FloatArrayType;
typedef std::vector<FloatArrayType> FloatMultiArrayType;
typedef std::shared_ptr<FloatMultiArrayType> FloatMultiArrayTypePtr;

struct SBbox {
	FloatArrayType _lower;
	FloatArrayType _upper;
};
class CKdTreeNode {
public:
	CKdTreeNode();
	CKdTreeNode(int k, int n, FloatMultiArrayTypePtr p,
			const std::vector<int>& i, const SBbox& b);
	virtual ~CKdTreeNode();
	void Build(void);
	std::tuple<int, FloatType, bool> NNSearch(const FloatType* p);
	void NNSearchRadius(const FloatType* p, int& closest_id, FloatType& min_radius);
	FloatArrayType GetPointById(const int id);
	bool HasIntersectWithSphere(const FloatType*  p,
			const FloatType R);
	bool HasInternalSphere(const FloatType* p,
				const FloatType R);

private:
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
typedef std::weak_ptr<CKdTreeNode> CKdTreeNodeWkPtr;

} /* namespace NPCA */

#endif /* KDTREENODE_H_ */
