#ifndef ROUTEMAN_ROUTER_HPP
#define ROUTEMAN_ROUTER_HPP

#include <string>
#include <set>
#include <map>
#include <boost/regex.hpp>
#include "utils.hpp"

namespace routeman {

    struct route {
        struct parameter {
            std::string name;
            std::string value;
            parameter(const std::string &name) : name(name) {

            }
        };
        std::string name;
        std::string path;
        std::string host;
        std::string parsed;
        std::vector<parameter> parameters;
        bool is_secure = false;
        route(const std::string &name, const std::string &path) : path(path), name(name) {

        }
        bool operator ==(const route &right) {
            return name == right.name;
        }
        bool operator <(const route &right) {
            return name < right.name;
        }
    };

    class router {
    private:
        const boost::regex ROUTE_REGEX = boost::regex("\\{(?:([^{}]+):)*([a-zA-Z][a-zA-Z0-9_]*)\\}");
        std::set<route*> routes;
        std::map<size_t, route*> expression_routes;
        boost::regex expression;
        bool compiled = false;
    protected:
        void compile_route(route *&route) const;
        void compile();
    public:
        void add_route(route *route);
        route *match(const std::string &url);
    };
}

#endif
