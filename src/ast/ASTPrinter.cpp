#include "ast/ASTPrinter.hpp"

namespace bminor {

void ASTPrinter::writeIndent() {
    for (int i = 0; i < indentLevel_; ++i) out_ << "  ";
}

void ASTPrinter::print(Program& root) {
    root.accept(*this);
}

void ASTPrinter::visit(Program& node) {
    out_ << "Program (" << node.loc.toString() << ")\n";
    ++indentLevel_;
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(TypeNode& node) {
    writeIndent();
    out_ << "TypeNode: ";
    switch (node.kind) {
        case TypeKind::Integer:  out_ << "integer\n"; break;
        case TypeKind::Boolean:  out_ << "boolean\n"; break;
        case TypeKind::Char:     out_ << "char\n"; break;
        case TypeKind::String:   out_ << "string\n"; break;
        case TypeKind::Void:     out_ << "void\n"; break;
        case TypeKind::Auto:     out_ << "auto\n"; break;
        case TypeKind::Array:
            out_ << "array\n";
            ++indentLevel_;
            if (node.arraySize) {
                writeIndent();
                out_ << "[Size]\n";
                ++indentLevel_;
                node.arraySize->accept(*this);
                --indentLevel_;
            }
            if (node.subtype) node.subtype->accept(*this);
            --indentLevel_;
            break;
        case TypeKind::Function:
            out_ << "function\n";
            ++indentLevel_;
            writeIndent();
            out_ << "[ReturnType]\n";
            ++indentLevel_;
            if (node.subtype) node.subtype->accept(*this);
            --indentLevel_;
            writeIndent();
            out_ << "[Params: " << node.params.size() << "]\n";
            ++indentLevel_;
            for (auto& p : node.params) {
                writeIndent();
                out_ << "Param '" << p.name << "'\n";
                ++indentLevel_;
                if (p.type) p.type->accept(*this);
                --indentLevel_;
            }
            --indentLevel_;
            --indentLevel_;
            break;
    }
}

void ASTPrinter::visit(VarDecl& node) {
    writeIndent();
    out_ << "VarDecl '" << node.name << "' (" << node.loc.toString() << ")\n";
    ++indentLevel_;
    if (node.type) node.type->accept(*this);
    if (node.initializer) {
        writeIndent();
        out_ << "[Initializer]\n";
        ++indentLevel_;
        node.initializer->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(FuncDecl& node) {
    writeIndent();
    out_ << "FuncDecl '" << node.name << "' ("
         << (node.isPrototype() ? "prototype" : "definition")
         << ", " << node.loc.toString() << ")\n";
    ++indentLevel_;
    if (node.signature) node.signature->accept(*this);
    if (node.body) node.body->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(BlockStmt& node) {
    writeIndent();
    out_ << "BlockStmt (" << node.statements.size() << " stmts)\n";
    ++indentLevel_;
    for (auto& s : node.statements) {
        if (s) s->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(IfStmt& node) {
    writeIndent();
    out_ << "IfStmt (" << node.loc.toString() << ")\n";
    ++indentLevel_;
    if (node.condition) node.condition->accept(*this);
    if (node.thenBranch) node.thenBranch->accept(*this);
    if (node.elseBranch) {
        writeIndent();
        out_ << "[Else]\n";
        ++indentLevel_;
        node.elseBranch->accept(*this);
        --indentLevel_;
    }
    --indentLevel_;
}

void ASTPrinter::visit(ForStmt& node) {
    writeIndent();
    out_ << "ForStmt (" << node.loc.toString() << ")\n";
    ++indentLevel_;
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.update) node.update->accept(*this);
    if (node.body) node.body->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(WhileStmt& node) {
    writeIndent();
    out_ << "WhileStmt (" << node.loc.toString() << ")\n";
    ++indentLevel_;
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(ReturnStmt& node) {
    writeIndent();
    out_ << "ReturnStmt (" << node.loc.toString() << ")\n";
    if (node.value) {
        ++indentLevel_;
        node.value->accept(*this);
        --indentLevel_;
    }
}

void ASTPrinter::visit(PrintStmt& node) {
    writeIndent();
    out_ << "PrintStmt (" << node.expressions.size() << " exprs)\n";
    ++indentLevel_;
    for (auto& e : node.expressions) {
        if (e) e->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(ExprStmt& node) {
    writeIndent();
    out_ << "ExprStmt\n";
    ++indentLevel_;
    if (node.expression) node.expression->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(AssignExpr& node) {
    writeIndent();
    out_ << "AssignExpr '='\n";
    ++indentLevel_;
    if (node.target) node.target->accept(*this);
    if (node.value) node.value->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(BinaryExpr& node) {
    writeIndent();
    out_ << "BinaryExpr '" << node.opLexeme << "'\n";
    ++indentLevel_;
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(UnaryExpr& node) {
    writeIndent();
    out_ << "UnaryExpr '" << node.opLexeme << "'\n";
    ++indentLevel_;
    if (node.operand) node.operand->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(PostfixExpr& node) {
    writeIndent();
    out_ << "PostfixExpr '" << node.opLexeme << "'\n";
    ++indentLevel_;
    if (node.operand) node.operand->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(CallExpr& node) {
    writeIndent();
    out_ << "CallExpr (" << node.args.size() << " args)\n";
    ++indentLevel_;
    if (node.callee) node.callee->accept(*this);
    for (auto& arg : node.args) {
        if (arg) arg->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(ArrayIndexExpr& node) {
    writeIndent();
    out_ << "ArrayIndexExpr\n";
    ++indentLevel_;
    if (node.array) node.array->accept(*this);
    if (node.index) node.index->accept(*this);
    --indentLevel_;
}

void ASTPrinter::visit(ArrayInitExpr& node) {
    writeIndent();
    out_ << "ArrayInitExpr (" << node.elements.size() << " items)\n";
    ++indentLevel_;
    for (auto& el : node.elements) {
        if (el) el->accept(*this);
    }
    --indentLevel_;
}

void ASTPrinter::visit(IntLiteral& node) {
    writeIndent();
    out_ << "IntLiteral: " << node.value << "\n";
}

void ASTPrinter::visit(BoolLiteral& node) {
    writeIndent();
    out_ << "BoolLiteral: " << (node.value ? "true" : "false") << "\n";
}

void ASTPrinter::visit(CharLiteral& node) {
    writeIndent();
    out_ << "CharLiteral: " << node.rawLexeme << "\n";
}

void ASTPrinter::visit(StringLiteral& node) {
    writeIndent();
    out_ << "StringLiteral: " << node.rawLexeme << "\n";
}

void ASTPrinter::visit(Identifier& node) {
    writeIndent();
    out_ << "Identifier: " << node.name << "\n";
}

}