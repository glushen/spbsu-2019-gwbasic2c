#include "core.cpp"

template<typename T>
void dim(std::vector<T>& values, std::vector<gw_int>& sizes, const std::vector<gw_int>& new_sizes) {
    assert(!new_sizes.empty(), MISSING_OPERAND);
    assert(sizes.empty(), DUPLICATE_DEFINITION);
    int value_count = 1;
    for (gw_int size : new_sizes) {
        assert(size >= 0, ILLEGAL_FUNCTION_CALL);
        value_count *= size;
    }
    sizes = new_sizes;
    values.resize(value_count);
}
template<typename T>
T& get(std::vector<T>& values, std::vector<gw_int>& sizes, const std::vector<gw_int>& indexes) {
    int dim_count = indexes.size();
    assert(dim_count > 0, MISSING_OPERAND);
    if (sizes.empty()) {
        dim(values, sizes, std::vector<gw_int>(dim_count, 11));
    } else {
        assert(sizes.size() == dim_count, SUBSCRIPT_OUT_OF_RANGE);
    }

    int result_index = 0;
    for (int i = 0; i < dim_count; i++) {
        assert(indexes[i] < sizes[i], SUBSCRIPT_OUT_OF_RANGE);
        result_index *= sizes[i];
        result_index += indexes[i];
    }
    return values[result_index];
}