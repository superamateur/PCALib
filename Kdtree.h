/*
 * Kdtree.h
 *
 *  Created on: 2017/05/20
 *      Author: work
 */
#include <vector>
#include <tuple>
#include "KdTreeNode.h"

#ifndef KDTREE_H_
#define KDTREE_H_

namespace NPCA {

class CKdtree {
public:
	CKdtree();
	CKdtree(int k);
	CKdtree(int k, int a);
	virtual ~CKdtree();
	int InsertNextPoints(const FloatType* p, int num);
	void BuildTree();
	std::vector<int> kNNSearch(const FloatType* p, int num);
	std::vector<int> rNNSearch(const FloatType* p, const FloatType R);
	void Debug() const;

private:
	int _k;
	int _atomic_num;
	FloatMultiArrayTypePtr _points; // 2D array: [k * num]
	CKdTreeNodePtr _root;
};
} /* namespace NPCA */

#endif /* KDTREE_H_ */
