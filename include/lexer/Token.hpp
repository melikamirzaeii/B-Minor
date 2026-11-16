#pragma once

#include "common/SourceLocation.hpp"
#include <string>

namespace bminor {

enum class TokenType {
    KW_ARRAY,
    KW_AUTO,
    KW_BOOLEAN,
    KW_CHAR,
    KW_ELSE,
    KW_FALSE,
    KW_FOR,
    KW_FUNCTION,
    KW_IF,
    KW_INTEGER,
    KW_PRINT,
    KW_RETURN,
    KW_STRING,
    KW_TRUE,
    KW_VOID,
    KW_WHILE,

    IDENTIFIER,
    INT_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,

    OP_PLUS,
    OP_MINUS,
    OP_STAR,
    OP_SLASH,
    OP_PERCENT,
    OP_CARET,
    OP_INC,
    OP_DEC,
    OP_ASSIGN,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_LTE,
    OP_GT,
    OP_GTE,
    OP_AND,
    OP_OR,
    OP_NOT,

    DELIM_LBRACE,
    DELIM_RBRACE,
    DELIM_LPAREN,
    DELIM_RPAREN,
    DELIM_LBRACKET,
    DELIM_RBRACKET,
    DELIM_SEMICOLON,
    DELIM_COMMA,
    DELIM_COLON,

    WHITESPACE,
    LINE_COMMENT,
    BLOCK_COMMENT,

    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type = TokenType::INVALID;
    std::string lexeme;
    SourceLocation location;

    bool isTrivia() const noexcept {
        return type == TokenType::WHITESPACE ||
               type == TokenType::LINE_COMMENT ||
               type == TokenType::BLOCK_COMMENT;
    }

    bool isTypeKeyword() const noexcept {
        return type == TokenType::KW_INTEGER ||
               type == TokenType::KW_BOOLEAN ||
               type == TokenType::KW_CHAR ||
               type == TokenType::KW_STRING ||
               type == TokenType::KW_VOID ||
               type == TokenType::KW_AUTO;
    }

    bool isKeyword() const noexcept {
        return type >= TokenType::KW_ARRAY && type <= TokenType::KW_WHILE;
    }
};

const char* tokenTypeToString(TokenType type);

}