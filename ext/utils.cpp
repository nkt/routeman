#include "utils.hpp"

namespace routeman {

    std::string str_replace(const std::string &str, const std::string &search, const std::string &replacement) {
        if (str.size() == 0) {
            return str;
        }

        std::string result = str;
        const size_t search_size = search.size();

        for (size_t pos = result.find(search); pos != std::string::npos; pos = result.find(search, pos + 2)) {
            result.replace(pos, search_size, replacement);
        }

        return result;
    }

}
