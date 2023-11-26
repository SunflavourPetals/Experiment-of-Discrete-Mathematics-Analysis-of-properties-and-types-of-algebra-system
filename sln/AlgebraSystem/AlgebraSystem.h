/*------------------------*
 | Header AlgebraSystem.h |
 *------------------------*/

#pragma once

#include <optional>

#include "Matrix.h"
#include "Output.h"

namespace Experiment
{
	// ������ SetType ��Ҫ��������: ������0Ϊʼ������������Ԫ��
	template <typename Elem, typename SetType = std::vector<Elem>, typename MatrixType = Matrix<Elem>>
	class AlgebraSystem
	{
	public:
		using Set = SetType;
		using Matrix = MatrixType;
		struct InverseElements final
		{
			Set left_inv_elems;
			Set right_inv_elems;
		};
	public:
		constexpr const auto& operator() (this auto&& self, std::size_t matrix_row_index, std::size_t matrix_col_index)
		{
			return self.matrix(matrix_row_index, matrix_col_index);
		}
		constexpr const auto& operator[] (this auto&& self, std::size_t set_elem_index)
		{
			return self.set[set_elem_index];
		}
		constexpr std::size_t Size(this auto&& self) noexcept
		{
			return self.set.size();
		}
		template <typename InputElem, typename InputTraits>
		void InputNewSize(std::basic_istream<InputElem, InputTraits>& input)
		{
			typename SetType::size_type new_size{};
			if ((input >> new_size).fail())
			{
				throw std::exception{ "Failed in input new_size" };
			}
			// reset Set and Matrix and state...
			this->set.resize(new_size);
			this->matrix.Resize(new_size, new_size);
			this->is_closure = std::nullopt;
			this->is_commutable = std::nullopt;
			this->is_associative = std::nullopt;
			this->is_idempotent = std::nullopt;
			this->is_groupoid = std::nullopt;
			this->is_semigroup = std::nullopt;
			this->is_monoid = std::nullopt;
			this->is_group = std::nullopt;
			this->identity_element = std::nullopt;
			this->zero_element = std::nullopt;
			this->inverse_elements = std::nullopt;
		}
		template <typename InputElem, typename InputTraits>
		void InputSet(std::basic_istream<InputElem, InputTraits>& input)
		{
			for (auto&& elem : this->set)
			{
				if ((input >> elem).fail())
				{
					throw std::exception{ "Failed in input set" };
				}
			}
		}
		template <typename InputElem, typename InputTraits>
		void InputMatrix(std::basic_istream<InputElem, InputTraits>& input)
		{
			for (auto&& row : this->matrix)
			{
				for (auto&& elem : row)
				{
					if ((input >> elem).fail())
					{
						throw std::exception{ "Failed in input matrix" };
					}
				}
			}
		}
		template <typename OElem, typename OTraits, typename STraits>
		void OutputSet(std::basic_ostream<OElem, OTraits>& output, std::basic_string_view<OElem, STraits> fmt) const
		{
			for (const auto& elem : this->set)
			{
				basic_print(output, fmt, elem);
			}
		}
		template <typename OElem, typename OTraits, typename STraits>
		void OutputMatrix(
			std::basic_ostream<OElem, OTraits>& output,
			std::basic_string_view<OElem, STraits> fmt,
			std::basic_string_view<OElem, STraits> row_separator) const
		{
			for (std::size_t index{ 1 }; const auto & row : this->matrix)
			{
				for (const auto& elem : row)
				{
					basic_print(output, fmt, elem);
				}
				index++ >= this->matrix.RowSize() ? 0 : (basic_print(output, row_separator), 0);
			}
		}
	public:
		// �õ��Ƿ�Ϊ���з���ԵĽ��
		bool IsClosure(this auto&& self)
		{
			return self.is_closure.has_value() ? self.is_closure.value() : (self.is_closure = self.CheckClosure()).value();
		}
		// �����Ƿ�Ϊ���з����
		bool CheckClosure(this auto&& self)
		{
			// for any result �� matrix.elements, result �� S => true
			for (const auto& row : self.matrix)
			{
				for (const auto& elem : row)
				{
					if (std::find(self.set.begin(), self.set.end(), elem) == self.set.end()) return false;
				}
			}
			return true;
		}
		// �õ��Ƿ�Ϊ���н����ԵĽ��
		bool IsCommutable(this auto&& self)
		{
			return self.is_commutable.has_value() ? self.is_commutable.value() : (self.is_commutable = self.CheckCommutable()).value();
		}
		// �����Ƿ���н�����
		bool CheckCommutable(this auto&& self)
		{
			// for any A, B �� S, A*B == B*A => true
			const std::size_t size{ self.set.size() };
			const std::size_t row_max{ size - 1 };
			for (std::size_t ai{}; ai < row_max; ++ai)
			{
				for (std::size_t bi = ai + 1; bi < size; ++bi)
				{
					// get the results of A*B and B*A
					const auto& AopB = self.matrix(ai, bi);
					const auto& BopA = self.matrix(bi, ai);
					if (AopB != BopA) return false;
				}
			}
			return true;
		}
		// �õ��Ƿ�Ϊ���н���ԵĽ��
		bool IsAssociative(this auto&& self)
		{
			return self.is_associative.has_value() ? self.is_associative.value() : (self.is_associative = self.CheckAssociative()).value();
		}
		// �����Ƿ���н����
		bool CheckAssociative(this auto&& self)
		{
			// if is not closure then is not associative
			if (self.IsClosure() == false) return false;
			// for any A, B, C �� S, (A*B)*C == A*(B*C) => true
			const std::size_t size{ self.set.size() };
			for (std::size_t ai{}; ai < size; ++ai)
			{
				for (std::size_t bi{}; bi < size; ++bi)
				{
					for (std::size_t ci{}; ci < size; ++ci)
					{
						// get result of A*B and B*C
						const auto& AopB = self.matrix(ai, bi);
						const auto& BopC = self.matrix(bi, ci);
						// find the results in S
						auto AopBit = std::find(self.set.begin(), self.set.end(), AopB); // it => iterator
						auto BopCit = std::find(self.set.begin(), self.set.end(), BopC);
						// case operation undefined when A*B or B*C is not in ths matrix
						if (AopBit == self.set.end()) return false;
						if (BopCit == self.set.end()) return false;
						// get the indices of A*B and B*C
						std::size_t AopBi = std::distance(self.set.begin(), AopBit); // * index never less than 0
						std::size_t BopCi = std::distance(self.set.begin(), BopCit);
						// get result of (A*B)*C and A*(B*C)
						const auto& PAopBPopC = self.matrix(AopBi, ci); // P => parentheses
						const auto& AopPBopCP = self.matrix(ai, BopCi);
						// compare
						if (PAopBPopC != AopPBopCP) return false;
					}
				}
			}
			return true;
		}
		// �õ��Ƿ�Ϊ���е����ԵĽ��
		bool IsIdempotent(this auto&& self)
		{
			return self.is_idempotent.has_value() ? self.is_idempotent.value() : (self.is_idempotent = self.CheckIdempotent()).value();
		}
		// �����Ƿ���е�����
		bool CheckIdempotent(this auto&& self)
		{
			// for any A �� S, A*A == A => true
			for (std::size_t ai{}; const auto& elem : self.set)
			{
				if (elem != self.matrix(ai, ai)) return false;
				++ai;
			}
			return true;
		}
		// �����Ԫ
		const auto& GetIdentity(this auto&& self)
		{
			return self.identity_element.has_value() ? self.identity_element.value() : (self.identity_element = self.FindIdentity()).value();
		}
		// Ѱ����Ԫ
		auto FindIdentity(this auto&& self) -> std::optional<Elem>
		{
			// A, I �� S, A*I == A and I*A == A => I is identity element
			const std::size_t size{ self.set.size() };
			for (std::size_t index{}; index < size; ++index)
			{
				bool is_identity{ true };
				for (std::size_t col_index{}; col_index < size; ++col_index)
				{
					// if not left identity element then skip
					if (self.set[col_index] != self.matrix(index, col_index))
					{
						is_identity = false;
						goto out_of_check;
					}
				}
				for (std::size_t row_index{}; row_index < size; ++row_index)
				{
					// if not right identity element then skip
					if (self.set[row_index] != self.matrix(row_index, index))
					{
						is_identity = false;
						goto out_of_check;
					}
				}
			out_of_check:
				if (is_identity) return self.set[index];
				continue;
			}
			return std::nullopt;
		}
		// �����Ԫ
		const auto& GetZero(this auto&& self)
		{
			return self.zero_element.has_value() ? self.zero_element.value() : (self.zero_element = self.FindZero()).value();
		}
		// Ѱ����Ԫ
		auto FindZero(this auto&& self) -> std::optional<Elem>
		{
			// A, Z �� S, A*Z == Z and Z*A == Z => Z is zero element
			const std::size_t size{ self.set.size() };
			for (std::size_t index{}; index < size; ++index)
			{
				const auto& elem = self.set[index]; // * �Ż�ʹ���±�Ѱַ�Ĳ���
				bool is_zero{ true };
				for (std::size_t col_index{}; col_index < size; ++col_index)
				{
					// if not left zero element then skip
					if (elem != self.matrix(index, col_index))
					{
						is_zero = false;
						goto out_of_check;
					}
				}
				for (std::size_t row_index{}; row_index < size; ++row_index)
				{
					// if not right zero element then skip
					if (elem != self.matrix(row_index, index))
					{
						is_zero = false;
						goto out_of_check;
					}
				}
			out_of_check:
				if (is_zero) return elem;
				continue;
			}
			return std::nullopt;
		}
		// �����Ԫ
		const auto& GetInverse(this auto&& self)
		{
			return self.inverse_elements.has_value() ? self.inverse_elements.value() : (self.inverse_elements = self.FindInverse()).value();
		}
		// Ѱ����Ԫ
		auto FindInverse(this auto&& self) -> std::vector<InverseElements>
		{
			// if identity element does not exist then inverse element does not exist
			std::vector<InverseElements> inverse_elements{ self.set.size() };
			if (self.GetIdentity().has_value() == false) return inverse_elements;
			
			const auto& identity_element{ *self.GetIdentity() };
			const std::size_t size{ self.set.size() };
			for (std::size_t ai{}; ai < size; ++ai)
			{
				for (std::size_t bi{}; bi < size; ++bi)
				{
					if (self.matrix(ai, bi) == identity_element)
					{
						// if AopB == identity_element => B is A's right reverse element and A is B's left reverse element
						inverse_elements[ai].right_inv_elems.push_back(self.set[bi]);
						inverse_elements[bi].left_inv_elems.push_back(self.set[ai]);
					}
				}
			}
			return inverse_elements;
		}
		// �õ��Ƿ�Ϊ��Ⱥ�Ľ��
		bool IsGroupoid(this auto&& self)
		{
			return self.is_groupoid.has_value() ? self.is_groupoid.value() : (self.is_groupoid = self.CheckGroupoid()).value();
		}
		// �ж��Ƿ�Ϊ��Ⱥ
		bool CheckGroupoid(this auto&& self)
		{
			return self.IsClosure();
		}
		// �õ��Ƿ�Ϊ��Ⱥ�Ľ��
		bool IsSemigroup(this auto&& self)
		{
			return self.is_semigroup.has_value() ? self.is_semigroup.value() : (self.is_semigroup = self.CheckSemigroup()).value();
		}
		// �ж��Ƿ�Ϊ��Ⱥ
		bool CheckSemigroup(this auto&& self)
		{
			return self.IsGroupoid() && self.IsAssociative();
		}
		// �õ��Ƿ�Ϊ���۰�Ⱥ�Ľ��
		bool IsMonoid(this auto&& self)
		{
			return self.is_monoid.has_value() ? self.is_monoid.value() : (self.is_monoid = self.CheckMonoid()).value();
		}
		// �ж��Ƿ�Ϊ���۰�Ⱥ
		bool CheckMonoid(this auto&& self)
		{
			return self.IsSemigroup() && self.GetIdentity().has_value();
		}
		// �õ��Ƿ�ΪȺ�Ľ��
		bool IsGroup(this auto&& self)
		{
			return self.is_group.has_value() ? self.is_group.value() : (self.is_group = self.CheckGroup()).value();
		}
		// �ж��Ƿ�ΪȺ
		bool CheckGroup(this auto&& self)
		{
			// if is not monoid then is not group
			if (self.IsMonoid() == false) return false;
			// if has zero element then is not group
			if (self.GetZero().has_value() == true) return false;
			// if every element has one and only one identity element then is group
			return self.IsMonoid() && [](auto&& self)
			{
				for (const auto& inv : self.GetInverse())
				{
					if (inv.left_inv_elems.size() != 1) return false;
					if (inv.right_inv_elems.size() != 1) return false;
					if (*inv.left_inv_elems.begin() != *inv.right_inv_elems.begin()) return false;
				}
				return true;
			}(self);
		}
	public:
		AlgebraSystem() = default;
	private:
		Set set{}; // ���� S
		Matrix matrix{}; // �������� Matrix
		// ���³�Ա�������ڻ���������ʵȵ�����������������ظ���ֵ
		mutable std::optional<bool> is_closure;
		mutable std::optional<bool> is_commutable;
		mutable std::optional<bool> is_associative;
		mutable std::optional<bool> is_idempotent;
		mutable std::optional<bool> is_groupoid;
		mutable std::optional<bool> is_semigroup;
		mutable std::optional<bool> is_monoid;
		mutable std::optional<bool> is_group;
		mutable std::optional<std::optional<Elem>> identity_element;
		mutable std::optional<std::optional<Elem>> zero_element;
		mutable std::optional<std::vector<InverseElements>> inverse_elements;
	};

