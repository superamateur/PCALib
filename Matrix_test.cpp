/*
 * Matrix_test.cpp
 *
 *  Created on: 2017/06/04
 *      Author: work
 */

#include "Matrix.h"
#include "Logger.h"

namespace NPCA {
bool test_matrix_multiplication(void) {
	FloatArrayType _d {
		  7, 8, 11, 13, 2
		, 14, 9, 15, 5, 4
		, 10, 12, 6, 16, 17
		, 18, 19, 20, 3, 21
		, 22, 23, 24, 25, 26
	};

	Matrix D(5, 5, _d.data());
	Matrix D_inv(5, 5, _d.data());
	D_inv.Invert();
	auto one = D * D_inv;
	D_inv.Debug();
	return true;
}
}


