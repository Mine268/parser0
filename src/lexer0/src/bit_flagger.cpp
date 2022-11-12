#include "bit_flagger.hpp"

namespace lexer0 {

    bit_flagger::bit_flagger(std::size_t n, bool v = false) : bit_storage(n, v) {}
    void bit_flagger::set(std::size_t ix, bool v) {
        bit_storage.at(ix) = v;
    }
    bool bit_flagger::get(std::size_t ix) {
        return bit_storage.at(ix);
    }

    bool operator==(const bit_flagger& lhs, const bit_flagger& rhs) {
        if (lhs.bit_storage.size() != rhs.bit_storage.size()) {
            return false;
        }
        for (std::size_t ix = 0; ix < lhs.bit_storage.size(); ++ix) {
            if (lhs.bit_storage.at(ix) != rhs.bit_storage.at(ix)) {
                return false;
            }
        }
        return true;
    }

    std::string bit_flagger::to_string() {
        std::string ret;
        for (std::size_t ix = 0; ix < this->bit_storage.size(); ++ix) {
            if (this->bit_storage.at(ix)) {
                if (!ret.empty()) {
                    ret += ',';
                }
                ret += std::to_string(ix);
            }
        }
        return ret;
    }
}