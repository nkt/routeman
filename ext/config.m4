PHP_ARG_ENABLE(routeman,
    whether to enable the "routeman" extension,
    [  --enable-routeman      Enable "routeman" extension support]
)

if test $PHP_ROUTEMAN != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(ROUTEMAN_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, ROUTEMAN_SHARED_LIBADD)
    PHP_ADD_LIBRARY(boost_regex, 1, ROUTEMAN_SHARED_LIBADD)
    PHP_NEW_EXTENSION(routeman, routeman.cc src/router.cpp, $ext_shared)
fi
