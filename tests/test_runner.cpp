#include "common/Diagnostics.hpp"
#include "highlighter/ANSIRenderer.hpp"
#include "highlighter/HTMLRenderer.hpp"
#include "highlighter/SyntaxHighlighter.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

std::string loadFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    assert(ifs && "Failed to open test file");
    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

void testValidFactorial(const std::string& testDir) {
    std::string src = loadFile(testDir + "/valid/factorial.bminor");
    bminor::DiagnosticEngine diag;
    bminor::Lexer lexer(src, "factorial.bminor", diag);
    auto tokens = lexer.tokenizeAll();
    assert(!diag.hasErrors());

    bminor::Parser parser(tokens, diag);
    auto ast = parser.parseProgram();
    assert(!diag.hasErrors());
    assert(ast->declarations.size() == 2);

    bminor::SyntaxHighlighter highlighter;
    auto spans = highlighter.highlight(tokens, ast.get());

    std::string reconstructed;
    bool sawFunctionHighlight = false;
    for (const auto& s : spans) {
        reconstructed += s.text;
        if (s.category == bminor::HighlightCategory::FunctionName && s.text == "factorial") {
            sawFunctionHighlight = true;
        }
    }
    assert(reconstructed == src);
    assert(sawFunctionHighlight);

    std::string html = bminor::HTMLRenderer::renderDocument("factorial.bminor", spans);
    assert(html.find("<span class=\"fn\">factorial</span>") != std::string::npos);
    assert(html.find("<span class=\"type\">integer</span>") != std::string::npos);
}

void testErrorRecovery(const std::string& testDir) {
    std::string src = loadFile(testDir + "/invalid/errors.bminor");
    bminor::DiagnosticEngine diag;
    bminor::Lexer lexer(src, "errors.bminor", diag);
    auto tokens = lexer.tokenizeAll();
    assert(diag.hasErrors());

    bminor::Parser parser(tokens, diag);
    auto ast = parser.parseProgram();
    assert(ast != nullptr);
    assert(ast->declarations.size() == 1);

    bminor::SyntaxHighlighter highlighter;
    auto spans = highlighter.highlight(tokens, ast.get());
    std::string ansi = bminor::ANSIRenderer::render(spans);
    assert(ansi.find("\033[4;31m") != std::string::npos);
}

}

int main(int argc, char* argv[]) {
    std::string testDir = (argc > 1) ? argv[1] : "tests";
    testValidFactorial(testDir);
    testErrorRecovery(testDir);
    std::cout << "All Phase 1 B-Minor tests passed.\n";
    return 0;
}