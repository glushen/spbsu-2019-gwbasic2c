#include <iostream>
#include <algorithm>

#include <parser.h>

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

void print(std::vector<Line*>* lines) {
    for (auto line : *lines) {
        line->print();
        std::cout << std::endl;
    }
}

void handleResult(std::vector<Line*>* lines) {
    std::sort(lines->begin(), lines->end(), [](Line* a, Line* b) {
        return a->line_number < b->line_number;
    });
    print(lines);
}

void yyerror(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}
