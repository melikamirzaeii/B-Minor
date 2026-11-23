| Category | Token(s) | Formal Regular Expression |
| :--- | :--- | :--- |
| **Keywords** | `array`, `auto`, `boolean`, `char`, `else`, `false`, `for`, `function`, `if`, `integer`, `print`, `return`, `string`, `true`, `void`, `while` | Exact literal match (case-sensitive) |
| **Identifiers** | `IDENTIFIER` | `[a-zA-Z_][a-zA-Z0-9_]*` |
| **Integer Literals** | `INT_LITERAL` | `[0-9]+` |
| **Boolean Literals** | `KW_TRUE`, `KW_FALSE` | `true \| false` |
| **String Literals** | `STRING_LITERAL` | `"([^"\\\n] \| \\[ntr\\'"0abfv])*"` |
| **Character Literals** | `CHAR_LITERAL` | `'([^'\\\n] \| \\[ntr\\'"0abfv])'` |
| **Multi-Char Operators** | `++`, `--`, `==`, `!=`, `<=`, `>=`, `&&`, `\|\|` | `\+\+ \| -- \| == \| != \| <= \| >= \| && \| \|\|` |
| **Single-Char Operators** | `+`, `-`, `*`, `/`, `%`, `^`, `=`, `<`, `>`, `!` | `[\+\-\*/%\^=<>!]` |
| **Delimiters** | `{`, `}`, `(`, `)`, `[`, `]`, `;`, `,`, `:` | `[\{\}\(\)\[\];,:]` |
| **Line Comments** | `LINE_COMMENT` | `//[^\n]*` |
| **Block Comments** | `BLOCK_COMMENT` | `/\*([^*] \| \*+[^*/])*\*+/` |
| **Whitespace** | `WHITESPACE` | `[ \t\r\n]+` |


1. **Maximal Munch (Longest Match)**: Always consumes the longest prefix matching a valid regular expression.
2. **Keyword-Before-Identifier Priority**: Words matching `[a-zA-Z_][a-zA-Z0-9_]*` are resolved against the reserved keyword table before falling back to `IDENTIFIER`.
3. **Lossless Trivia Retention**: `WHITESPACE`, `LINE_COMMENT`, and `BLOCK_COMMENT` remain in the master token stream with exact byte offsets.