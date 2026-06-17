# Ruby test file for tree-sitter symbol extraction
# Should extract: class, module, method, constant

# ---- Constants ----
VERSION = '1.0.0'
AUTHOR = 'CodeEditorLite Team'

# ---- Module ----
module Utils
  def self.format_name(name)
    name.strip.capitalize
  end
  
  def self.log(message, level = :info)
    puts "[#{level.upcase}] #{message}"
  end
end

# ---- Class ----
class Animal
  attr_reader :name
  attr_accessor :age
  
  def initialize(name, age = 0)
    @name = name
    @age = age
  end
  
  def speak
    raise NotImplementedError
  end
  
  def self.create(name)
    new(name)
  end
end

# ---- Derived class ----
class Dog < Animal
  def initialize(name, breed = 'unknown')
    super(name)
    @breed = breed
  end
  
  def speak
    "#{@name} says Woof!"
  end
  
  def breed
    @breed
  end
end

class Cat < Animal
  def speak
    "#{@name} says Meow!"
  end
end

# ---- Mixin module ----
module Printable
  def to_s
    "#{self.class.name}: #{@name}"
  end
  
  def inspect
    "<#{self.class.name}: #{@name}>"
  end
end

# ---- Class with mixin ----
class Robot
  include Printable
  
  def initialize(name)
    @name = name
  end
  
  def work
    puts "#{@name} is working..."
  end
end

# ---- Standalone method ----
def greet(name)
  "Hello, #{name}!"
end

def add(a, b)
  a + b
end
