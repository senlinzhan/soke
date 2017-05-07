#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <iterator>


// formating string
template<typename ... Args>
std::string string_format(const std::string &format, Args ... args)
{
    int size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    std::string str(size, '\0');
    std::snprintf(&str[0], size, format.c_str(), args ...);
    str.pop_back();
    return str;
}

// return true if elem is in container, otherwise return false
// the container type may be set, unordered_set, map or unordered_map
template <typename Container, typename T>
bool contains(const Container &container, const T &elem)
{
    return container.find(elem) != container.cend();
}


// returns a vector of all the words in the string
// these words are separated by the delimiters
// if return_tokens is true, then we will also return these delimiters in the result
std::vector<std::string> split_tokens(const std::string &str,
				      const std::string &delimiters,
				      bool return_tokens = false);

// remove leading and trailing whitespace characters from the string
std::string &strip(std::string &str, const std::string &spaces = " \t\n");

// return a new string that without the leading and trailing whitespace characters
std::string strip(const std::string &str, const std::string &spaces = " \t\n");

// test whether a string represent for a double value
bool is_double_str(const std::string &str);


// return the sum of the squares of the array
template<typename InputIterator>
typename std::iterator_traits<InputIterator>::value_type
squares_sum(InputIterator first, InputIterator last)
{
    typename std::iterator_traits<InputIterator>::value_type initValue = 0;
    return std::inner_product(first, last, first, initValue);
}

// return the sum of the array
template<typename InputIterator>
typename std::iterator_traits<InputIterator>::value_type
range_sum(InputIterator first, InputIterator last)
{
    typename std::iterator_traits<InputIterator>::value_type initValue = 0;
    initValue = std::accumulate(first, last, initValue);
    return initValue;
}

#endif /* UTILS_H */
