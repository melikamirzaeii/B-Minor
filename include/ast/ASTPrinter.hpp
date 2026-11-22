#pragma once

#include "ast/AST.hpp"
#include <ostream>
#include <string>

namespace bminor {

class ASTPrinter : public ASTVisitor {
public:
    explicit ASTPrinter(std::ostream& out) : out_(out) {}

    void print(Program& root);

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
    std::ostream& out_;
    int indentLevel_ = 0;

    void writeIndent();
};

}