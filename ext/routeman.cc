#include "php_routeman.h"
#include "router.hpp"

inline char *str_to_char(const std::string &str) {
    const unsigned size = str.size();
    char *result = new char [size + 1];
    std::strcpy (result, str.c_str());
    return result;
}

zend_object_handlers routeman_router_object_handlers;

struct routeman_router_object {
    zend_object std;
    routeman::router *router;
};

zend_object_value routeman_router_create_handler(zend_class_entry *class_type TSRMLS_DC)
{
    zend_object_value retval;
    routeman_router_object *obj = (routeman_router_object *)emalloc(sizeof(routeman_router_object));
    zend_object_std_init((zend_object *) obj, class_type TSRMLS_CC);

#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->std.properties, &(class_type->default_properties), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
#else
    object_properties_init(&obj->std, class_type);
#endif

    retval.handle = zend_objects_store_put(obj,
        (zend_objects_store_dtor_t) zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) zend_objects_free_object_storage,
        NULL TSRMLS_CC
    );
    retval.handlers = &routeman_router_object_handlers;

    return retval;
}

zend_class_entry *routeman_router_ce;

PHP_METHOD(Routeman_Router, __construct)
{
    routeman_router_object *obj = (routeman_router_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    obj->router = new routeman::router();
}

/**
 * @param string $name
 * @param string $path
 */
PHP_METHOD(Routeman_Router, add)
{
    char *name, *path;
    int len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len, &path, &len) == FAILURE) {
        RETURN_NULL();
    }
    routeman_router_object *obj = (routeman_router_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    obj->router->add_route(new routeman::route(name, path));
    RETURN_NULL();
}

PHP_METHOD(Routeman_Router, match)
{
    char *url;
    int len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &url, &len) == FAILURE) {
        RETURN_NULL();
    }
    routeman_router_object *obj = (routeman_router_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
        auto route = obj->router->match(url);
    if (route) {
        zval *parameters;
        ALLOC_INIT_ZVAL(parameters);
        array_init(parameters);
        for (auto &parameter : route->parameters) {
            add_assoc_string(parameters, parameter.name.c_str(), str_to_char(parameter.value), 1);
        }

        array_init(return_value);
        add_assoc_string(return_value, "name", str_to_char(route->name), 1);
        add_assoc_zval(return_value, "parameters", parameters);
    } else {
        RETURN_NULL();
    }
}

zend_function_entry routeman_methods[] = {
    PHP_ME(Routeman_Router, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Routeman_Router, add,         NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Routeman_Router, match,       NULL, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(routeman)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Routeman\\Router", routeman_methods);
    routeman_router_ce = zend_register_internal_class(&ce TSRMLS_CC);
    routeman_router_ce->create_object = routeman_router_create_handler;
    memcpy(&routeman_router_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    routeman_router_object_handlers.clone_obj = NULL;

    return SUCCESS;
}

zend_module_entry routeman_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_ROUTEMAN_EXTNAME,
    NULL,                  /* Functions */
    PHP_MINIT(routeman),
    NULL,                  /* MSHUTDOWN */
    NULL,                  /* RINIT */
    NULL,                  /* RSHUTDOWN */
    NULL,                  /* MINFO */
    PHP_ROUTEMAN_EXTVER,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ROUTEMAN
extern "C" {
ZEND_GET_MODULE(routeman)
}
#endif
