/*
 * PCAlg.h
 *
 *  Created on: 2017/06/03
 *      Author: work
 */

#ifndef PCALG_H_
#define PCALG_H_

#include <vector>
#include <memory>

namespace NPCA {
typedef float FloatType;
typedef std::vector<FloatType> FloatArrayType;
typedef std::vector<FloatArrayType> FloatMultiArrayType;
typedef std::shared_ptr<FloatMultiArrayType> FloatMultiArrayTypePtr;

struct SBbox {
	FloatArrayType _lower;
	FloatArrayType _upper;
};
} // namespace NPCA



#endif /* PCALG_H_ */
