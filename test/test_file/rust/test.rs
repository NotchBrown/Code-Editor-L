// Rust test file for tree-sitter symbol extraction
// Should extract: struct, enum, function, trait, impl method, module, type alias

mod utils {
    pub fn helper() -> i32 {
        42
    }
}

// ---- Traits ----
pub trait Drawable {
    fn draw(&self);
    fn area(&self) -> f64;
}

// ---- Struct ----
pub struct Circle {
    pub radius: f64,
    pub center_x: f64,
    pub center_y: f64,
}

// ---- Enum ----
pub enum Color {
    Red,
    Green,
    Blue,
    Rgb(u8, u8, u8),
}

// ---- Type alias ----
type Coordinate = f64;

// ---- Function ----
pub fn new_circle(radius: f64) -> Circle {
    Circle {
        radius,
        center_x: 0.0,
        center_y: 0.0,
    }
}

pub fn calculate_area(radius: f64) -> f64 {
    std::f64::consts::PI * radius * radius
}

// ---- Impl with method ----
impl Circle {
    pub fn diameter(&self) -> f64 {
        self.radius * 2.0
    }

    pub fn scale(&mut self, factor: f64) {
        self.radius *= factor;
    }
}

impl Drawable for Circle {
    fn draw(&self) {
        println!("Circle at ({}, {}) r={}", self.center_x, self.center_y, self.radius);
    }

    fn area(&self) -> f64 {
        std::f64::consts::PI * self.radius * self.radius
    }
}

// ---- Union ----
pub union IntOrFloat {
    pub int_val: i32,
    pub float_val: f32,
}
