#include "highlighter/ANSIRenderer.hpp"
#include <sstream>

namespace bminor {

const char* ANSIRenderer::ansiCodeFor(HighlightCategory category) noexcept {
    switch (category) {
        case HighlightCategory::Keyword:       return "\033[1;34m";
        case HighlightCategory::TypeName:      return "\033[36m";
        case HighlightCategory::VariableIdent: return "\033[97m";
        case HighlightCategory::FunctionName:  return "\033[1;33m";
        case HighlightCategory::NumberLiteral: return "\033[38;5;208m";
        case HighlightCategory::BoolLiteral:   return "\033[38;5;208m";
        case HighlightCategory::StringLiteral: return "\033[32m";
        case HighlightCategory::Operator:      return "\033[37m";
        case HighlightCategory::Delimiter:     return "\033[37m";
        case HighlightCategory::Comment:       return "\033[3;90m";
        case HighlightCategory::Error:         return "\033[4;31m";
        case HighlightCategory::Whitespace:    return "";
    }
    return "";
}

void ANSIRenderer::renderTo(std::ostream& out, const std::vector<StyledSpan>& spans) {
    for (const auto& span : spans) {
        if (span.category == HighlightCategory::Whitespace) {
            out << span.text;
        } else {
            out << ansiCodeFor(span.category) << span.text << "\033[0m";
        }
    }
}

std::string ANSIRenderer::render(const std::vector<StyledSpan>& spans) {
    std::ostringstream oss;
    renderTo(oss, spans);
    return oss.str();
}

}