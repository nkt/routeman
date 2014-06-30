<?php

class RouterTest extends \PHPUnit_Framework_TestCase
{
    public function testSimpleMatch()
    {
        $router = new \Routeman\Router();
        $router->add('users', '/users/{id}');
        $match = $router->match('/users/1', 'GET');

        $this->assertNotNull($match);
        $this->assertSame('users', $match['name']);
        $this->assertSame(array(
            'id' => '1'
        ), $match['parameters']);

        $this->assertNull($router->match('/', 'GET'));
        $this->assertNull($router->match('/users/1/', 'GET'));
    }

    public function testPatternMatch()
    {
        $router = new \Routeman\Router();
        $router->add('post_by_tag', '/users/{i:id}/posts/{a:tag}');
        $match = $router->match('/users/100500/posts/cpp', 'GET');

        $this->assertNotNull($match);
        $this->assertSame('post_by_tag', $match['name']);
        $this->assertSame(array(
            'id'  => '100500',
            'tag' => 'cpp'
        ), $match['parameters']);

        $this->assertNull($router->match('/users/foo/posts/c++', 'GET'));
    }
}
