#include "utils.hpp"

bool    isDigitStr(std::string str) {
    size_t    i = 0, ie = str.size();

    while (i < ie && isdigit(str[i]))
        i++;

    return (i == ie);
}