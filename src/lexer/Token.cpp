#include "lexer/Token.hpp"

namespace bminor {

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KW_ARRAY:         return "KW_ARRAY";
        case TokenType::KW_AUTO:          return "KW_AUTO";
        case TokenType::KW_BOOLEAN:       return "KW_BOOLEAN";
        case TokenType::KW_CHAR:          return "KW_CHAR";
        case TokenType::KW_ELSE:          return "KW_ELSE";
        case TokenType::KW_FALSE:         return "KW_FALSE";
        case TokenType::KW_FOR:           return "KW_FOR";
        case TokenType::KW_FUNCTION:      return "KW_FUNCTION";
        case TokenType::KW_IF:            return "KW_IF";
        case TokenType::KW_INTEGER:       return "KW_INTEGER";
        case TokenType::KW_PRINT:         return "KW_PRINT";
        case TokenType::KW_RETURN:        return "KW_RETURN";
        case TokenType::KW_STRING:        return "KW_STRING";
        case TokenType::KW_TRUE:          return "KW_TRUE";
        case TokenType::KW_VOID:          return "KW_VOID";
        case TokenType::KW_WHILE:         return "KW_WHILE";
        case TokenType::IDENTIFIER:       return "IDENTIFIER";
        case TokenType::INT_LITERAL:      return "INT_LITERAL";
        case TokenType::CHAR_LITERAL:     return "CHAR_LITERAL";
        case TokenType::STRING_LITERAL:   return "STRING_LITERAL";
        case TokenType::OP_PLUS:          return "OP_PLUS";
        case TokenType::OP_MINUS:         return "OP_MINUS";
        case TokenType::OP_STAR:          return "OP_STAR";
        case TokenType::OP_SLASH:         return "OP_SLASH";
        case TokenType::OP_PERCENT:       return "OP_PERCENT";
        case TokenType::OP_CARET:         return "OP_CARET";
        case TokenType::OP_INC:           return "OP_INC";
        case TokenType::OP_DEC:           return "OP_DEC";
        case TokenType::OP_ASSIGN:        return "OP_ASSIGN";
        case TokenType::OP_EQ:            return "OP_EQ";
        case TokenType::OP_NEQ:           return "OP_NEQ";
        case TokenType::OP_LT:            return "OP_LT";
        case TokenType::OP_LTE:           return "OP_LTE";
        case TokenType::OP_GT:            return "OP_GT";
        case TokenType::OP_GTE:           return "OP_GTE";
        case TokenType::OP_AND:           return "OP_AND";
        case TokenType::OP_OR:            return "OP_OR";
        case TokenType::OP_NOT:           return "OP_NOT";
        case TokenType::DELIM_LBRACE:     return "DELIM_LBRACE";
        case TokenType::DELIM_RBRACE:     return "DELIM_RBRACE";
        case TokenType::DELIM_LPAREN:     return "DELIM_LPAREN";
        case TokenType::DELIM_RPAREN:     return "DELIM_RPAREN";
        case TokenType::DELIM_LBRACKET:   return "DELIM_LBRACKET";
        case TokenType::DELIM_RBRACKET:   return "DELIM_RBRACKET";
        case TokenType::DELIM_SEMICOLON:  return "DELIM_SEMICOLON";
        case TokenType::DELIM_COMMA:      return "DELIM_COMMA";
        case TokenType::DELIM_COLON:      return "DELIM_COLON";
        case TokenType::WHITESPACE:       return "WHITESPACE";
        case TokenType::LINE_COMMENT:     return "LINE_COMMENT";
        case TokenType::BLOCK_COMMENT:    return "BLOCK_COMMENT";
        case TokenType::END_OF_FILE:      return "END_OF_FILE";
        case TokenType::INVALID:          return "INVALID";
    }
    return "UNKNOWN";
}

}