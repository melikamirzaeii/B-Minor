#include "parser/Parser.hpp"
#include <cstdlib>

namespace bminor {

Parser::Parser(const std::vector<Token>& allTokens, DiagnosticEngine& diagnostics)
    : diagnostics_(diagnostics) {
    tokens_.reserve(allTokens.size());
    for (const auto& tok : allTokens) {
        if (!tok.isTrivia()) {
            tokens_.push_back(tok);
        }
    }
    if (tokens_.empty() || tokens_.back().type != TokenType::END_OF_FILE) {
        tokens_.push_back(Token{TokenType::END_OF_FILE, "", SourceLocation{}});
    }
}

const Token& Parser::peek(std::size_t lookahead) const noexcept {
    if (current_ + lookahead >= tokens_.size()) return tokens_.back();
    return tokens_[current_ + lookahead];
}

const Token& Parser::previous() const noexcept {
    return tokens_[current_ > 0 ? current_ - 1 : 0];
}

bool Parser::isAtEnd() const noexcept {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type, std::size_t lookahead) const noexcept {
    return peek(lookahead).type == type;
}

const Token& Parser::advance() noexcept {
    if (!isAtEnd()) ++current_;
    return previous();
}

bool Parser::match(TokenType type) noexcept {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    diagnostics_.reportParseError(peek().location, errorMessage + " (found '" + peek().lexeme + "')");
    throw ParseError(errorMessage);
}

bool Parser::isStatementSyncPoint(TokenType type) const noexcept {
    switch (type) {
        case TokenType::KW_IF:
        case TokenType::KW_FOR:
        case TokenType::KW_WHILE:
        case TokenType::KW_RETURN:
        case TokenType::KW_PRINT:
        case TokenType::KW_FUNCTION:
        case TokenType::DELIM_RBRACE:
            return true;
        default:
            return false;
    }
}

void Parser::synchronize() {
    if (!isAtEnd()) advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::DELIM_SEMICOLON) return;
        if (isStatementSyncPoint(peek().type)) return;
        if (check(TokenType::IDENTIFIER, 0) && check(TokenType::DELIM_COLON, 1)) return;
        advance();
    }
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto prog = std::make_unique<Program>(peek().location);
    while (!isAtEnd()) {
        try {
            DeclPtr decl = parseDeclaration();
            if (decl) prog->declarations.push_back(std::move(decl));
        } catch (const ParseError&) {
            synchronize();
        }
    }
    return prog;
}

DeclPtr Parser::parseDeclaration() {
    const Token& nameTok = consume(TokenType::IDENTIFIER, "Expected declaration identifier");
    consume(TokenType::DELIM_COLON, "Expected ':' after identifier '" + nameTok.lexeme + "'");

    TypeNodePtr type = parseType();

    if (type && type->kind == TypeKind::Function) {
        if (match(TokenType::DELIM_SEMICOLON)) {
            return std::make_unique<FuncDecl>(nameTok.location, nameTok.lexeme, nameTok.location,
                                              std::move(type), nullptr);
        }
        consume(TokenType::OP_ASSIGN, "Expected '=' or ';' after function signature");
        auto body = parseBlock();
        return std::make_unique<FuncDecl>(nameTok.location, nameTok.lexeme, nameTok.location,
                                          std::move(type), std::move(body));
    }

    ExprPtr init = nullptr;
    if (match(TokenType::OP_ASSIGN)) {
        init = parseInitializer();
    }
    consume(TokenType::DELIM_SEMICOLON, "Expected ';' after variable declaration");
    return std::make_unique<VarDecl>(nameTok.location, nameTok.lexeme, nameTok.location,
                                     std::move(type), std::move(init));
}

