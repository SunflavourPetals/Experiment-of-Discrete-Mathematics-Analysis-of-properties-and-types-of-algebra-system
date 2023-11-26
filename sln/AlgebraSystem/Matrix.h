/*-----------------*
 | Header Matrix.h |
 *-----------------*/

#pragma once

#include <vector>

namespace Experiment
{
	template <typename Elem>
	class Matrix
	{
	public:
		struct Index
		{
			std::size_t row{};
			std::size_t col{};
		};
	public:
		// get element
		template <typename Self>
		constexpr auto&& operator() (this Self&& self, std::size_t row_index, std::size_t col_index)
		{
			return std::forward<Self>(self).data[row_index][col_index];
		}
		// get element
		template <typename Self>
		constexpr auto&& operator[] (this Self&& self, Index index)
		{
			return std::forward<Self>(self).operator()(index.row, index.col);
		}
		// get row
		template <typename Self>
		constexpr auto&& operator[] (this Self&& self, std::size_t row_index)
		{
			return std::forward<Self>(self).data[row_index];
		}
		// get iterator
		constexpr auto RowBegin(this auto&& self) noexcept
		{
			return self.data.begin();
		}
		// get iterator
		constexpr auto RowEnd(this auto&& self) noexcept
		{
			return self.data.end();
		}
		// for range-based for
		constexpr auto begin(this auto&& self) noexcept
		{
			return self.RowBegin();
		}
		// for range-based for
		constexpr auto end(this auto&& self) noexcept
		{
			return self.RowEnd();
		}
		// resize
		constexpr void Resize(std::size_t row_size, std::size_t col_size)
		{
			std::vector<Elem> each_row(row_size);
			this->data.resize(col_size, each_row);
		}
		// get element-count of each row
		constexpr std::size_t RowSize(this auto&& self) noexcept
		{
			if (self.RowBegin() == self.RowEnd())
			{
				return 0;
			}
			return self.RowBegin()->size();
		}
		// get element-count of each col
		constexpr std::size_t ColSize(this auto&& self) noexcept
		{
			return self.data.size();
		}
	public:
		constexpr Matrix& operator=(const Matrix&) = default;
		constexpr Matrix& operator=(Matrix&&) noexcept = default;
		constexpr Matrix() = default;
		constexpr Matrix(std::size_t row_size, std::size_t col_size) { this->Resize(row_size, col_size); }
		constexpr Matrix(const Matrix&) = default;
		constexpr Matrix(Matrix&&) noexcept = default;
		constexpr ~Matrix() = default;
	private:
		std::vector<std::vector<Elem>> data;
	};
}
