#include "lexer/Lexer.hpp"
#include <cctype>
#include <unordered_map>

namespace bminor {

Lexer::Lexer(std::string source, std::string filename, DiagnosticEngine& diagnostics)
    : source_(std::move(source)), filename_(std::move(filename)), diagnostics_(diagnostics) {}

bool Lexer::isAtEnd() const noexcept {
    return pos_ >= source_.size();
}

char Lexer::peek(std::size_t lookahead) const noexcept {
    if (pos_ + lookahead >= source_.size()) return '\0';
    return source_[pos_ + lookahead];
}

char Lexer::advance() noexcept {
    char c = source_[pos_++];
    if (c == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return c;
}

bool Lexer::match(char expected) noexcept {
    if (isAtEnd() || source_[pos_] != expected) return false;
    advance();
    return true;
}

SourceLocation Lexer::currentLocation() const {
    return SourceLocation{filename_, line_, column_, pos_, 0};
}

Token Lexer::makeToken(TokenType type, const SourceLocation& startLoc) const {
    SourceLocation loc = startLoc;
    loc.length = pos_ - startLoc.offset;
    return Token{type, source_.substr(startLoc.offset, loc.length), loc};
}

std::vector<Token> Lexer::tokenizeAll() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        tokens.push_back(scanNextToken());
    }
    SourceLocation eofLoc = currentLocation();
    tokens.push_back(Token{TokenType::END_OF_FILE, "", eofLoc});
    return tokens;
}

Token Lexer::scanNextToken() {
    SourceLocation startLoc = currentLocation();
    char c = advance();

    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
        return scanWhitespace(startLoc);
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        return scanIdentifierOrKeyword(startLoc);
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
        return scanNumber(startLoc);
    }

    if (c == '"') return scanStringLiteral(startLoc);
    if (c == '\'') return scanCharLiteral(startLoc);

    switch (c) {
        case '+':
            return makeToken(match('+') ? TokenType::OP_INC : TokenType::OP_PLUS, startLoc);
        case '-':
            return makeToken(match('-') ? TokenType::OP_DEC : TokenType::OP_MINUS, startLoc);
        case '*':
            return makeToken(TokenType::OP_STAR, startLoc);
        case '/':
            if (match('/')) return scanLineComment(startLoc);
            if (match('*')) return scanBlockComment(startLoc);
            return makeToken(TokenType::OP_SLASH, startLoc);
        case '%':
            return makeToken(TokenType::OP_PERCENT, startLoc);
        case '^':
            return makeToken(TokenType::OP_CARET, startLoc);
        case '=':
            return makeToken(match('=') ? TokenType::OP_EQ : TokenType::OP_ASSIGN, startLoc);
        case '!':
            return makeToken(match('=') ? TokenType::OP_NEQ : TokenType::OP_NOT, startLoc);
        case '<':
            return makeToken(match('=') ? TokenType::OP_LTE : TokenType::OP_LT, startLoc);
        case '>':
            return makeToken(match('=') ? TokenType::OP_GTE : TokenType::OP_GT, startLoc);
        case '&':
            if (match('&')) return makeToken(TokenType::OP_AND, startLoc);
            {
                Token err = makeToken(TokenType::INVALID, startLoc);
                diagnostics_.reportLexError(err.location, "Unexpected character '&' (did you mean '&&'?)");
                return err;
            }
        case '|':
            if (match('|')) return makeToken(TokenType::OP_OR, startLoc);
            {
                Token err = makeToken(TokenType::INVALID, startLoc);
                diagnostics_.reportLexError(err.location, "Unexpected character '|' (did you mean '||'?)");
                return err;
            }
        case '{': return makeToken(TokenType::DELIM_LBRACE, startLoc);
        case '}': return makeToken(TokenType::DELIM_RBRACE, startLoc);
        case '(': return makeToken(TokenType::DELIM_LPAREN, startLoc);
        case ')': return makeToken(TokenType::DELIM_RPAREN, startLoc);
        case '[': return makeToken(TokenType::DELIM_LBRACKET, startLoc);
        case ']': return makeToken(TokenType::DELIM_RBRACKET, startLoc);
        case ';': return makeToken(TokenType::DELIM_SEMICOLON, startLoc);
        case ',': return makeToken(TokenType::DELIM_COMMA, startLoc);
        case ':': return makeToken(TokenType::DELIM_COLON, startLoc);
        default: {
            Token err = makeToken(TokenType::INVALID, startLoc);
            diagnostics_.reportLexError(err.location,
                std::string("Unrecognized character '") + c + "'");
            return err;
        }
    }
}

Token Lexer::scanWhitespace(const SourceLocation& startLoc) {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
    return makeToken(TokenType::WHITESPACE, startLoc);
}

Token Lexer::scanLineComment(const SourceLocation& startLoc) {
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
    return makeToken(TokenType::LINE_COMMENT, startLoc);
}

Token Lexer::scanBlockComment(const SourceLocation& startLoc) {
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance();
            advance();
            return makeToken(TokenType::BLOCK_COMMENT, startLoc);
        }
        advance();
    }
    Token err = makeToken(TokenType::INVALID, startLoc);
    diagnostics_.reportLexError(err.location, "Unterminated multi-line block comment");
    return err;
}

