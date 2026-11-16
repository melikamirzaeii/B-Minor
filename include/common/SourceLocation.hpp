#pragma once

#include <cstddef>
#include <string>

namespace bminor {

struct SourceLocation {
    std::string filename;
    std::size_t line = 1;
    std::size_t column = 1;
    std::size_t offset = 0;
    std::size_t length = 0;

    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

}