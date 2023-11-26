/*-----------------*
 | Source main.cpp |
 *-----------------*/

#include "AlgebraSystem.h"

int main()
{
#ifdef _MSC_VER
	std::cout << std::format("msvc version: {}\n", _MSC_VER) << std::endl;
#endif
	return Experiment::run<char>();
}
