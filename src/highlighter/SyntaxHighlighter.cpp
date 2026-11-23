#include "highlighter/SyntaxHighlighter.hpp"

namespace bminor {

std::vector<StyledSpan> SyntaxHighlighter::highlight(const std::vector<Token>& allTokens,
                                                     Program* astRoot) {
    astRoles_.clear();
    inCallCallee_ = false;

    if (astRoot) {
        astRoot->accept(*this);
    }

    std::vector<StyledSpan> spans;
    spans.reserve(allTokens.size());

    for (const auto& tok : allTokens) {
        if (tok.type == TokenType::END_OF_FILE) continue;
        HighlightCategory cat = classifyToken(tok);
        spans.push_back(StyledSpan{cat, tok.lexeme});
    }
    return spans;
}

HighlightCategory SyntaxHighlighter::classifyToken(const Token& token) const {
    if (token.type == TokenType::INVALID) {
        return HighlightCategory::Error;
    }
    if (token.type == TokenType::WHITESPACE) {
        return HighlightCategory::Whitespace;
    }
    if (token.type == TokenType::LINE_COMMENT || token.type == TokenType::BLOCK_COMMENT) {
        return HighlightCategory::Comment;
    }

    auto it = astRoles_.find(token.location.offset);
    if (it != astRoles_.end()) {
        return it->second;
    }

    if (token.isTypeKeyword()) {
        return HighlightCategory::TypeName;
    }
    if (token.type == TokenType::KW_TRUE || token.type == TokenType::KW_FALSE) {
        return HighlightCategory::BoolLiteral;
    }
    if (token.isKeyword()) {
        return HighlightCategory::Keyword;
    }
    if (token.type == TokenType::INT_LITERAL) {
        return HighlightCategory::NumberLiteral;
    }
    if (token.type == TokenType::STRING_LITERAL || token.type == TokenType::CHAR_LITERAL) {
        return HighlightCategory::StringLiteral;
    }
    if (token.type == TokenType::IDENTIFIER) {
        return HighlightCategory::VariableIdent;
    }
    if (token.type >= TokenType::OP_PLUS && token.type <= TokenType::OP_NOT) {
        return HighlightCategory::Operator;
    }
    return HighlightCategory::Delimiter;
}

void SyntaxHighlighter::visit(Program& node) {
    for (auto& d : node.declarations) {
        if (d) d->accept(*this);
    }
}

void SyntaxHighlighter::visit(TypeNode& node) {
    if (node.arraySize) node.arraySize->accept(*this);
    if (node.subtype) node.subtype->accept(*this);
    for (auto& param : node.params) {
        astRoles_[param.nameLoc.offset] = HighlightCategory::VariableIdent;
        if (param.type) param.type->accept(*this);
    }
}

void SyntaxHighlighter::visit(VarDecl& node) {
    astRoles_[node.nameLoc.offset] = HighlightCategory::VariableIdent;
    if (node.type) node.type->accept(*this);
    if (node.initializer) node.initializer->accept(*this);
}

void SyntaxHighlighter::visit(FuncDecl& node) {
    astRoles_[node.nameLoc.offset] = HighlightCategory::FunctionName;
    if (node.signature) node.signature->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SyntaxHighlighter::visit(BlockStmt& node) {
    for (auto& s : node.statements) {
        if (s) s->accept(*this);
    }
}

void SyntaxHighlighter::visit(IfStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.thenBranch) node.thenBranch->accept(*this);
    if (node.elseBranch) node.elseBranch->accept(*this);
}

void SyntaxHighlighter::visit(ForStmt& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.update) node.update->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SyntaxHighlighter::visit(WhileStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SyntaxHighlighter::visit(ReturnStmt& node) {
    if (node.value) node.value->accept(*this);
}

void SyntaxHighlighter::visit(PrintStmt& node) {
    for (auto& e : node.expressions) {
        if (e) e->accept(*this);
    }
}

void SyntaxHighlighter::visit(ExprStmt& node) {
    if (node.expression) node.expression->accept(*this);
}

void SyntaxHighlighter::visit(AssignExpr& node) {
    if (node.target) node.target->accept(*this);
    if (node.value) node.value->accept(*this);
}

void SyntaxHighlighter::visit(BinaryExpr& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
}

void SyntaxHighlighter::visit(UnaryExpr& node) {
    if (node.operand) node.operand->accept(*this);
}

void SyntaxHighlighter::visit(PostfixExpr& node) {
    if (node.operand) node.operand->accept(*this);
}

void SyntaxHighlighter::visit(CallExpr& node) {
    if (node.callee) {
        bool prev = inCallCallee_;
        inCallCallee_ = true;
        node.callee->accept(*this);
        inCallCallee_ = prev;
    }
    for (auto& arg : node.args) {
        if (arg) arg->accept(*this);
    }
}

void SyntaxHighlighter::visit(ArrayIndexExpr& node) {
    if (node.array) node.array->accept(*this);
    if (node.index) node.index->accept(*this);
}

void SyntaxHighlighter::visit(ArrayInitExpr& node) {
    for (auto& el : node.elements) {
        if (el) el->accept(*this);
    }
}

void SyntaxHighlighter::visit(IntLiteral&) {}
void SyntaxHighlighter::visit(BoolLiteral&) {}
void SyntaxHighlighter::visit(CharLiteral&) {}
void SyntaxHighlighter::visit(StringLiteral&) {}

void SyntaxHighlighter::visit(Identifier& node) {
    astRoles_[node.loc.offset] =
        inCallCallee_ ? HighlightCategory::FunctionName : HighlightCategory::VariableIdent;
}

}