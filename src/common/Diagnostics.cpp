#include "common/Diagnostics.hpp"

namespace bminor {

void DiagnosticEngine::report(DiagnosticSeverity severity, DiagnosticPhase phase,
                              const SourceLocation& loc, const std::string& message) {
    diagnostics_.push_back({severity, phase, loc, message});
    if (severity == DiagnosticSeverity::Error) {
        ++errorCount_;
    }
}

void DiagnosticEngine::reportLexError(const SourceLocation& loc, const std::string& message) {
    report(DiagnosticSeverity::Error, DiagnosticPhase::Lexer, loc, message);
}

void DiagnosticEngine::reportParseError(const SourceLocation& loc, const std::string& message) {
    report(DiagnosticSeverity::Error, DiagnosticPhase::Parser, loc, message);
}

void DiagnosticEngine::printAll(std::ostream& out) const {
    for (const auto& d : diagnostics_) {
        const char* phaseStr = (d.phase == DiagnosticPhase::Lexer)   ? "Lexical" :
                               (d.phase == DiagnosticPhase::Parser)  ? "Syntax"  : "Semantic";
        const char* sevStr   = (d.severity == DiagnosticSeverity::Error) ? "error" : "warning";
        out << d.location.toString() << ": [" << phaseStr << " " << sevStr << "] "
            << d.message << "\n";
    }
}

void DiagnosticEngine::clear() {
    diagnostics_.clear();
    errorCount_ = 0;
}

}