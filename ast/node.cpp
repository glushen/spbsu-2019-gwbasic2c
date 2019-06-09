#include <utility>
#include "node.h"
#include <cassert>
#include "util.h"

using namespace std;
using namespace gw_logic;

std::string ast::to_string(gw_logic::Type type) {
    switch (type) {
        case INT: return "INT";
        case FLOAT: return "FLOAT";
        case DOUBLE: return "DOUBLE";
        case STRING: return "STRING";
        case INT_REF: return "INT_REF";
        case FLOAT_REF: return "FLOAT_REF";
        case DOUBLE_REF: return "DOUBLE_REF";
        case STRING_REF: return "STRING_REF";
        case VOID: return "VOID";
    }
    assert(false);
}

bool ast::isReference(gw_logic::Type type) {
    switch (type) {
        case INT_REF:
        case FLOAT_REF:
        case DOUBLE_REF:
        case STRING_REF:
            return true;
        default:
            return false;
    }
}

ast::Node::~Node() = default;
