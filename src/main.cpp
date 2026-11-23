#include "ast/ASTPrinter.hpp"
#include "common/Diagnostics.hpp"
#include "highlighter/ANSIRenderer.hpp"
#include "highlighter/HTMLRenderer.hpp"
#include "highlighter/SyntaxHighlighter.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void printUsage(const char* progName) {
    std::cerr << "Usage:\n"
              << "  " << progName << " <input.bminor> --ansi\n"
              << "  " << progName << " <input.bminor> --html <output.html>\n"
              << "  " << progName << " <input.bminor> --tokens\n"
              << "  " << progName << " <input.bminor> --ast\n";
}

bool readFile(const std::string& path, std::string& content) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) return false;
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    return true;
}

}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputPath = argv[1];
    std::string mode = argv[2];
    std::string outputPath;

    if (mode == "--html") {
        if (argc < 4) {
            std::cerr << "Error: --html requires an output file path.\n";
            printUsage(argv[0]);
            return 1;
        }
        outputPath = argv[3];
    } else if (mode != "--ansi" && mode != "--tokens" && mode != "--ast") {
        std::cerr << "Error: Unknown flag '" << mode << "'.\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string source;
    if (!readFile(inputPath, source)) {
        std::cerr << "Error: Could not open file '" << inputPath << "'.\n";
        return 1;
    }

    bminor::DiagnosticEngine diagnostics;

    bminor::Lexer lexer(source, inputPath, diagnostics);
    std::vector<bminor::Token> tokens = lexer.tokenizeAll();

    if (mode == "--tokens") {
        for (const auto& tok : tokens) {
            if (tok.isTrivia()) continue;
            std::cout << tok.location.toString() << "\t"
                      << bminor::tokenTypeToString(tok.type) << "\t"
                      << "'" << tok.lexeme << "'\n";
        }
        diagnostics.printAll(std::cerr);
        return diagnostics.hasErrors() ? 1 : 0;
    }

    bminor::Parser parser(tokens, diagnostics);
    std::unique_ptr<bminor::Program> ast = parser.parseProgram();

    if (mode == "--ast") {
        bminor::ASTPrinter printer(std::cout);
        printer.print(*ast);
        diagnostics.printAll(std::cerr);
        return diagnostics.hasErrors() ? 1 : 0;
    }

    bminor::SyntaxHighlighter highlighter;
    std::vector<bminor::StyledSpan> spans = highlighter.highlight(tokens, ast.get());

    if (mode == "--ansi") {
        bminor::ANSIRenderer::renderTo(std::cout, spans);
    } else if (mode == "--html") {
        std::ofstream outFile(outputPath, std::ios::out | std::ios::binary);
        if (!outFile) {
            std::cerr << "Error: Could not write to '" << outputPath << "'.\n";
            return 1;
        }
        bminor::HTMLRenderer::renderDocumentTo(outFile, inputPath, spans);
        std::cout << "Wrote highlighted HTML to " << outputPath << "\n";
    }

    diagnostics.printAll(std::cerr);
    return diagnostics.hasErrors() ? 1 : 0;
}