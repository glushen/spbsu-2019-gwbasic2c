#include <iostream>
#include <algorithm>
#include <parser.h>
#include "ast/program.h"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        auto file = freopen(argv[1], "r", stdin);
        if (file == nullptr) {
            std::cerr << "Cannot open file " << argv[1] << std::endl;
            return 1;
        }
    }
    if (argc > 2) {
        auto file = freopen(argv[2], "w", stdout);
        if (file == nullptr) {
            std::cerr << "Cannot open file " << argv[2] << std::endl;
            return 1;
        }
    }

    yyparse();

    return 0;
}

void handleResult(std::vector<ast::Line*>* lines) {
    ast::printProgram(std::cout, lines);
}

void yyerror(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}
