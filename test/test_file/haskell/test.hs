-- Haskell test file for tree-sitter symbol extraction
-- Should extract: module, function declarations

module Main where

import Data.List (sort)
import qualified Data.Map as Map

-- ---- Type signature ----
add :: Int -> Int -> Int
add x y = x + y

-- ---- Function ----
factorial 0 = 1
factorial n = n * factorial (n - 1)

-- ---- Function with type signature ----
fibonacci :: Int -> Int
fibonacci 0 = 0
fibonacci 1 = 1
fibonacci n = fibonacci (n - 1) + fibonacci (n - 2)

-- ---- Higher-order function ----
mapWithIndex :: (Int -> a -> b) -> [a] -> [b]
mapWithIndex f xs = map (\(i, x) -> f i x) (zip [0..] xs)

-- ---- Pattern matching ----
data Maybe a = Nothing | Just a

safeDiv :: Int -> Int -> Maybe Int
safeDiv _ 0 = Nothing
safeDiv x y = Just (x `div` y)

-- ---- List processing ----
quicksort :: Ord a => [a] -> [a]
quicksort [] = []
quicksort (x:xs) =
    let smaller = quicksort [a | a <- xs, a <= x]
        larger  = quicksort [a | a <- xs, a > x]
    in smaller ++ [x] ++ larger

main :: IO ()
main = do
    putStrLn "Hello from tree-sitter Haskell test"
    print (factorial 10)
    print (fibonacci 20)
