// Go test file for tree-sitter symbol extraction
// Should extract: function, struct, interface, method, variable, type

package main

import (
	"fmt"
	"math"
)

// ---- Constants ----
const (
	Pi    = 3.14159
	AppName = "CodeEditorLite"
)

// ---- Variables ----
var version = "1.0.0"
var debugMode = true

// ---- Struct ----
type Point struct {
	X float64
	Y float64
}

type Rectangle struct {
	TopLeft     Point
	BottomRight Point
}

// ---- Interface ----
type Shape interface {
	Area() float64
	Perimeter() float64
}

// ---- Method on struct ----
func (p Point) Distance(other Point) float64 {
	dx := p.X - other.X
	dy := p.Y - other.Y
	return math.Sqrt(dx*dx + dy*dy)
}

// ---- Function ----
func NewPoint(x, y float64) Point {
	return Point{X: x, Y: y}
}

func CalculateArea(shape Shape) float64 {
	return shape.Area()
}

// ---- Method on type ----
func (r Rectangle) Area() float64 {
	width := r.BottomRight.X - r.TopLeft.X
	height := r.BottomRight.Y - r.TopLeft.Y
	return width * height
}

func (r Rectangle) Perimeter() float64 {
	width := r.BottomRight.X - r.TopLeft.X
	height := r.BottomRight.Y - r.TopLeft.Y
	return 2 * (width + height)
}

// ---- Type alias ----
type Float = float64

// ---- Main function ----
func main() {
	p1 := NewPoint(0, 0)
	p2 := NewPoint(3, 4)
	fmt.Println(p1.Distance(p2))
}
