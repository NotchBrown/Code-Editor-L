cmake_minimum_required(VERSION 3.10)
project(tree-sitter-grammars C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find tree-sitter headers
set(TREE_SITTER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/../tree_sitter_mingw64/include")

# Output directory for grammar DLLs
set(GRAMMAR_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/../tree_sitter_mingw64/grammars")

# Function to build a grammar as a shared library
function(add_tree_sitter_grammar name)
    set(SOURCES "tree-sitter-${name}/src/parser.c")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tree-sitter-${name}/src/scanner.c")
        list(APPEND SOURCES "tree-sitter-${name}/src/scanner.c")
    endif()
    
    add_library("ts_${name}" SHARED ${SOURCES})
    target_include_directories("ts_${name}" PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/tree-sitter-${name}/src"
        ${TREE_SITTER_INCLUDE_DIR}
    )
    target_compile_definitions("ts_${name}" PRIVATE TREE_SITTER_HIDE_SYMBOLS)
    
    # Set output name and directory
    set_target_properties("ts_${name}" PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${GRAMMAR_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${GRAMMAR_OUTPUT_DIR}"
        PREFIX ""
    )
    
    if(WIN32)
        set_target_properties("ts_${name}" PROPERTIES
            SUFFIX ".dll"
        )
    endif()
    
    add_custom_command(TARGET "ts_${name}" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:ts_${name}>"
            "${GRAMMAR_OUTPUT_DIR}/$<TARGET_FILE_NAME:ts_${name}>"
    )
endfunction()

# Build grammars
add_tree_sitter_grammar("c")
add_tree_sitter_grammar("cpp")
add_tree_sitter_grammar("python")
add_tree_sitter_grammar("javascript")
add_tree_sitter_grammar("bash")
add_tree_sitter_grammar("java")
add_tree_sitter_grammar("c-sharp")
add_tree_sitter_grammar("ruby")
add_tree_sitter_grammar("html")
add_tree_sitter_grammar("css")
add_tree_sitter_grammar("json")
add_tree_sitter_grammar("verilog")