TypeNodePtr Parser::parseType() {
    const Token& tok = peek();
    if (match(TokenType::KW_INTEGER)) return std::make_unique<TypeNode>(tok.location, TypeKind::Integer);
    if (match(TokenType::KW_BOOLEAN)) return std::make_unique<TypeNode>(tok.location, TypeKind::Boolean);
    if (match(TokenType::KW_CHAR))    return std::make_unique<TypeNode>(tok.location, TypeKind::Char);
    if (match(TokenType::KW_STRING))  return std::make_unique<TypeNode>(tok.location, TypeKind::String);
    if (match(TokenType::KW_VOID))    return std::make_unique<TypeNode>(tok.location, TypeKind::Void);
    if (match(TokenType::KW_AUTO))    return std::make_unique<TypeNode>(tok.location, TypeKind::Auto);

    if (match(TokenType::KW_ARRAY)) {
        auto arrType = std::make_unique<TypeNode>(tok.location, TypeKind::Array);
        consume(TokenType::DELIM_LBRACKET, "Expected '[' after 'array'");
        if (!check(TokenType::DELIM_RBRACKET)) {
            arrType->arraySize = parseExpression();
        }
        consume(TokenType::DELIM_RBRACKET, "Expected ']' after array size");
        arrType->subtype = parseType();
        return arrType;
    }

    if (match(TokenType::KW_FUNCTION)) {
        auto fnType = std::make_unique<TypeNode>(tok.location, TypeKind::Function);
        fnType->subtype = parseType();
        consume(TokenType::DELIM_LPAREN, "Expected '(' for function parameter list");
        if (!check(TokenType::DELIM_RPAREN)) {
            fnType->params = parseParamList();
        }
        consume(TokenType::DELIM_RPAREN, "Expected ')' after function parameter list");
        return fnType;
    }

    diagnostics_.reportParseError(tok.location, "Expected type specifier");
    throw ParseError("Expected type specifier");
}

std::vector<Param> Parser::parseParamList() {
    std::vector<Param> params;
    params.push_back(parseParam());
    while (match(TokenType::DELIM_COMMA)) {
        params.push_back(parseParam());
    }
    return params;
}

Param Parser::parseParam() {
    const Token& nameTok = consume(TokenType::IDENTIFIER, "Expected parameter name");
    consume(TokenType::DELIM_COLON, "Expected ':' after parameter name");
    TypeNodePtr paramType = parseType();
    return Param{nameTok.location, nameTok.lexeme, nameTok.location, std::move(paramType)};
}

StmtPtr Parser::parseStatement() {
    if (check(TokenType::IDENTIFIER, 0) && check(TokenType::DELIM_COLON, 1)) {
        return parseDeclaration();
    }
    if (check(TokenType::DELIM_LBRACE)) return parseBlock();
    if (match(TokenType::KW_IF))        return parseIfStatement();
    if (match(TokenType::KW_FOR))       return parseForStatement();
    if (match(TokenType::KW_WHILE))     return parseWhileStatement();
    if (match(TokenType::KW_RETURN))    return parseReturnStatement();
    if (match(TokenType::KW_PRINT))     return parsePrintStatement();

    return parseExprStatement();
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    const Token& lbrace = consume(TokenType::DELIM_LBRACE, "Expected '{'");
    std::vector<StmtPtr> stmts;
    while (!check(TokenType::DELIM_RBRACE) && !isAtEnd()) {
        try {
            stmts.push_back(parseStatement());
        } catch (const ParseError&) {
            synchronize();
        }
    }
    consume(TokenType::DELIM_RBRACE, "Expected '}' to close block");
    return std::make_unique<BlockStmt>(lbrace.location, std::move(stmts));
}

StmtPtr Parser::parseIfStatement() {
    SourceLocation loc = previous().location;
    consume(TokenType::DELIM_LPAREN, "Expected '(' after 'if'");
    ExprPtr cond = parseExpression();
    consume(TokenType::DELIM_RPAREN, "Expected ')' after if condition");
    StmtPtr thenBranch = parseStatement();
    StmtPtr elseBranch = nullptr;
    if (match(TokenType::KW_ELSE)) {
        elseBranch = parseStatement();
    }
    return std::make_unique<IfStmt>(loc, std::move(cond), std::move(thenBranch), std::move(elseBranch));
}

StmtPtr Parser::parseForStatement() {
    SourceLocation loc = previous().location;
    consume(TokenType::DELIM_LPAREN, "Expected '(' after 'for'");

    ExprPtr init = nullptr;
    if (!check(TokenType::DELIM_SEMICOLON)) {
        init = parseExpression();
    }
    consume(TokenType::DELIM_SEMICOLON, "Expected ';' after for-loop initializer");

    ExprPtr cond = nullptr;
    if (!check(TokenType::DELIM_SEMICOLON)) {
        cond = parseExpression();
    }
    consume(TokenType::DELIM_SEMICOLON, "Expected ';' after for-loop condition");

    ExprPtr update = nullptr;
    if (!check(TokenType::DELIM_RPAREN)) {
        update = parseExpression();
    }
    consume(TokenType::DELIM_RPAREN, "Expected ')' after for-loop clauses");

    StmtPtr body = parseStatement();
    return std::make_unique<ForStmt>(loc, std::move(init), std::move(cond),
                                     std::move(update), std::move(body));
}

