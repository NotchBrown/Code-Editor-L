// Java test file for tree-sitter symbol extraction
// Should extract: class, interface, enum, method, field

import java.util.*;

// ---- Interface ----
interface Drawable {
    void draw();
    double getArea();
}

// ---- Enum ----
enum Color {
    RED(0xFF0000),
    GREEN(0x00FF00),
    BLUE(0x0000FF);
    
    private final int rgb;
    
    Color(int rgb) {
        this.rgb = rgb;
    }
    
    public int getRgb() { return rgb; }
}

// ---- Class ----
public class Test {
    // Fields
    private String name;
    private int value;
    
    // Constructor
    public Test(String name, int value) {
        this.name = name;
        this.value = value;
    }
    
    // Methods
    public String getName() {
        return name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    public static Test createDefault() {
        return new Test("default", 42);
    }
    
    // Inner class
    static class Builder {
        private String name = "default";
        private int value = 0;
        
        public Builder withName(String name) {
            this.name = name;
            return this;
        }
        
        public Builder withValue(int value) {
            this.value = value;
            return this;
        }
        
        public Test build() {
            return new Test(name, value);
        }
    }
}

// ---- Abstract class ----
abstract class Shape implements Drawable {
    protected String id;
    
    public Shape(String id) {
        this.id = id;
    }
    
    public abstract void resize(double factor);
}

// ---- Extends class ----
class Circle extends Shape {
    private double radius;
    
    public Circle(String id, double radius) {
        super(id);
        this.radius = radius;
    }
    
    @Override
    public void draw() {
        System.out.println("Drawing circle: " + id);
    }
    
    @Override
    public double getArea() {
        return Math.PI * radius * radius;
    }
    
    @Override
    public void resize(double factor) {
        this.radius *= factor;
    }
}
