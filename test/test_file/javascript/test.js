// JavaScript test file for tree-sitter symbol extraction
// Should extract: class, function, method, variable (const/let), generator function

// ---- Variables ----
const VERSION = "1.0.0";
const AUTHOR = "CodeEditorLite Team";
let debugMode = true;
var globalCounter = 0;

// ---- Function ----
function greet(name) {
    return `Hello, ${name}!`;
}

function add(a, b) {
    return a + b;
}

function processItems(items) {
    items.forEach(item => {
        console.log(`Processing: ${item}`);
    });
}

// ---- Generator function ----
function* countTo(n) {
    for (let i = 1; i <= n; i++) {
        yield i;
    }
}

// ---- Class ----
class Animal {
    constructor(name) {
        this.name = name;
    }
    
    speak() {
        return "...";
    }
    
    static getKingdom() {
        return "Animalia";
    }
}

class Dog extends Animal {
    constructor(name, breed) {
        super(name);
        this.breed = breed;
    }
    
    speak() {
        return "Woof!";
    }
}

// ---- Arrow function assigned to variable (should NOT be extracted ideally) ----
const multiply = (a, b) => a * b;

// ---- Object method shorthand ----
const utils = {
    format(str) {
        return str.trim();
    },
    parse(data) {
        return JSON.parse(data);
    }
};
