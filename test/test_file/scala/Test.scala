// Scala test file for tree-sitter symbol extraction
// Should extract: class, object, trait, def, val, var, type

package com.codeeditorlite

// ---- Import ----
import scala.collection.mutable

// ---- Object (singleton) ----
object Constants {
  val Version = "1.0.0"
  val Author = "CodeEditorLite Team"
}

// ---- Trait ----
trait Drawable {
  def draw(): Unit
  def area: Double
}

// ---- Class ----
class Animal(val name: String) {
  protected var age: Int = 0
  
  def this(name: String, age: Int) = {
    this(name)
    this.age = age
  }
  
  def speak(): String = "..."
  
  override def toString: String = s"Animal($name)"
}

// ---- Case class ----
case class Dog(name: String, breed: String) extends Animal(name) {
  override def speak(): String = s"$name says Woof!"
}

case class Cat(name: String) extends Animal(name) {
  override def speak(): String = s"$name says Meow!"
}

// ---- Abstract class ----
abstract class Shape(val id: String) extends Drawable {
  def resize(factor: Double): Unit
}

// ---- Class with generic ----
class Container[T] {
  private val items = mutable.Buffer.empty[T]
  
  def add(item: T): Unit = items += item
  
  def get(index: Int): T = items(index)
  
  def getAll: Seq[T] = items.toSeq
}

// ---- Standalone function ----
def greet(name: String): String = {
  s"Hello, $name!"
}

def add(a: Int, b: Int): Int = a + b

// ---- Value ----
val pi: Double = 3.14159
val appName: String = "CodeEditorLite"
