// C# test file for tree-sitter symbol extraction
// Should extract: class, struct, interface, enum, method, property, namespace

using System;
using System.Collections.Generic;

// ---- Namespace ----
namespace MyApp {
    
    // ---- Interface ----
    public interface IShape {
        double GetArea();
        void Draw();
    }
    
    // ---- Enum ----
    public enum Color {
        Red,
        Green,
        Blue
    }
    
    // ---- Struct ----
    public struct Point {
        public double X { get; set; }
        public double Y { get; set; }
        
        public Point(double x, double y) {
            X = x;
            Y = y;
        }
        
        public double DistanceTo(Point other) {
            double dx = X - other.X;
            double dy = Y - other.Y;
            return Math.Sqrt(dx * dx + dy * dy);
        }
    }
    
    // ---- Class ----
    public class Shape : IShape {
        public string Id { get; set; }
        
        public Shape(string id) {
            Id = id;
        }
        
        public virtual double GetArea() {
            return 0;
        }
        
        public void Draw() {
            Console.WriteLine($"Drawing shape: {Id}");
        }
    }
    
    // ---- Derived class ----
    public class Circle : Shape {
        public double Radius { get; set; }
        
        public Circle(string id, double radius) : base(id) {
            Radius = radius;
        }
        
        public override double GetArea() {
            return Math.PI * Radius * Radius;
        }
    }
    
    // ---- Static class ----
    public static class MathUtils {
        public static double Add(double a, double b) => a + b;
        public static double Multiply(double a, double b) => a * b;
    }
    
    // ---- Generic class ----
    public class Container<T> {
        private List<T> items = new List<T>();
        
        public void Add(T item) {
            items.Add(item);
        }
        
        public T Get(int index) {
            return items[index];
        }
    }
}
