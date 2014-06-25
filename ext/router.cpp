#include "router.hpp"

namespace routeman {

    void router::compile_route(route *&route) const {
        std::string path = route->path;
        std::string parsed;
        boost::smatch matches;
        bool matched = false;

        while (boost::regex_search(path, matches, ROUTE_REGEX)) {
            matched = true;
            parsed += str_replace(matches.prefix().str(), "/", "\\/");
            std::string type = matches[1].str();
            if (type.size() > 1) {
                parsed += "(" + type + ")";
            } else {
                switch (type.c_str()[0]) {
                    case 'i':
                        parsed += "([0-9]+)";
                        break;
                    case 's':
                        parsed += "([a-z]+)";
                        break;
                    case 'a':
                        parsed += "([0-9a-z]+)";
                        break;
                    default:
                        parsed += "([^/])";
                        break;
                }
            }
            route->parameters.push_back(route::parameter(matches[2].str()));
            path = matches.suffix().str();
        }
        if (matched) {
            route->parsed = parsed + str_replace(path, "/", "\\/");
        } else {
            route->parsed = path;
        }
    }

    void router::add_route(route *route) {
        compile_route(route);
        routes.insert(route);
    }

    void router::compile() {
        std::string pattern("^(?:");
        size_t counter = 0;
        bool first = true;
        for (auto &route : routes) {
            const size_t parameters_size = route->parameters.size();
            if (!first) {
                pattern += "|";
                first = true;
            }
            pattern += route->parsed;
            for (size_t i = 0; i < counter; ++i) {
                pattern += "()";
            }
            expression_routes[counter] = route;
            counter += parameters_size + 1;
        }
        pattern += ")$";
        expression = boost::regex(pattern);
    }

    route *router::match(const std::string &url) {
        if (!compiled) {
            compile();
            compiled = true;
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
}
