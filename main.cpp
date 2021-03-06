#include <utility>
#include <iostream>
#include <algorithm>
#include <parser.h>
#include "ast/program.h"

int yy_utilityVariablesCount = 0;
bool yy_hadErrors = false;

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

    return yy_hadErrors ? EXIT_FAILURE : EXIT_SUCCESS;
}

void yyerror(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    yy_hadErrors = true;
}
