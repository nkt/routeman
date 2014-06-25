<?php

$count = 100;
$matches = 100;

$router = new Routeman\Router();

$startTime = microtime(true);
for ($i = 0, $str = 'a'; $i < $count; $i++, $str++) {
    $router->addRoute('route_' . $i, '/' . $str . '/{arg}');
    $lastStr = $str;
}
printf("Routeman added routes: %f\n", microtime(true) - $startTime);

// Lazy compile
$router->match('/');

$startTime = microtime(true);
for ($i = 0; $i < $matches; $i++) {
    $route = $router->match('/a/foo');
}
printf("Routeman first route: %f\n", (microtime(true) - $startTime)/$matches);

$startTime = microtime(true);
for ($i = 0; $i < $matches; $i++) {
    $route = $router->match('/'. $lastStr .'/foo');
}
printf("Routeman last route: %f\n", (microtime(true) - $startTime)/$matches);

$startTime = microtime(true);
for ($i = 0; $i < $matches; $i++) {
    $route = $router->match('/'. $lastStr .'/foo');
}
printf("Routeman unknown route: %f\n", (microtime(true) - $startTime)/$matches);
