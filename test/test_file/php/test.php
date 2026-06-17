<?php
// PHP test file for tree-sitter symbol extraction
// Should extract: class, interface, trait, function, method, constant

// ---- Constants ----
define('VERSION', '1.0.0');
define('AUTHOR', 'CodeEditorLite Team');

// ---- Function ----
function greet($name) {
    return "Hello, $name!";
}

function add($a, $b) {
    return $a + $b;
}

// ---- Interface ----
interface Drawable {
    public function draw();
    public function getArea();
}

// ---- Trait ----
trait Loggable {
    public function log($message) {
        echo "[LOG] $message\n";
    }
    
    protected function formatLog($level, $message) {
        return "[$level] $message";
    }
}

// ---- Class ----
class Animal {
    use Loggable;
    
    protected $name;
    protected $age;
    
    public function __construct($name, $age = 0) {
        $this->name = $name;
        $this->age = $age;
    }
    
    public function speak() {
        return "...";
    }
    
    public static function create($name) {
        return new static($name);
    }
}

// ---- Derived class ----
class Dog extends Animal implements Drawable {
    private $breed;
    
    public function __construct($name, $breed = 'unknown') {
        parent::__construct($name);
        $this->breed = $breed;
    }
    
    public function speak() {
        return "{$this->name} says Woof!";
    }
    
    public function draw() {
        echo "Drawing dog: {$this->name}\n";
    }
    
    public function getArea() {
        return 0;
    }
}

// ---- Abstract class ----
abstract class Shape {
    protected $id;
    
    public function __construct($id) {
        $this->id = $id;
    }
    
    abstract protected function calculateArea();
}
