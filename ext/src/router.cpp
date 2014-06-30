#include "router.h"

namespace routeman {

std::string str_replace(const std::string &str, const std::string &search, const std::string &replacement) {
    std::string result = str;
    if (str.size() > 0) {
        const size_t search_size = search.size();

        for (size_t pos = result.find(search);
             pos != std::string::npos;
             pos = result.find(search, pos + 2)) {
            result.replace(pos, search_size, replacement);
        }
    }

    return result;
}

void router::compile_route(route *&route) const {
    std::string path = route->path;
    boost::smatch matches;
    bool matched = false;

    while (boost::regex_search(path, matches, ROUTE_REGEX)) {
        matched = true;
        route->parsed += str_replace(matches.prefix().str(), "/", "\\/");
        std::string type = matches[1].str();
        if (type.size() > 1) {
            route->parsed += "(" + type + ")";
        } else {
            switch (type.c_str()[0]) {
                case 'i':
                    route->parsed += "([0-9]+)";
                    break;
                case 's':
                    route->parsed += "([a-z]+)";
                    break;
                case 'a':
                    route->parsed += "([0-9a-z]+)";
                    break;
                default:
                    route->parsed += "([^/]+)";
                    break;
            }
        }
        route->parameters.push_back(route::parameter(matches[2].str()));
        path = matches.suffix().str();
    }
    if (matched) {
        route->parsed += str_replace(path, "/", "\\/");
    } else {
        route->parsed = path;
    }
}

void router::add(route *route) {
    compile_route(route);
    routes.push_back(route);
}

void router::compile() {
    std::string pattern("^(?:");
    size_t counter = 0;
    bool first = true;
    for (auto &route : routes) {
        const size_t parameters_size = route->parameters.size();

        if (parameters_size == 0) {
            simple_routes[route->path] = route;
        } else {
            if (first) {
                first = false;
            } else {
                pattern += "|";
            }
            pattern += route->parsed;
            for (size_t i = 0; i < counter; ++i) {
                pattern += "()";
            }
            expression_routes[counter] = route;
            counter += parameters_size + 1;
        }
    }
    pattern += ")$";
    expression = boost::regex(pattern);
}

route *router::match(const std::string &url, const std::string &method) {
    if (!compiled) {
        compile();
        compiled = true;
    }
    auto simple_route = simple_routes.find(url);
    if (simple_route != simple_routes.end()) {
        return simple_route->second;
    }

    boost::smatch matches;
    if (boost::regex_search(url, matches, expression)) {
        for (size_t i = 1, size = matches.size(); i < size; ++i) {
            if(matches[i].matched) {
                route *matched = expression_routes.find(i - 1)->second;
                for (auto &parameter : matched->parameters) {
                    parameter.value = matches[i++];
                }
                return matched;
            }
        }
    }
    return nullptr;
}

} // namespace routeman
