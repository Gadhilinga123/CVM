#include "lexer.h"
#include <cctype>
#include <stdexcept>

std::vector<Token> tokenize(const std::string& src) {
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < src.size()) {
        // Skip whitespace — spaces, tabs, newlines don't mean anything
        if (isspace(src[i])) {
            i++;
            continue;
        }

        // Handle numbers: keep reading digits until they stop
        if (isdigit(src[i])) {
            std::string num;
            while (i < src.size() && isdigit(src[i]))
                num += src[i++];
            tokens.push_back({TokenType::NUMBER, num});
            continue;
        }

        // Handle identifiers and keywords
        if (isalpha(src[i]) || src[i] == '_') {
            std::string word;
            while (i < src.size() && (isalnum(src[i]) || src[i] == '_'))
                word += src[i++];

            if (word == "let")   tokens.push_back({TokenType::LET,   word});
            else if (word == "if")    tokens.push_back({TokenType::IF,    word});
            else if (word == "else")  tokens.push_back({TokenType::ELSE,  word});
            else if (word == "while") tokens.push_back({TokenType::WHILE, word});
            else if (word == "print") tokens.push_back({TokenType::PRINT, word});
            else if (word == "input") tokens.push_back({TokenType::INPUT, word});
            else if (word == "true")  tokens.push_back({TokenType::BOOL_TRUE,  word});
            else if (word == "false") tokens.push_back({TokenType::BOOL_FALSE, word});
            else                      tokens.push_back({TokenType::IDENTIFIER, word});
            continue;
        }

        // Handle single-character and two-character operators
        switch (src[i]) {
            case '+': tokens.push_back({TokenType::PLUS,      "+"}); break;
            case '-': tokens.push_back({TokenType::MINUS,     "-"}); break;
            case '*': tokens.push_back({TokenType::STAR,      "*"}); break;
            case '/': tokens.push_back({TokenType::SLASH,     "/"}); break;
            case '<': tokens.push_back({TokenType::LT,        "<"}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";"}); break;
            case '(': tokens.push_back({TokenType::LPAREN,    "("}); break;
            case ')': tokens.push_back({TokenType::RPAREN,    ")"}); break;
            case '{': tokens.push_back({TokenType::LBRACE,    "{"}); break;
            case '}': tokens.push_back({TokenType::RBRACE,    "}"}); break;

            case '=':
                if (i + 1 < src.size() && src[i + 1] == '=') {
                    tokens.push_back({TokenType::EQ, "=="});
                    i++; // consume the second '=' too
                } else {
                    tokens.push_back({TokenType::ASSIGN, "="});
                }
                break;

            default:
                throw std::runtime_error(
                    std::string("Unknown character: ") + src[i]
                );
        }
        i++;
    }
    
    tokens.push_back({TokenType::END_OF_FILE, ""});
    return tokens;
}