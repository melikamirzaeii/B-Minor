#pragma once

#include "common/SourceLocation.hpp"
#include "lexer/Token.hpp"
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace bminor {

struct TypeInfo;
struct Symbol;

class ASTVisitor;

class ASTNode {
public:
    SourceLocation loc;

    std::shared_ptr<TypeInfo> typeAnnotation = nullptr;
    std::shared_ptr<Symbol> symbolAnnotation = nullptr;

    explicit ASTNode(SourceLocation location) : loc(std::move(location)) {}
    virtual ~ASTNode() = default;

    virtual void accept(ASTVisitor& visitor) = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

class Expr : public ASTNode {
public:
    using ASTNode::ASTNode;
};
using ExprPtr = std::unique_ptr<Expr>;

class Stmt : public ASTNode {
public:
    using ASTNode::ASTNode;
};
using StmtPtr = std::unique_ptr<Stmt>;

class Decl : public Stmt {
public:
    using Stmt::Stmt;
};
using DeclPtr = std::unique_ptr<Decl>;

struct Param;
class TypeNode;
class IntLiteral;
class BoolLiteral;
class CharLiteral;
class StringLiteral;
class Identifier;
class AssignExpr;
class BinaryExpr;
class UnaryExpr;
class PostfixExpr;
class CallExpr;
class ArrayIndexExpr;
class ArrayInitExpr;
class BlockStmt;
class IfStmt;
class ForStmt;
class WhileStmt;
class ReturnStmt;
class PrintStmt;
class ExprStmt;
class VarDecl;
class FuncDecl;
class Program;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(Program& node) = 0;
    virtual void visit(TypeNode& node) = 0;
    virtual void visit(VarDecl& node) = 0;
    virtual void visit(FuncDecl& node) = 0;

    virtual void visit(BlockStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(ForStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(ReturnStmt& node) = 0;
    virtual void visit(PrintStmt& node) = 0;
    virtual void visit(ExprStmt& node) = 0;

    virtual void visit(AssignExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(PostfixExpr& node) = 0;
    virtual void visit(CallExpr& node) = 0;
    virtual void visit(ArrayIndexExpr& node) = 0;
    virtual void visit(ArrayInitExpr& node) = 0;

    virtual void visit(IntLiteral& node) = 0;
    virtual void visit(BoolLiteral& node) = 0;
    virtual void visit(CharLiteral& node) = 0;
    virtual void visit(StringLiteral& node) = 0;
    virtual void visit(Identifier& node) = 0;
};

enum class TypeKind {
    Integer,
    Boolean,
    Char,
    String,
    Void,
    Auto,
    Array,
    Function
};

struct Param {
    SourceLocation loc;
    std::string name;
    SourceLocation nameLoc;
    std::unique_ptr<TypeNode> type;
};

class TypeNode : public ASTNode {
public:
    TypeKind kind;
    ExprPtr arraySize;
    std::unique_ptr<TypeNode> subtype;
    std::vector<Param> params;

    TypeNode(SourceLocation location, TypeKind k)
        : ASTNode(std::move(location)), kind(k) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};
using TypeNodePtr = std::unique_ptr<TypeNode>;

class IntLiteral : public Expr {
public:
    std::int64_t value;
    std::string rawLexeme;

    IntLiteral(SourceLocation location, std::int64_t val, std::string raw)
        : Expr(std::move(location)), value(val), rawLexeme(std::move(raw)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BoolLiteral : public Expr {
public:
    bool value;

    BoolLiteral(SourceLocation location, bool val)
        : Expr(std::move(location)), value(val) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CharLiteral : public Expr {
public:
    char value;
    std::string rawLexeme;

    CharLiteral(SourceLocation location, char val, std::string raw)
        : Expr(std::move(location)), value(val), rawLexeme(std::move(raw)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class StringLiteral : public Expr {
public:
    std::string value;
    std::string rawLexeme;

    StringLiteral(SourceLocation location, std::string val, std::string raw)
        : Expr(std::move(location)), value(std::move(val)), rawLexeme(std::move(raw)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class Identifier : public Expr {
public:
    std::string name;

    Identifier(SourceLocation location, std::string identName)
        : Expr(std::move(location)), name(std::move(identName)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class AssignExpr : public Expr {
public:
    ExprPtr target;
    ExprPtr value;

    AssignExpr(SourceLocation location, ExprPtr lhs, ExprPtr rhs)
        : Expr(std::move(location)), target(std::move(lhs)), value(std::move(rhs)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BinaryExpr : public Expr {
public:
    TokenType op;
    std::string opLexeme;
    ExprPtr left;
    ExprPtr right;

    BinaryExpr(SourceLocation location, TokenType oper, std::string lex, ExprPtr lhs, ExprPtr rhs)
        : Expr(std::move(location)), op(oper), opLexeme(std::move(lex)),
          left(std::move(lhs)), right(std::move(rhs)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class UnaryExpr : public Expr {
public:
    TokenType op;
    std::string opLexeme;
    ExprPtr operand;

    UnaryExpr(SourceLocation location, TokenType oper, std::string lex, ExprPtr expr)
        : Expr(std::move(location)), op(oper), opLexeme(std::move(lex)), operand(std::move(expr)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class PostfixExpr : public Expr {
public:
    TokenType op;
    std::string opLexeme;
    ExprPtr operand;

    PostfixExpr(SourceLocation location, TokenType oper, std::string lex, ExprPtr expr)
        : Expr(std::move(location)), op(oper), opLexeme(std::move(lex)), operand(std::move(expr)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class CallExpr : public Expr {
public:
    ExprPtr callee;
    std::vector<ExprPtr> args;

    CallExpr(SourceLocation location, ExprPtr fn, std::vector<ExprPtr> arguments)
        : Expr(std::move(location)), callee(std::move(fn)), args(std::move(arguments)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ArrayIndexExpr : public Expr {
public:
    ExprPtr array;
    ExprPtr index;

    ArrayIndexExpr(SourceLocation location, ExprPtr arr, ExprPtr idx)
        : Expr(std::move(location)), array(std::move(arr)), index(std::move(idx)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ArrayInitExpr : public Expr {
public:
    std::vector<ExprPtr> elements;

    ArrayInitExpr(SourceLocation location, std::vector<ExprPtr> elems)
        : Expr(std::move(location)), elements(std::move(elems)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class BlockStmt : public Stmt {
public:
    std::vector<StmtPtr> statements;

    BlockStmt(SourceLocation location, std::vector<StmtPtr> stmts)
        : Stmt(std::move(location)), statements(std::move(stmts)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class IfStmt : public Stmt {
public:
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;

    IfStmt(SourceLocation location, ExprPtr cond, StmtPtr thenB, StmtPtr elseB = nullptr)
        : Stmt(std::move(location)), condition(std::move(cond)),
          thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ForStmt : public Stmt {
public:
    ExprPtr init;
    ExprPtr condition;
    ExprPtr update;
    StmtPtr body;

    ForStmt(SourceLocation location, ExprPtr i, ExprPtr c, ExprPtr u, StmtPtr b)
        : Stmt(std::move(location)), init(std::move(i)), condition(std::move(c)),
          update(std::move(u)), body(std::move(b)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class WhileStmt : public Stmt {
public:
    ExprPtr condition;
    StmtPtr body;

    WhileStmt(SourceLocation location, ExprPtr cond, StmtPtr b)
        : Stmt(std::move(location)), condition(std::move(cond)), body(std::move(b)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ReturnStmt : public Stmt {
public:
    ExprPtr value;

    ReturnStmt(SourceLocation location, ExprPtr val = nullptr)
        : Stmt(std::move(location)), value(std::move(val)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class PrintStmt : public Stmt {
public:
    std::vector<ExprPtr> expressions;

    PrintStmt(SourceLocation location, std::vector<ExprPtr> exprs)
        : Stmt(std::move(location)), expressions(std::move(exprs)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class ExprStmt : public Stmt {
public:
    ExprPtr expression;

    ExprStmt(SourceLocation location, ExprPtr expr)
        : Stmt(std::move(location)), expression(std::move(expr)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class VarDecl : public Decl {
public:
    std::string name;
    SourceLocation nameLoc;
    TypeNodePtr type;
    ExprPtr initializer;

    VarDecl(SourceLocation location, std::string varName, SourceLocation nLoc,
            TypeNodePtr varType, ExprPtr init = nullptr)
        : Decl(std::move(location)), name(std::move(varName)), nameLoc(std::move(nLoc)),
          type(std::move(varType)), initializer(std::move(init)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class FuncDecl : public Decl {
public:
    std::string name;
    SourceLocation nameLoc;
    TypeNodePtr signature;
    std::unique_ptr<BlockStmt> body;

    FuncDecl(SourceLocation location, std::string fnName, SourceLocation nLoc,
             TypeNodePtr sig, std::unique_ptr<BlockStmt> fnBody = nullptr)
        : Decl(std::move(location)), name(std::move(fnName)), nameLoc(std::move(nLoc)),
          signature(std::move(sig)), body(std::move(fnBody)) {}

    bool isPrototype() const noexcept { return body == nullptr; }

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

class Program : public ASTNode {
public:
    std::vector<DeclPtr> declarations;

    explicit Program(SourceLocation location) : ASTNode(std::move(location)) {}

    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

}