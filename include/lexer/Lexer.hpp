#pragma once

#include "common/Diagnostics.hpp"
#include "lexer/Token.hpp"
#include <string>
#include <vector>

namespace bminor {

class Lexer {
public:
    Lexer(std::string source, std::string filename, DiagnosticEngine& diagnostics);

    std::vector<Token> tokenizeAll();

private:
    std::string source_;
    std::string filename_;
    DiagnosticEngine& diagnostics_;

    std::size_t pos_ = 0;
    std::size_t line_ = 1;
    std::size_t column_ = 1;

    bool isAtEnd() const noexcept;
    char peek(std::size_t lookahead = 0) const noexcept;
    char advance() noexcept;
    bool match(char expected) noexcept;

    SourceLocation currentLocation() const;
    Token makeToken(TokenType type, const SourceLocation& startLoc) const;

    Token scanNextToken();
    Token scanWhitespace(const SourceLocation& startLoc);
    Token scanLineComment(const SourceLocation& startLoc);
    Token scanBlockComment(const SourceLocation& startLoc);
    Token scanIdentifierOrKeyword(const SourceLocation& startLoc);
    Token scanNumber(const SourceLocation& startLoc);
    Token scanStringLiteral(const SourceLocation& startLoc);
    Token scanCharLiteral(const SourceLocation& startLoc);

    static bool isValidEscapeChar(char c) noexcept;
    static TokenType lookupKeyword(const std::string& lexeme) noexcept;
};

}