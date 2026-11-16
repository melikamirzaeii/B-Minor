#pragma once

#include "SourceLocation.hpp"
#include <ostream>
#include <string>
#include <vector>

namespace bminor {

enum class DiagnosticSeverity {
    Warning,
    Error
};

enum class DiagnosticPhase {
    Lexer,
    Parser,
    Semantic
};

struct Diagnostic {
    DiagnosticSeverity severity;
    DiagnosticPhase phase;
    SourceLocation location;
    std::string message;
};

class DiagnosticEngine {
public:
    void report(DiagnosticSeverity severity, DiagnosticPhase phase,
                const SourceLocation& loc, const std::string& message);

    void reportLexError(const SourceLocation& loc, const std::string& message);
    void reportParseError(const SourceLocation& loc, const std::string& message);

    bool hasErrors() const noexcept { return errorCount_ > 0; }
    std::size_t errorCount() const noexcept { return errorCount_; }
    const std::vector<Diagnostic>& diagnostics() const noexcept { return diagnostics_; }

    void printAll(std::ostream& out) const;
    void clear();

private:
    std::vector<Diagnostic> diagnostics_;
    std::size_t errorCount_ = 0;
};

}