#ifndef ROUTEMAN_ROUTER_H
#define ROUTEMAN_ROUTER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <boost/regex.hpp>

namespace routeman {

std::string str_replace(const std::string &str, const std::string &search, const std::string &replacement);

struct route {
    enum methods {
        GET    = 1 << 1,
        POST   = 1 << 2,
        PUT    = 1 << 3,
        DELETE = 1 << 4,
        ANY    = GET | POST | PUT | DELETE
    };
    struct parameter {
        std::string name;
        std::string value;
        parameter(const std::string &name) : name(name) {}
    };
    unsigned method;
    std::string name;
    std::string path;
    std::string host;
    std::string parsed;
    std::vector<parameter> parameters;
    bool is_secure;
    route(const std::string &name, const std::string &path, const std::string &method = "ANY",
          const std::string &host = "", const bool &is_secure = false)
    : name(name), path(path), host(host), is_secure(is_secure) {
        route::method = str_to_method(method);
    }
    static unsigned str_to_method(const std::string &method) {
        if (method == "GET") {
            return GET;
        } else if (method == "POST") {
            return POST;
        } else if (method == "PUT") {
            return PUT;
        } else if (method == "DELETE") {
            return DELETE;
        } else {
            return ANY;
        }
    }
    static std::string method_to_str(const unsigned &method) {
        switch(method) {
            case GET:    return "GET";
            case POST:   return "POST";
            case PUT:    return "PUT";
            case DELETE: return "DELETE";
            default:     return "ANY";
        }
    }
};

class router {
private:
    const boost::regex ROUTE_REGEX = boost::regex("\\{(?:([^{}]+):)*([a-zA-Z][a-zA-Z0-9_]*)\\}");
    std::list<route*> routes;
    std::unordered_map<size_t, route*> expression_routes;
    std::unordered_map<std::string, route*> simple_routes;
    boost::regex expression;
    bool compiled = false;
protected:
    void compile_route(route *&route) const;
    void compile();
public:
    void add(route *route);
    route *match(const std::string &url, const std::string &method);
};

} // namespace routeman

#endif /* defined ROUTEMAN_ROUTER_H */
