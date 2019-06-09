#include <utility>
#include "node.h"
#include <cassert>
#include "util.h"

std::string ast::to_string(gw::Type type) {
    switch (type) {
        case gw::INT: return "INT";
        case gw::FLOAT: return "FLOAT";
        case gw::DOUBLE: return "DOUBLE";
        case gw::STRING: return "STRING";
        case gw::INT_REF: return "INT_REF";
        case gw::FLOAT_REF: return "FLOAT_REF";
        case gw::DOUBLE_REF: return "DOUBLE_REF";
        case gw::STRING_REF: return "STRING_REF";
        case gw::VOID: return "VOID";
    }
    assert(false);
}

bool ast::isReference(gw::Type type) {
    switch (type) {
        case gw::INT_REF:
        case gw::FLOAT_REF:
        case gw::DOUBLE_REF:
        case gw::STRING_REF:
            return true;
        default:
            return false;
    }
}

ast::Node::~Node() = default;