	template <typename Elem>
	int run() noexcept try
	{
		AlgebraSystem<Elem> as;

		println("���뼯�� S �е�Ԫ������: ");
		as.InputNewSize(std::cin);
		println();

		println("���뼯�� S �е�ÿ��Ԫ��(�� {} ��): ", as.Size());
		as.InputSet(std::cin);
		println("������� S: ");
		println("", (as.OutputSet(std::cout, "{:<3} "_view), println(), 0));

		println("���� {0}��{0} ��С����������: ", as.Size());
		as.InputMatrix(std::cin);
		println("�����������: ");
		println("", (as.OutputMatrix(std::cout, "{:<3} "_view, "\n"_view), println(), 0));
		
		println("*** ��������: ");

		println("�Ƿ���з����: {}", as.IsClosure());
		println("�Ƿ���н�����: {}", as.IsCommutable());
		println("�Ƿ���н����: {}", as.IsAssociative());
		println("�Ƿ���е�����: {}", as.IsIdempotent());
		println();

		println("*** ����Ԫ: ");

		print("�Ƿ������Ԫ: {}", as.GetIdentity().has_value());
		println("", as.GetIdentity().has_value() ? (print(", ��Ԫ�� {}", *as.GetIdentity()), 0) : 0);
		print("�Ƿ������Ԫ: {}", as.GetZero().has_value());
		println("", as.GetZero().has_value() ? (print(", ��Ԫ�� {}", *as.GetZero()), 0) : 0);
		for (std::size_t index{}; index < as.Size(); ++index)
		{
			const auto& left_inv{ as.GetInverse()[index].left_inv_elems };
			const auto& right_inv{ as.GetInverse()[index].right_inv_elems };
			print("Ԫ�� {:<4}:", as[index]);
			print("{}��������Ԫ", left_inv.size() > 0 ? "" : "��");
			for (const auto& inv : left_inv) { print(" {}", inv); }
			print(", ");
			print("{}��������Ԫ", right_inv.size() > 0 ? "" : "��");
			for (const auto& inv : right_inv) { print(" {}", inv); }
			println();
		}
		println();

		println("*** Ⱥ�����ж�: ");
		println("�Ƿ��ǹ�Ⱥ: {}", as.IsGroupoid());
		println("�Ƿ��ǰ�Ⱥ: {}", as.IsSemigroup());
		println("�Ƿ��Ǻ��۰�Ⱥ: {}", as.IsMonoid());
		println("�Ƿ���Ⱥ: {}", as.IsGroup());
		println();

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << "exception: \n" << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cout << "unknown exception" << std::endl;
		return 2;
	}
}
