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
	int NNSearch(const FloatType* p);
	void Debug() const;

private:
	int _k;
	int _atomic_num;
	FloatMultiArrayTypePtr _points; // 2D array: [k * num]
	CKdTreeNodePtr _root;
};
} /* namespace NPCA */

#endif /* KDTREE_H_ */
