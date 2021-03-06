//
//

#include "StringUtils.h"
#include <algorithm>
#include <type_traits>

namespace riner {

    ParsePoolAddressResult parsePoolAddress(const char *cstr) {
        ParsePoolAddressResult res;
        auto npos = std::string::npos;

        std::string str{cstr};

        auto colonPos = str.find(':');
        res.host = str.substr(0, colonPos);

        if (colonPos == npos && (colonPos != str.size() - 1)) {
            res.port = str.substr(colonPos+1);
        }
        return res;
    }

    bool startsWith(const std::string &str, const std::string &prefix) {
        return str.find(prefix) == 0;
    }

    std::string stripDirFromPath(const std::string &path) {
        return path.substr(0, path.find_last_of("\\/"));
    }

    std::string partBefore(const std::string &delim, const std::string &str) {
        return str.substr(0, str.find(delim));
    }

    std::string concatPath(const std::string& a, const std::string& b) {
        //s1 starts with /
        if (!b.empty() && b.front() == '/') {
            throw std::invalid_argument("concatPath(a, b): value of b '" + b + "' cannot start with a slash");
            //return b; //or treat as root path?
        }
        //s0 does not end with /
        else if (!a.empty() && a.back() != '/') {
            return a + '/' + b;
        }
        //s0 empty or s0 ends with / and s1 doesn't
        return a + b;
    }

    optional<int64_t> strToInt64(const char *str) {
        try {
            return std::stoll(str);
        }
        catch (const std::invalid_argument &) {}
        catch (const std::out_of_range &) {}
        return nullopt;
    }

    std::string toLower(const std::string &inStr) {
        std::string str = inStr;
        std::transform(str.begin(), str.end(), str.begin(),
                       [] (char c) { return static_cast<char>(std::tolower(c)); });
        return str;
    }

}