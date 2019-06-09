#include "core.cpp"

template<typename T>
void dim(std::vector<T>& values, std::vector<gw_int>& max_indexes, const std::vector<gw_int>& new_max_indexes) {
    assert(!new_max_indexes.empty(), MISSING_OPERAND);
    assert(max_indexes.empty(), DUPLICATE_DEFINITION);
    int value_count = 1;
    for (gw_int max_index : new_max_indexes) {
        assert(max_index >= 0, ILLEGAL_FUNCTION_CALL);
        value_count *= int(max_index) + 1;
    }
    max_indexes = new_max_indexes;
    values.resize(value_count);
}
template<typename T>
T& get(std::vector<T>& values, std::vector<gw_int>& max_indexes, const std::vector<gw_int>& indexes) {
    int dim_count = indexes.size();
    assert(dim_count > 0, MISSING_OPERAND);
    if (max_indexes.empty()) {
        dim(values, max_indexes, std::vector<gw_int>(dim_count, 10));
    } else {
        assert(max_indexes.size() == dim_count, SUBSCRIPT_OUT_OF_RANGE);
    }

    int result_index = 0;
    for (int i = 0; i < dim_count; i++) {
        assert(indexes[i] <= max_indexes[i], SUBSCRIPT_OUT_OF_RANGE);
        result_index *= (int(max_indexes[i]) + 1);
        result_index += indexes[i];
    }
    return values[result_index];
}