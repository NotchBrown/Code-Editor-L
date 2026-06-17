! Fortran test file for tree-sitter symbol extraction
! Should extract: module, function, subroutine, program, interface, type

! ---- Module ----
module math_utils
    implicit none
    private
    
    ! Public interface
    public :: add, subtract, multiply, divide
    public :: PI, E
    
    ! ---- Module parameters ----
    real, parameter :: PI = 3.14159265
    real, parameter :: E = 2.71828183
    
    ! ---- Derived type ----
    type :: vector2d
        real :: x
        real :: y
    end type vector2d
    
contains

    ! ---- Module function ----
    function add(a, b) result(sum)
        real, intent(in) :: a, b
        real :: sum
        sum = a + b
    end function add
    
    ! ---- Module subroutine ----
    subroutine swap(a, b)
        real, intent(inout) :: a, b
        real :: temp
        temp = a
        a = b
        b = temp
    end subroutine swap
    
    ! ---- Pure function ----
    pure function multiply(a, b) result(product)
        real, intent(in) :: a, b
        real :: product
        product = a * b
    end function multiply
    
end module math_utils

! ---- Program ----
program main
    use math_utils
    implicit none
    
    real :: x, y, result
    type(vector2d) :: v
    
    x = 10.0
    y = 20.0
    
    result = add(x, y)
    print *, "Sum:", result
    
    call swap(x, y)
    print *, "After swap:", x, y
    
    v%x = 1.0
    v%y = 2.0
    print *, "Vector:", v%x, v%y
    
end program main

! ---- Standalone subroutine ----
subroutine print_matrix(matrix, rows, cols)
    implicit none
    real, intent(in) :: matrix(:,:)
    integer, intent(in) :: rows, cols
    integer :: i, j
    
    do i = 1, rows
        do j = 1, cols
            write(*, '(F8.2)', advance='no') matrix(i, j)
        end do
        write(*, *)
    end do
end subroutine print_matrix

! ---- Standalone function ----
function norm(x, y) result(n)
    implicit none
    real, intent(in) :: x, y
    real :: n
    n = sqrt(x*x + y*y)
end function norm

! ---- Interface block ----
interface
    function external_func(a, b) result(c)
        real, intent(in) :: a, b
        real :: c
    end function external_func
end interface
