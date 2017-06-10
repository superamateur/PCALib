/*
 * Matrix.h
 *
 *  Created on: 2017/06/03
 *      Author: work
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <functional>
#include "PCAlg.h"

namespace NPCA {

class Matrix {
public:
	Matrix();
	Matrix(int rows, int cols);
	Matrix(int rows, int cols, FloatType* data);
	virtual ~Matrix();

	int Rows() const { return _rows; }
	int Cols() const { return _cols; }
	FloatType* Data(const int r = 0, const int c = 0) { return &_data[r][c]; }

	Matrix operator * (const Matrix& other) const;
	Matrix operator + (const Matrix& other) const;
	Matrix operator - (const Matrix& other) const;

	Matrix SubMatrix(const int row_from, const int row_to, const int col_from, const int col_to) const;
	Matrix& Invert(void);

	void Debug(void) const;

	std::vector<FloatType>& operator[](int row) { return _data[row]; }
	const std::vector<FloatType>& operator[](int row) const { return _data[row]; }

private:
	typedef std::function<FloatType(FloatType, FloatType)> BinaryOperator;
	Matrix BinaryOperation(const Matrix& other, const BinaryOperator& bo) const;

	int _rows;
	int _cols;
	std::vector<std::vector<FloatType>> _data;
};

} /* namespace NPCA */

#endif /* MATRIX_H_ */
