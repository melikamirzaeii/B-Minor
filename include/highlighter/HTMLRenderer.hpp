#pragma once

#include "highlighter/SyntaxHighlighter.hpp"
#include <ostream>
#include <string>
#include <vector>

namespace bminor {

class HTMLRenderer {
public:
    static std::string renderDocument(const std::string& title,
                                      const std::vector<StyledSpan>& spans);
    static void renderDocumentTo(std::ostream& out,
                                 const std::string& title,
                                 const std::vector<StyledSpan>& spans);

private:
    static const char* cssClassFor(HighlightCategory category) noexcept;
    static std::string escapeHTML(const std::string& raw);
};

}