#pragma once

#include "ast/AST.hpp"
#include "common/Diagnostics.hpp"
#include "lexer/Token.hpp"
#include <stdexcept>
#include <vector>

namespace bminor {

class Parser {
public:
    Parser(const std::vector<Token>& allTokens, DiagnosticEngine& diagnostics);

    std::unique_ptr<Program> parseProgram();

private:
    struct ParseError : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    std::vector<Token> tokens_;
    DiagnosticEngine& diagnostics_;
    std::size_t current_ = 0;

    const Token& peek(std::size_t lookahead = 0) const noexcept;
    const Token& previous() const noexcept;
    bool isAtEnd() const noexcept;
    bool check(TokenType type, std::size_t lookahead = 0) const noexcept;
    const Token& advance() noexcept;
    bool match(TokenType type) noexcept;
    const Token& consume(TokenType type, const std::string& errorMessage);

    void synchronize();
    bool isStatementSyncPoint(TokenType type) const noexcept;

    DeclPtr parseDeclaration();
    TypeNodePtr parseType();
    std::vector<Param> parseParamList();
    Param parseParam();

    StmtPtr parseStatement();
    std::unique_ptr<BlockStmt> parseBlock();
    StmtPtr parseIfStatement();
    StmtPtr parseForStatement();
    StmtPtr parseWhileStatement();
    StmtPtr parseReturnStatement();
    StmtPtr parsePrintStatement();
    StmtPtr parseExprStatement();

    ExprPtr parseInitializer();
    ExprPtr parseExpression();
    ExprPtr parseAssignment();
    ExprPtr parseLogicalOr();
    ExprPtr parseLogicalAnd();
    ExprPtr parseEquality();
    ExprPtr parseRelational();
    ExprPtr parseAdditive();
    ExprPtr parseMultiplicative();
    ExprPtr parseExponentiation();
    ExprPtr parseUnary();
    ExprPtr parsePostfix();
    ExprPtr parsePrimary();

    static char decodeCharLiteral(const std::string& raw);
    static std::string decodeStringLiteral(const std::string& raw);
};

}