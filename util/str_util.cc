#include "util/str_util.h"

std::vector<std::string> split(const std::string &str, const char *delimeters)
{
    std::vector<std::string> results;
    int pos = 0;
    while(pos != std::string::npos)
    {
        int next = str.find_first_of(delimeters, pos);
        if(next == std::string::npos)
        {
            results.push_back(str.substr(pos));
            return results;
        }

        results.push_back(str.substr(pos, next - pos));
        pos = next + 1;
    }

    return results;
}

