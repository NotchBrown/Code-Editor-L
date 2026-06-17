# Python test file for tree-sitter symbol extraction
# Should extract: class, function, variable

import os
import sys
from typing import List, Optional

# ---- Module-level variables ----
VERSION = "1.0.0"
AUTHOR = "CodeEditorLite Team"
DEBUG_MODE = True

# ---- Function ----
def greet(name: str) -> str:
    """Greet someone."""
    return f"Hello, {name}!"

def add(a: int, b: int) -> int:
    """Add two numbers."""
    return a + b

def process_items(items: List[str]) -> None:
    """Process a list of items."""
    for item in items:
        print(f"Processing: {item}")

def get_config_path() -> Optional[str]:
    """Get the configuration file path."""
    config = os.path.join(os.path.dirname(__file__), "config.json")
    if os.path.exists(config):
        return config
    return None

# ---- Class ----
class Animal:
    """Base class for animals."""
    
    species_count = 0
    
    def __init__(self, name: str):
        self.name = name
        Animal.species_count += 1
    
    def speak(self) -> str:
        """Make a sound."""
        return "..."
    
    @classmethod
    def get_count(cls) -> int:
        return cls.species_count
    
    @staticmethod
    def is_animal(obj) -> bool:
        return isinstance(obj, Animal)

class Dog(Animal):
    """Dog class inheriting from Animal."""
    
    def __init__(self, name: str, breed: str):
        super().__init__(name)
        self.breed = breed
    
    def speak(self) -> str:
        return "Woof!"

class Cat(Animal):
    """Cat class inheriting from Animal."""
    
    def speak(self) -> str:
        return "Meow!"

# ---- Variables inside function (should NOT be extracted) ----
def run_tests():
    test_name = "unit_test"
    test_count = 42
    print(f"Running {test_name}: {test_count} tests")