Token Lexer::scanIdentifierOrKeyword(const SourceLocation& startLoc) {
    while (!isAtEnd()) {
        char c = peek();
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            advance();
        } else {
            break;
        }
    }
    Token tok = makeToken(TokenType::IDENTIFIER, startLoc);
    tok.type = lookupKeyword(tok.lexeme);
    return tok;
}

Token Lexer::scanNumber(const SourceLocation& startLoc) {
    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }
    if (!isAtEnd() && (std::isalpha(static_cast<unsigned char>(peek())) || peek() == '_')) {
        while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
            advance();
        }
        Token err = makeToken(TokenType::INVALID, startLoc);
        diagnostics_.reportLexError(err.location, "Malformed numeric literal '" + err.lexeme + "'");
        return err;
    }
    return makeToken(TokenType::INT_LITERAL, startLoc);
}

bool Lexer::isValidEscapeChar(char c) noexcept {
    switch (c) {
        case 'n': case 't': case 'r': case '\\':
        case '"': case '\'': case '0': case 'a':
        case 'b': case 'f': case 'v':
            return true;
        default:
            return false;
    }
}

Token Lexer::scanStringLiteral(const SourceLocation& startLoc) {
    bool hasBadEscape = false;
    while (!isAtEnd()) {
        char c = peek();
        if (c == '\n') {
            Token err = makeToken(TokenType::INVALID, startLoc);
            diagnostics_.reportLexError(err.location, "Unterminated string literal before newline");
            return err;
        }
        if (c == '"') {
            advance();
            if (hasBadEscape) {
                return makeToken(TokenType::INVALID, startLoc);
            }
            return makeToken(TokenType::STRING_LITERAL, startLoc);
        }
        if (c == '\\') {
            SourceLocation escLoc = currentLocation();
            advance();
            if (isAtEnd() || peek() == '\n') {
                Token err = makeToken(TokenType::INVALID, startLoc);
                diagnostics_.reportLexError(err.location, "Unterminated string literal after escape");
                return err;
            }
            char esc = advance();
            if (!isValidEscapeChar(esc)) {
                diagnostics_.reportLexError(escLoc,
                    std::string("Invalid escape sequence '\\") + esc + "' in string literal");
                hasBadEscape = true;
            }
        } else {
            advance();
        }
    }

    Token err = makeToken(TokenType::INVALID, startLoc);
    diagnostics_.reportLexError(err.location, "Unterminated string literal at end of file");
    return err;
}

Token Lexer::scanCharLiteral(const SourceLocation& startLoc) {
    if (isAtEnd() || peek() == '\n') {
        Token err = makeToken(TokenType::INVALID, startLoc);
        diagnostics_.reportLexError(err.location, "Unterminated character literal");
        return err;
    }
    if (peek() == '\'') {
        advance();
        Token err = makeToken(TokenType::INVALID, startLoc);
        diagnostics_.reportLexError(err.location, "Empty character literal ''");
        return err;
    }

    bool valid = true;
    if (peek() == '\\') {
        advance();
        if (isAtEnd() || peek() == '\n') {
            Token err = makeToken(TokenType::INVALID, startLoc);
            diagnostics_.reportLexError(err.location, "Unterminated character literal");
            return err;
        }
        char esc = advance();
        if (!isValidEscapeChar(esc)) {
            diagnostics_.reportLexError(startLoc,
                std::string("Invalid escape sequence '\\") + esc + "' in character literal");
            valid = false;
        }
    } else {
        advance();
    }

    if (isAtEnd() || peek() != '\'') {
        while (!isAtEnd() && peek() != '\'' && peek() != '\n' && peek() != ';') {
            advance();
        }
        if (!isAtEnd() && peek() == '\'') advance();
        Token err = makeToken(TokenType::INVALID, startLoc);
        diagnostics_.reportLexError(err.location, "Unterminated or multi-character literal");
        return err;
    }

    advance();
    return makeToken(valid ? TokenType::CHAR_LITERAL : TokenType::INVALID, startLoc);
}

TokenType Lexer::lookupKeyword(const std::string& lexeme) noexcept {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"array",    TokenType::KW_ARRAY},
        {"auto",     TokenType::KW_AUTO},
        {"boolean",  TokenType::KW_BOOLEAN},
        {"char",     TokenType::KW_CHAR},
        {"else",     TokenType::KW_ELSE},
        {"false",    TokenType::KW_FALSE},
        {"for",      TokenType::KW_FOR},
        {"function", TokenType::KW_FUNCTION},
        {"if",       TokenType::KW_IF},
        {"integer",  TokenType::KW_INTEGER},
        {"print",    TokenType::KW_PRINT},
        {"return",   TokenType::KW_RETURN},
        {"string",   TokenType::KW_STRING},
        {"true",     TokenType::KW_TRUE},
        {"void",     TokenType::KW_VOID},
        {"while",    TokenType::KW_WHILE}
    };
    auto it = keywords.find(lexeme);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

}