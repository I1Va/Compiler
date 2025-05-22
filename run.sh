#!/usr/bin/env bash

action=""
graphics=0
filename=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -S|-C)
            if [[ -n "$action" ]]; then
                echo "Error: Multiple actions specified" >&2
                exit 1
            fi
            action="${1#-}"
            shift
            if [[ $# -eq 0 ]]; then
                echo "Error: Missing filename for -$action" >&2
                exit 1
            fi
            filename="$1"
            shift
            if [[ $# -gt 0 ]]; then
                echo "Error: Extra arguments after filename" >&2
                exit 1
            fi
            ;;
        --AST)
            if [[ -n "$action" ]]; then
                echo "Error: Multiple actions specified" >&2
                exit 1
            fi
            action="AST"
            shift
            ;;
        --graphics)
            if [[ "$action" != "AST" ]]; then
                echo "Error: --graphics requires --AST" >&2
                exit 1
            fi
            graphics=1
            shift
            ;;
        *)
            if [[ -z "$filename" ]]; then
                filename="$1"
                shift
            else
                echo "Error: Unexpected argument $1" >&2
                exit 1
            fi
            ;;
    esac
done

if [[ -z "$action" ]]; then
    echo "Error: No action specified. Use -S, -C, or --AST." >&2
    exit 1
fi

if [[ -z "$filename" ]]; then
    echo "Error: Filename not provided" >&2
    exit 1
fi

if [[ ! -f "$filename" ]]; then
    echo "Error: File '$filename' not found" >&2
    exit 1
fi

short_filename="${filename%.*}"
ast_filename="${short_filename}.ast"
asm_filename="${short_filename}.asm"
png_filename="${short_filename}.png"
standart_lib_dir=standart_lib


case "$action" in
    S)
        ./FrontEnd/build/bin/FrontEnd -i=$filename -o=$ast_filename
        ./BackEnd/build/bin/BackEnd -s=$standart_lib_dir -i=$ast_filename -o=$asm_filename
        ;;
    C)
        echo "THERE IS SHOULD BE COMPILATION IN ELF FILE"
        ;;
    AST)
        if [[ $graphics -eq 1 ]]; then
            ./FrontEnd/build/bin/FrontEnd -i=$filename -o=$ast_filename -p=$png_filename
        else
            ./FrontEnd/build/bin/FrontEnd -i=$filename -o=$ast_filename
        fi
        ./BackEnd/build/bin/BackEnd -s=$standart_lib_dir -i=$ast_filename -o=$asm_filename
        ;;
    *)
        echo "Error: Invalid action" >&2
        exit 1
        ;;
esac

if [[ $? -ne 0 ]]; then
    echo "Error: Command execution failed" >&2
    exit 1
fi