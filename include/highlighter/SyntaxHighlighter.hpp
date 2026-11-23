#pragma once

#include "ast/AST.hpp"
#include "lexer/Token.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace bminor {

enum class HighlightCategory {
    Keyword,
    TypeName,
    VariableIdent,
    FunctionName,
    NumberLiteral,
    BoolLiteral,
    StringLiteral,
    Operator,
    Delimiter,
    Comment,
    Error,
    Whitespace
};

struct StyledSpan {
    HighlightCategory category;
    std::string text;
};

class SyntaxHighlighter : public ASTVisitor {
public:
    std::vector<StyledSpan> highlight(const std::vector<Token>& allTokens, Program* astRoot);

    void visit(Program& node) override;
    void visit(TypeNode& node) override;
    void visit(VarDecl& node) override;
    void visit(FuncDecl& node) override;
    void visit(BlockStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(ForStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(ReturnStmt& node) override;
    void visit(PrintStmt& node) override;
    void visit(ExprStmt& node) override;
    void visit(AssignExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(PostfixExpr& node) override;
    void visit(CallExpr& node) override;
    void visit(ArrayIndexExpr& node) override;
    void visit(ArrayInitExpr& node) override;
    void visit(IntLiteral& node) override;
    void visit(BoolLiteral& node) override;
    void visit(CharLiteral& node) override;
    void visit(StringLiteral& node) override;
    void visit(Identifier& node) override;

private:
    std::unordered_map<std::size_t, HighlightCategory> astRoles_;
    bool inCallCallee_ = false;

    HighlightCategory classifyToken(const Token& token) const;
};

}