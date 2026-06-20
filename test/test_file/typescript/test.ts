// TypeScript test file for tree-sitter symbol extraction
// Should extract: interface, type alias, class, function, method, module (namespace)

// ---- Namespace ----
namespace Geometry {
    export interface Point {
        x: number;
        y: number;
    }

    export function distance(a: Point, b: Point): number {
        const dx = a.x - b.x;
        const dy = a.y - b.y;
        return Math.sqrt(dx * dx + dy * dy);
    }
}

// ---- Interface ----
interface Shape {
    readonly name: string;
    area(): number;
    perimeter(): number;
}

// ---- Interface extending another ----
interface Drawable {
    draw(): void;
}

// ---- Type alias ----
type Coordinate = number;

type Point2D = {
    x: Coordinate;
    y: Coordinate;
};

// ---- Class implementing interface ----
class Circle implements Shape {
    constructor(public radius: number) {}

    get name(): string {
        return "Circle";
    }

    area(): number {
        return Math.PI * this.radius * this.radius;
    }

    perimeter(): number {
        return 2 * Math.PI * this.radius;
    }
}

// ---- Abstract class ----
abstract class Animal {
    abstract makeSound(): void;

    move(): void {
        console.log("Moving...");
    }
}

// ---- Class extending abstract ----
class Dog extends Animal {
    makeSound(): void {
        console.log("Woof!");
    }
}

// ---- Function ----
function createDefaultCircle(): Circle {
    return new Circle(1.0);
}

function calculateArea(radius: number): number {
    return Math.PI * radius * radius;
}

// ---- Arrow function ----
const square = (x: number): number => x * x;
