#include "parser.h"
#include <stdexcept>

// Look at the current token without consuming it
Token& Parser::peek() {
    return tokens[pos];
}

// Assert the current token is what we expect, then advance past it
Token Parser::consume(TokenType expected) {
    if (tokens[pos].type != expected)
        throw std::runtime_error(
            "Unexpected token: '" + tokens[pos].value + "'"
        );
    return tokens[pos++];
}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto prog = std::make_unique<ProgramNode>();
    while (peek().type != TokenType::END_OF_FILE)
        prog->statements.push_back(parseStatement());
    return prog;
}

std::vector<NodePtr> Parser::parseBlock() {
    consume(TokenType::LBRACE);           // expect opening {
    std::vector<NodePtr> stmts;
    while (peek().type != TokenType::RBRACE)
        stmts.push_back(parseStatement());
    consume(TokenType::RBRACE);           // expect closing }
    return stmts;
}

NodePtr Parser::parseStatement() {

    // let x = expr;
    if (peek().type == TokenType::LET) {
        consume(TokenType::LET);
        auto name = consume(TokenType::IDENTIFIER).value;
        consume(TokenType::ASSIGN);
        auto expr = parseExpr();
        consume(TokenType::SEMICOLON);
        return std::make_unique<AssignNode>(name, std::move(expr));
    }

    // x = expr;  (re-assignment, no let)
    if (peek().type == TokenType::IDENTIFIER) {
        auto name = consume(TokenType::IDENTIFIER).value;
        consume(TokenType::ASSIGN);
        auto expr = parseExpr();
        consume(TokenType::SEMICOLON);
        return std::make_unique<AssignNode>(name, std::move(expr));
    }

    // print(expr);
    if (peek().type == TokenType::PRINT) {
        consume(TokenType::PRINT);
        consume(TokenType::LPAREN);
        auto expr = parseExpr();
        consume(TokenType::RPAREN);
        consume(TokenType::SEMICOLON);
        return std::make_unique<PrintNode>(std::move(expr));
    }

    // input(varName);
    if (peek().type == TokenType::INPUT) {
        consume(TokenType::INPUT);
        consume(TokenType::LPAREN);
        auto name = consume(TokenType::IDENTIFIER).value;
        consume(TokenType::RPAREN);
        consume(TokenType::SEMICOLON);
        return std::make_unique<InputNode>(name);
    }

    // if (condition) { ... } else { ... }
    if (peek().type == TokenType::IF) {
        consume(TokenType::IF);
        consume(TokenType::LPAREN);
        auto cond = parseExpr();
        consume(TokenType::RPAREN);
        auto node = std::make_unique<IfNode>();
        node->condition = std::move(cond);
        node->thenBlock = parseBlock();
        if (peek().type == TokenType::ELSE) {
            consume(TokenType::ELSE);
            node->elseBlock = parseBlock();
        }
        return node;
    }

    // while (condition) { ... }
    if (peek().type == TokenType::WHILE) {
        consume(TokenType::WHILE);
        consume(TokenType::LPAREN);
        auto cond = parseExpr();
        consume(TokenType::RPAREN);
        auto node = std::make_unique<WhileNode>();
        node->condition = std::move(cond);
        node->body = parseBlock();
        return node;
    }

    throw std::runtime_error(
        "Unknown statement starting with: '" + peek().value + "'"
    );
}


// Parse an expression: handles binary operators
NodePtr Parser::parseExpr() {
    auto left = parsePrimary();
    while (
        peek().type == TokenType::PLUS  ||
        peek().type == TokenType::MINUS ||
        peek().type == TokenType::STAR  ||
        peek().type == TokenType::SLASH ||
        peek().type == TokenType::EQ    ||
        peek().type == TokenType::LT
    ) {
        auto op = tokens[pos++].value;   // consume the operator
        auto right = parsePrimary();
        left = std::make_unique<BinaryOpNode>(
            op, std::move(left), std::move(right)
        );
    }
    return left;
}

// Parse the simplest indivisible unit of an expression
NodePtr Parser::parsePrimary() {
    // Unary minus: -expr  →  (0 - expr)
    if (peek().type == TokenType::MINUS) {
        pos++;  // consume '-'
        auto operand = parsePrimary();
        return std::make_unique<BinaryOpNode>(
            "-",
            std::make_unique<NumberNode>(0),
            std::move(operand)
        );
    }

    // Unary plus: +expr  →  expr  (no-op)
    if (peek().type == TokenType::PLUS) {
        pos++;
        return parsePrimary();
    }
    
    // A number literal
    if (peek().type == TokenType::NUMBER)
        return std::make_unique<NumberNode>(std::stoi(tokens[pos++].value));

    // true / false
    if (peek().type == TokenType::BOOL_TRUE)  { pos++; return std::make_unique<BoolNode>(true); }
    if (peek().type == TokenType::BOOL_FALSE) { pos++; return std::make_unique<BoolNode>(false); }

    // A variable name
    if (peek().type == TokenType::IDENTIFIER)
        return std::make_unique<VarNode>(tokens[pos++].value);

    // A parenthesized expression: (expr)
    if (peek().type == TokenType::LPAREN) {
        consume(TokenType::LPAREN);
        auto expr = parseExpr();
        consume(TokenType::RPAREN);
        return expr;
    }

    throw std::runtime_error(
        "Unexpected token in expression: '" + peek().value + "'"
    );
}
