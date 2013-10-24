#ifndef STACKOVERFLOWQ00_HPP
#define STACKOVERFLOWQ00_HPP

void test_spirit_qi(int iteration);
void test_strcpy_s(int iteration);

inline void test_so_00()
{
    int const ITERATIONS = 500 * 10000;
    test_strcpy_s(ITERATIONS);
    test_spirit_qi(ITERATIONS);
}

#endif // STACKOVERFLOWQ00_HPP
