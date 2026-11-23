#include "highlighter/HTMLRenderer.hpp"
#include <sstream>

namespace bminor {

const char* HTMLRenderer::cssClassFor(HighlightCategory category) noexcept {
    switch (category) {
        case HighlightCategory::Keyword:       return "kw";
        case HighlightCategory::TypeName:      return "type";
        case HighlightCategory::VariableIdent: return "var";
        case HighlightCategory::FunctionName:  return "fn";
        case HighlightCategory::NumberLiteral: return "num";
        case HighlightCategory::BoolLiteral:   return "bool";
        case HighlightCategory::StringLiteral: return "str";
        case HighlightCategory::Operator:      return "op";
        case HighlightCategory::Delimiter:     return "delim";
        case HighlightCategory::Comment:       return "comment";
        case HighlightCategory::Error:         return "err";
        case HighlightCategory::Whitespace:    return nullptr;
    }
    return nullptr;
}

std::string HTMLRenderer::escapeHTML(const std::string& raw) {
    std::string escaped;
    escaped.reserve(raw.size());
    for (char c : raw) {
        switch (c) {
            case '&':  escaped += "&amp;";  break;
            case '<':  escaped += "&lt;";   break;
            case '>':  escaped += "&gt;";   break;
            case '"':  escaped += "&quot;"; break;
            case '\'': escaped += "&#39;";  break;
            default:   escaped.push_back(c); break;
        }
    }
    return escaped;
}

void HTMLRenderer::renderDocumentTo(std::ostream& out,
                                    const std::string& title,
                                    const std::vector<StyledSpan>& spans) {
    out << "<!DOCTYPE html>\n"
        << "<html lang=\"en\">\n"
        << "<head>\n"
        << "  <meta charset=\"UTF-8\">\n"
        << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        << "  <title>" << escapeHTML(title) << " — B-Minor AST Highlight</title>\n"
        << "  <style>\n"
        << "    body { background: #1e1e2e; color: #cdd6f4; font-family: 'Fira Code', 'JetBrains Mono', Consolas, monospace; margin: 2rem; }\n"
        << "    h1 { font-size: 1rem; color: #a6adc8; font-weight: 500; margin-bottom: 0.75rem; }\n"
        << "    pre { background: #181825; border: 1px solid #313244; border-radius: 8px; padding: 1.25rem; overflow-x: auto; line-height: 1.55; font-size: 0.95rem; }\n"
        << "    .kw      { color: #569cd6; font-weight: bold; }\n"
        << "    .type    { color: #4ec9b0; }\n"
        << "    .var     { color: #f8f8f2; }\n"
        << "    .fn      { color: #dcdcaa; font-weight: 600; }\n"
        << "    .num     { color: #ce9178; }\n"
        << "    .bool    { color: #ce9178; font-weight: 600; }\n"
        << "    .str     { color: #6a9955; }\n"
        << "    .op      { color: #d4d4d4; }\n"
        << "    .delim   { color: #d4d4d4; }\n"
        << "    .comment { color: #7f849c; font-style: italic; }\n"
        << "    .err     { color: #f38ba8; text-decoration: underline wavy #f38ba8; background: rgba(243, 139, 168, 0.12); }\n"
        << "  </style>\n"
        << "</head>\n"
        << "<body>\n"
        << "  <h1>Source: " << escapeHTML(title) << "</h1>\n"
        << "  <pre><code>";

    for (const auto& span : spans) {
        const char* cls = cssClassFor(span.category);
        if (!cls) {
            out << escapeHTML(span.text);
        } else {
            out << "<span class=\"" << cls << "\">" << escapeHTML(span.text) << "</span>";
        }
    }

    out << "</code></pre>\n"
        << "</body>\n"
        << "</html>\n";
}

std::string HTMLRenderer::renderDocument(const std::string& title,
                                         const std::vector<StyledSpan>& spans) {
    std::ostringstream oss;
    renderDocumentTo(oss, title, spans);
    return oss.str();
}

}