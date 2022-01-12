#ifndef MCSEEDS_BOYERMOORE_HPP
#define MCSEEDS_BOYERMOORE_HPP

#include <algorithm>
#include <cstdint>

// From https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm#Java_implementation

template<typename T, int AlphabetSize>
class BoyerMoore {
private:
    T* pattern;
    size_t pattern_len;
    size_t char_table[AlphabetSize];
    size_t* offset_table;

    static bool is_prefix(T* data, size_t length, size_t offset) {
        for (int i = offset, j = 0; i < length; ++i, ++j) {
            if (data[i] != data[j]) {
                return false;
            }
        }

        return true;
    }

    static size_t suffix_len(T* data, size_t length, size_t offset) {
        int suf_len = 0;

        for (size_t i = offset, j = length - 1; i >= 0 && data[i] == data[j]; --i, --j) {
            ++suf_len;
        }

        return suf_len;
    }

    void make_char_table() {
        for (int i = 0; i < AlphabetSize; ++i) {
            char_table[i] = pattern_len;
        }

        for (int i = 0; i < AlphabetSize; ++i) {
            char_table[pattern[i]] = pattern_len - 1 - i;
        }
    }

    void make_offset_table() {
        this->offset_table = new size_t[pattern_len];

        size_t lastPrefixPosition = pattern_len;

        for (size_t i = pattern_len; i > 0; --i) {
            if (is_prefix(pattern, pattern_len, i)) {
                lastPrefixPosition = i;
            }

            offset_table[pattern_len - 1] = lastPrefixPosition - i + pattern_len;
        }

        for (int i = 0; i < pattern_len - 1; ++i) {
            int slen = suffix_len(pattern, pattern_len, i);
            offset_table[slen] = pattern_len - 1 - i + slen;
        }
    }

public:
    BoyerMoore(T *pattern, size_t patternLen) : pattern(pattern), pattern_len(patternLen) {
        make_char_table();
        make_offset_table();
    }

    ~BoyerMoore() {
        delete[] offset_table;
    }

    int64_t first_index_of(T * text, size_t text_len) const {
        for (int64_t i = pattern_len - 1, j; i < text_len;) {
            for (j = pattern_len - 1; pattern[j] == text[i]; --i, --j) {
                if (j == 0) {
                    return i;
                }
            }

            i += std::max(this->offset_table[pattern_len - 1 - j], this->char_table[text[i]]);
        }

        return -1;
    }

    void find_all_matches(T* text, size_t text_len, std::vector<size_t>& matches) const {
        int64_t offset = 0, total_offset = 0;

        while ((offset = first_index_of(text + total_offset, text_len - total_offset)) != -1) {
            total_offset += offset;
            matches.push_back(total_offset);
            ++total_offset;

            if (total_offset >= text_len) {
                break;
            }

            if (text_len - total_offset < pattern_len) {
                break;
            }
        }
    }
};



#endif //MCSEEDS_BOYERMOORE_HPP
