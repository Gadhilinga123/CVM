#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // Literals — actual values in source code
    NUMBER,
    BOOL_TRUE,
    BOOL_FALSE,

    // Identifiers and Keywords
    IDENTIFIER,   // variable names like x, count, total
    LET,          // keyword: let
    IF,           // keyword: if
    ELSE,         // keyword: else
    WHILE,        // keyword: while
    PRINT,        // keyword: print
    INPUT,        // keyword: input

    // Arithmetic Operators
    PLUS,         // +
    MINUS,        // -
    STAR,         // *
    SLASH,        // /

    // Comparison Operators
    EQ,           // ==
    LT,           // 

    // Symbols
    ASSIGN,       // =
    SEMICOLON,    // ;
    LPAREN,       // (
    RPAREN,       // )
    LBRACE,       // {
    RBRACE,       // }

    // Special marker for end of file
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& source);