#include "php_routeman.h"
#include "src/router.h"

zend_object_handlers routeman_router_object_handlers;

struct routeman_router_object {
    zend_object std;
    routeman::router *router;
};

void routeman_router_free_storage(void *object TSRMLS_DC)
{
    routeman_router_object *obj = (routeman_router_object *)object;
    delete obj->router;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);

    efree(obj);
}

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
        routeman_router_free_storage,
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

PHP_METHOD(Routeman_Router, handle)
{
    RETURN_NULL();
}

zend_function_entry routeman_methods[] = {
    PHP_ME(Routeman_Router, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Routeman_Router, handle,      NULL, ZEND_ACC_PUBLIC)
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
