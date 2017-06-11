/*
 * Matrix.cpp
 *
 *  Created on: 2017/06/03
 *      Author: work
 */
#include <cstring>
#include <limits>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "Matrix.h"
#include "Logger.h"

namespace NPCA {

Matrix::Matrix()
: Matrix(0, 0)
{
}

Matrix::Matrix(int rows, int cols)
: _rows(rows), _cols(cols)
{
	_data.resize(rows);
	for(auto& row : _data) {
		row = std::vector<FloatType>(cols, (FloatType)0);
	}
}

Matrix::Matrix(int rows, int cols, FloatType* data)
: Matrix(rows, cols)
{
	for (int i = 0; i < _rows; ++i) {
		std::memmove((void*)_data[i].data(), (const void*)(&data[i * cols]), cols * sizeof(FloatType));
	}
}

Matrix::~Matrix() {
}

Matrix Matrix::operator * (const Matrix& other) const {
	if(_cols != other.Rows()) {
		LOG_ERROR("Invalid Matrix size" << std::endl);
		return Matrix();
	}
	Matrix ret(_rows, other.Cols());
	for(int i = 0; i < _rows; ++i) {
		for(int k = 0; k < _cols; ++k) {
			for(int j = 0; j < other.Cols(); ++j) {
				ret[i][j] += _data[i][k] * other[k][j];
			}
		}
	}
	return ret;
}
Matrix Matrix::operator +(const Matrix& other) const {
	return BinaryOperation(other, std::plus<FloatType>());
}

Matrix Matrix::operator -(const Matrix& other) const {
	return BinaryOperation(other, std::minus<FloatType>());
}

Matrix Matrix::SubMatrix(const int row_from, const int row_to, const int col_from, const int col_to) const
{
	Matrix sub_matrix(row_to - row_from, col_to - col_from);
	for(int i = row_from; i < row_to; ++i) {
		std::memcpy((void*)sub_matrix[i - row_from].data(), (const void*)(&_data[i].data()[col_from]), (col_to - col_from) * sizeof(FloatType));
	}
	return sub_matrix;
}

Matrix& Matrix::Invert(void)
{
	if (_rows != _cols) {
		throw std::logic_error("This Matrix CANNOT Be Inverted");
	}
	// Gauss-Jordan elimination with full pivoting
	auto is_zero = [](FloatType a) { return a < 1.e-6 && a > -1.e-6; };
	int N = _rows;
	// Indices of the diagonal element BEFORE being switched
	std::vector<std::pair<int, int>> pindx(N);
	// This is to indicate whether a column has been reduced
	std::vector<bool> pchecked(N, false);

	// Loop over the column
	for(int c = 0; c < N; ++c) {
		auto max_pivot = (FloatType)0.;
		int pivot_row = -1, pivot_col = -1;
		/*
		 * scan for the maximum element (absolute value).
		 * Note that we are skipping columns that have already been processed (and so as the corresponding rows)
		 */
		for(int i = 0; i < N; ++i) {
			if(!pchecked[i]) { // skip the rows
				for (int j = 0; j < N; ++j) {
					if (!pchecked[j]) { // skip the columns
						if (std::fabs(_data[i][j]) >= max_pivot) {
							max_pivot = std::fabs(_data[i][j]);
							pivot_row = i;
							pivot_col = j;
						}
					}
				}
			}
		}
		// we are going to reduce this column, and the corresponding row. So mark it as pivoted
		pchecked[pivot_col] = true;

		// now, make sure the pivot is on diagonal
		if (pivot_row != pivot_col) {
			_data[pivot_row].swap(_data[pivot_col]);
		}

		if (is_zero(_data[pivot_col][pivot_col])) {
			LOG_ERROR("Rank = " << c);
			throw std::logic_error("This Matrix CANNOT Be Inverted");
		}

		// remember the original location of the pivot
		pindx[c].first  = pivot_row;
		pindx[c].second = pivot_col;

		const FloatType norm_ = FloatType(1) / _data[pivot_col][pivot_col];
		/*
		 * Here is the trick: This algorithm is in-place. Meaning we do not use extra storage for the augmented unit matrix.
		 * So we have to imagine what would happen to the same row in the unit matrix, if we normalize the row containing the pivot
		 * It is rather clear that the diagonal element of that row in the unit matrix is still untouched, and remains 1
		 * Here, we set the pivot element to 1 in advance, so that it will the same as in the augmented matrix after transformation
		 * Question is, how about the others in the same row? Well, they will be taken care of later.
		 */
		_data[pivot_col][pivot_col] = FloatType(1);
		std::transform(_data[pivot_col].begin(), _data[pivot_col].end(), _data[pivot_col].begin(), [&](FloatType a) {return a * norm_;});

		// Next we reduce the rows every [pivot_col] element of each row become 0
		for(int r = 0; r < N; ++r) {
			if(pivot_col != r) { // ofcourse, we do not want to touch the pivot
				const auto coeff_ = _data[r][pivot_col];
				/*
				 * Same trick as above. We copy the corresponding element in advance,
				 * so that it will be come the same after transformation
				 */
				_data[r][pivot_col] = (FloatType)0;
				for(int k = 0; k < N; ++k) {
					_data[r][k] -= _data[pivot_col][k] * coeff_;
				}
			}
		}
	}

	for (int c = N - 1; c >= 0; --c) {
		if (pindx[c].first != pindx[c].second) {
			for (int r = 0; r < N; ++r) {
				std::swap(_data[r][pindx[c].first], _data[r][pindx[c].second]);
			}
		}
	}
	return *this;
}

Matrix Matrix::BinaryOperation(const Matrix& other, const BinaryOperator& bo) const
{
	if (_rows != other.Rows() || _cols != other.Cols()) {
		LOG_ERROR("Invalid Matrix size" << std::endl);
		return Matrix();
	}
	Matrix ret(_rows, _cols);
#pragma omp parallel for
	for (int i = 0; i < _rows; ++i) {
		for (int j = 0; j < _cols; ++j) {
			ret[i][j] = bo(_data[i][j], other[i][j]);
		}
	}

	return ret;
}

void Matrix::Debug(void) const {
	LOG_INFO("Matrix " << _rows << "x" << _cols << std::endl);
	for (int i = 0; i < _rows; ++i) {
		LOG_ARRAY(_data[i], _data[i].size());
	}
}

} /* namespace NPCA */
