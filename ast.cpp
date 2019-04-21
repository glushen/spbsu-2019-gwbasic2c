struct Statement {
    std::string* statement;

    explicit Statement(std::string* statement): statement(statement) { }

    void print() {
        std::cout << *statement;
    }
};

struct Line {
    int line_number;
    std::vector<Statement*>* statement_list;
    std::string* comment;

    Line(int line_number, std::vector<Statement*>* statement_list, std::string* comment):
        line_number(line_number), statement_list(statement_list), comment(comment) { }

    void print() {
        std::cout << line_number << " ";
        bool firstPrinted = false;
        for (auto statement : *statement_list) {
            if (firstPrinted) {
                std::cout << " :";
            } else {
                firstPrinted = true;
            }
            statement->print();
        }
        std::cout << " '" << *comment;
    }
};