StmtPtr Parser::parseWhileStatement() {
    SourceLocation loc = previous().location;
    consume(TokenType::DELIM_LPAREN, "Expected '(' after 'while'");
    ExprPtr cond = parseExpression();
    consume(TokenType::DELIM_RPAREN, "Expected ')' after while condition");
    StmtPtr body = parseStatement();
    return std::make_unique<WhileStmt>(loc, std::move(cond), std::move(body));
}

StmtPtr Parser::parseReturnStatement() {
    SourceLocation loc = previous().location;
    ExprPtr val = nullptr;
    if (!check(TokenType::DELIM_SEMICOLON)) {
        val = parseExpression();
    }
    consume(TokenType::DELIM_SEMICOLON, "Expected ';' after return statement");
    return std::make_unique<ReturnStmt>(loc, std::move(val));
}

StmtPtr Parser::parsePrintStatement() {
    SourceLocation loc = previous().location;
    std::vector<ExprPtr> exprs;
    if (!check(TokenType::DELIM_SEMICOLON)) {
        exprs.push_back(parseExpression());
        while (match(TokenType::DELIM_COMMA)) {
            exprs.push_back(parseExpression());
        }
    }
    consume(TokenType::DELIM_SEMICOLON, "Expected ';' after print statement");
    return std::make_unique<PrintStmt>(loc, std::move(exprs));
}

