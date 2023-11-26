/*-----------------*
 | Header Output.h |
 *-----------------*/

#pragma once

#include <iostream>
#include <string>

namespace Experiment
{
	// ����ʹ�� format ��������ĺ���ģ��
	template <typename Elem, typename OTraits, typename STraits, typename... Args>
	inline void basic_print(std::basic_ostream<Elem, OTraits>& output, std::basic_string_view<Elem, STraits> fmt, Args&&... args)
	{
		output << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
	}
	template <typename... Args>
	inline void print(std::string_view fmt, Args&&... args)
	{
		basic_print(std::cout, fmt, std::forward<Args>(args)...);
	}
	inline void println()
	{
		print("\n");
	}
	template <typename... Args>
	inline void println(std::string_view fmt, Args&&... args)
	{
		print(fmt, std::forward<Args>(args)...);
		println();
	}
	// ����ת���ַ���������Ϊ�ַ�����ͼ����������������� (char �汾)
	inline std::string_view operator"" _view(const char* str, std::size_t size)
	{
		return std::string_view{ str, size };
	}
	// ����ת���ַ���������Ϊ�ַ�����ͼ����������������� (wchar_t �汾)
	inline std::wstring_view operator"" _view(const wchar_t* str, std::size_t size)
	{
		return std::wstring_view{ str, size };
	}
}
