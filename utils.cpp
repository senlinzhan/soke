#include <cstdio>
#include <unordered_set>
#include <algorithm>
#include "utils.hpp"

using std::string;
using std::vector;
using std::unordered_set;

vector<string> split_tokens(const string &str, const string &delimiters, bool return_tokens)
{
    vector<string> tokens;
    const unordered_set<char> delims(delimiters.cbegin(), delimiters.cend());
    
    auto is_delimiter = [&delims](char c) {
        return contains(delims, c);
    };

    auto first = str.cbegin();
    decltype(first) last;
    
    while ((last = find_if(first, str.cend(), is_delimiter)) != str.cend())
    {
        // store the non-empty token
        if (first != last)
	{
	    tokens.push_back(string(first, last));	    
	}
	
	while (last != str.cend() && is_delimiter(*last)) 
	{
	    // store the token if we need
	    if (return_tokens)
	    {
	        tokens.push_back(string(last, last + 1));
	    }
	    ++last;
	}
	first = last;	
    }
    
    if (first != last) 
    {
        tokens.push_back(string(first, last));
    }
    
    return tokens;
}



std::string &strip(std::string &str, const std::string &spaces)
{
    const unordered_set<char> white_spaces(spaces.cbegin(), spaces.cend());

    auto not_spaces = [&white_spaces](char c) {
        return !contains(white_spaces, c);
    };
    
    str.erase(str.begin(), find_if(str.begin(), str.end(), not_spaces));
    str.erase(find_if(str.rbegin(), str.rend(), not_spaces).base(), str.end());
    return str;
}

std::string strip(const std::string &str, const std::string &spaces)
{
    auto other = str;
    strip(other);
    return other;
}

bool is_double_str(const std::string &str)
{
    bool is_double = true;
    try {
	std::stod(str);
    } catch (const std::invalid_argument &e) {
	is_double = false;
    }
    return is_double;
}

double mean_value(const std::vector<double> &v) 
{
    return range_sum(v.cbegin(), v.cend()) / v.size();
}

double standard_deviation(const std::vector<double> &v)    
{
    double mean = mean_value(v);
    return std::accumulate(v.cbegin(), v.cend(), 0.0,
			   [&mean, &v] (double init, double elem) {
			       return  init + std::pow((elem - mean), 2) / v.size();
			   });
}

double median(const std::vector<double> &v)
{
    if (!std::is_sorted(v.cbegin(), v.cend()))
    {
	throw std::invalid_argument("the array is not sorted");
    }
    
    if (v.empty()) {
	return 0.0;
    }
    auto mid = v.size() / 2;
    if (v.size() % 2 == 0)
    {
	return (v[mid - 1] + v[mid]) / 2;	
    }
    return v[mid];    
}