StmtPtr Parser::parseExprStatement() {
    SourceLocation loc = peek().location;
    ExprPtr expr = parseExpression();
    consume(TokenType::DELIM_SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExprStmt>(loc, std::move(expr));
}

ExprPtr Parser::parseInitializer() {
    if (check(TokenType::DELIM_LBRACE)) {
        const Token& lbrace = advance();
        std::vector<ExprPtr> elems;
        if (!check(TokenType::DELIM_RBRACE)) {
            elems.push_back(parseInitializer());
            while (match(TokenType::DELIM_COMMA)) {
                elems.push_back(parseInitializer());
            }
        }
        consume(TokenType::DELIM_RBRACE, "Expected '}' after array initializer");
        return std::make_unique<ArrayInitExpr>(lbrace.location, std::move(elems));
    }
    return parseExpression();
}

ExprPtr Parser::parseExpression() {
    return parseAssignment();
}

ExprPtr Parser::parseAssignment() {
    ExprPtr expr = parseLogicalOr();
    if (match(TokenType::OP_ASSIGN)) {
        SourceLocation opLoc = previous().location;
        ExprPtr rhs = parseAssignment();
        return std::make_unique<AssignExpr>(opLoc, std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseLogicalOr() {
    ExprPtr expr = parseLogicalAnd();
    while (match(TokenType::OP_OR)) {
        const Token& op = previous();
        ExprPtr rhs = parseLogicalAnd();
        expr = std::make_unique<BinaryExpr>(op.location, op.type, op.lexeme,
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseLogicalAnd() {
    ExprPtr expr = parseEquality();
    while (match(TokenType::OP_AND)) {
        const Token& op = previous();
        ExprPtr rhs = parseEquality();
        expr = std::make_unique<BinaryExpr>(op.location, op.type, op.lexeme,
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseEquality() {
    ExprPtr expr = parseRelational();
    while (match(TokenType::OP_EQ) || match(TokenType::OP_NEQ)) {
        const Token& op = previous();
        ExprPtr rhs = parseRelational();
        expr = std::make_unique<BinaryExpr>(op.location, op.type, op.lexeme,
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseRelational() {
    ExprPtr expr = parseAdditive();
    while (match(TokenType::OP_LT) || match(TokenType::OP_LTE) ||
           match(TokenType::OP_GT) || match(TokenType::OP_GTE)) {
        const Token& op = previous();
        ExprPtr rhs = parseAdditive();
        expr = std::make_unique<BinaryExpr>(op.location, op.type, op.lexeme,
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseAdditive() {
    ExprPtr expr = parseMultiplicative();
    while (match(TokenType::OP_PLUS) || match(TokenType::OP_MINUS)) {
        const Token& op = previous();
        ExprPtr rhs = parseMultiplicative();
        expr = std::make_unique<BinaryExpr>(op.location, op.type, op.lexeme,
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseMultiplicative() {
    ExprPtr expr = parseExponentiation();
    while (match(TokenType::OP_STAR) || match(TokenType::OP_SLASH) || match(TokenType::OP_PERCENT)) {
        const Token& op = previous();
        ExprPtr rhs = parseExponentiation();
        expr = std::make_unique<BinaryExpr>(op.location, op.type, op.lexeme,
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseExponentiation() {
    ExprPtr expr = parseUnary();
    if (match(TokenType::OP_CARET)) {
        const Token& op = previous();
        ExprPtr rhs = parseExponentiation();
        return std::make_unique<BinaryExpr>(op.location, op.type, op.lexeme,
                                            std::move(expr), std::move(rhs));
    }
    return expr;
}

ExprPtr Parser::parseUnary() {
    if (match(TokenType::OP_MINUS) || match(TokenType::OP_NOT)) {
        const Token& op = previous();
        ExprPtr operand = parseUnary();
        return std::make_unique<UnaryExpr>(op.location, op.type, op.lexeme, std::move(operand));
    }
    return parsePostfix();
}

ExprPtr Parser::parsePostfix() {
    ExprPtr expr = parsePrimary();

    while (true) {
        if (match(TokenType::OP_INC) || match(TokenType::OP_DEC)) {
            const Token& op = previous();
            expr = std::make_unique<PostfixExpr>(op.location, op.type, op.lexeme, std::move(expr));
        } else if (match(TokenType::DELIM_LBRACKET)) {
            SourceLocation loc = previous().location;
            ExprPtr index = parseExpression();
            consume(TokenType::DELIM_RBRACKET, "Expected ']' after array index");
            expr = std::make_unique<ArrayIndexExpr>(loc, std::move(expr), std::move(index));
        } else if (match(TokenType::DELIM_LPAREN)) {
            SourceLocation loc = previous().location;
            std::vector<ExprPtr> args;
            if (!check(TokenType::DELIM_RPAREN)) {
                args.push_back(parseExpression());
                while (match(TokenType::DELIM_COMMA)) {
                    args.push_back(parseExpression());
                }
            }
            consume(TokenType::DELIM_RPAREN, "Expected ')' after function arguments");
            expr = std::make_unique<CallExpr>(loc, std::move(expr), std::move(args));
        } else {
            break;
        }
    }

    return expr;
}

ExprPtr Parser::parsePrimary() {
    const Token& tok = peek();

    if (match(TokenType::INT_LITERAL)) {
        long long val = std::strtoll(tok.lexeme.c_str(), nullptr, 10);
        return std::make_unique<IntLiteral>(tok.location, static_cast<std::int64_t>(val), tok.lexeme);
    }
    if (match(TokenType::KW_TRUE)) {
        return std::make_unique<BoolLiteral>(tok.location, true);
    }
    if (match(TokenType::KW_FALSE)) {
        return std::make_unique<BoolLiteral>(tok.location, false);
    }
    if (match(TokenType::CHAR_LITERAL)) {
        return std::make_unique<CharLiteral>(tok.location, decodeCharLiteral(tok.lexeme), tok.lexeme);
    }
    if (match(TokenType::STRING_LITERAL)) {
        return std::make_unique<StringLiteral>(tok.location, decodeStringLiteral(tok.lexeme), tok.lexeme);
    }
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<Identifier>(tok.location, tok.lexeme);
    }
    if (match(TokenType::DELIM_LPAREN)) {
        ExprPtr inner = parseExpression();
        consume(TokenType::DELIM_RPAREN, "Expected ')' after parenthesized expression");
        return inner;
    }

    diagnostics_.reportParseError(tok.location, "Expected expression (found '" + tok.lexeme + "')");
    throw ParseError("Expected expression");
}

char Parser::decodeCharLiteral(const std::string& raw) {
    if (raw.size() < 3) return '\0';
    if (raw[1] != '\\') return raw[1];
    switch (raw[2]) {
        case 'n':  return '\n';
        case 't':  return '\t';
        case 'r':  return '\r';
        case '\\': return '\\';
        case '\'': return '\'';
        case '"':  return '"';
        case '0':  return '\0';
        default:   return raw[2];
    }
}

std::string Parser::decodeStringLiteral(const std::string& raw) {
    std::string out;
    if (raw.size() < 2) return out;
    for (std::size_t i = 1; i + 1 < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size() - 1) {
            ++i;
            switch (raw[i]) {
                case 'n':  out.push_back('\n'); break;
                case 't':  out.push_back('\t'); break;
                case 'r':  out.push_back('\r'); break;
                case '\\': out.push_back('\\'); break;
                case '"':  out.push_back('"');  break;
                case '\'': out.push_back('\''); break;
                case '0':  out.push_back('\0'); break;
                default:   out.push_back(raw[i]); break;
            }
        } else {
            out.push_back(raw[i]);
        }
    }
    return out;
}

}