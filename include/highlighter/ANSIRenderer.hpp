#pragma once

#include "highlighter/SyntaxHighlighter.hpp"
#include <ostream>
#include <string>
#include <vector>

namespace bminor {

class ANSIRenderer {
public:
    static std::string render(const std::vector<StyledSpan>& spans);
    static void renderTo(std::ostream& out, const std::vector<StyledSpan>& spans);

private:
    static const char* ansiCodeFor(HighlightCategory category) noexcept;
};

}