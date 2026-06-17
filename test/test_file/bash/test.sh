#!/bin/bash
# Bash test file for tree-sitter symbol extraction
# Should extract: function definition, variable assignment

# ---- Variables ----
VERSION="1.0.0"
PROJECT_NAME="CodeEditorLite"
DEBUG_MODE=true
MAX_FILES=100

# ---- Function ----
function print_usage() {
    echo "Usage: $0 [options]"
    echo "  -h, --help     Show help"
    echo "  -v, --version  Show version"
}

function init_project() {
    local project_dir="$1"
    mkdir -p "$project_dir"/{src,inc,lib,doc}
    echo "Project initialized at $project_dir"
}

# ---- Function (POSIX style) ----
cleanup() {
    echo "Cleaning up..."
    rm -rf /tmp/build_*
}

is_valid_file() {
    local file="$1"
    [[ -f "$file" && -r "$file" ]]
}

# ---- Function with local variables ----
process_directory() {
    local dir="$1"
    local count=0
    
    for file in "$dir"/*; do
        if is_valid_file "$file"; then
            echo "Processing: $file"
            ((count++))
        fi
    done
    
    echo "Processed $count files"
    return $count
}

# ---- Main entry point ----
main() {
    case "$1" in
        -h|--help)
            print_usage
            ;;
        -v|--version)
            echo "$PROJECT_NAME version $VERSION"
            ;;
        init)
            shift
            init_project "$@"
            ;;
        *)
            process_directory "${1:-.}"
            ;;
    esac
}

# Execute main
main "$@"
