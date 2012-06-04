// Author: Hongzhang Liu
// Date:

#ifndef SWIFT_BASE_STRING_REF_H__
#define SWIFT_BASE_STRING_REF_H__

//copied from llvm

#include <cassert>
#include <cstring>
#include <limits>
#include <string>
#include <utility>

namespace swift {

/// StringRef - Represent a constant reference to a string, i.e. a character
/// array and a length, which need not be null terminated.
///
/// This class does not own the string data, it is expected to be used in
/// situations where the character data resides in some other buffer, whose
/// lifetime extends past that of the StringRef. For this reason, it is not in
/// general safe to store a StringRef.
class StringRef {
public:
    typedef const char *iterator;
    typedef const char *const_iterator;
    static const size_t npos = ~size_t(0);
    typedef size_t size_type;

public:
    /// @name Constructors
    /// @{

    /// Construct an empty string ref.
    /*implicit*/ StringRef() : data_(0), length_(0) {}

    /// Construct a string ref from a cstring.
    StringRef(const char *str) : data_(str)
    {
        assert(str && "StringRef cannot be built from a NULL argument");
        length_ = ::strlen(str); // invoking strlen(NULL) is undefined behavior
    }

    /// Construct a string ref from a pointer and length.
    StringRef(const char *data, size_t length)
        : data_(data), length_(length)
    {
        assert((data || length == 0) &&
                "StringRef cannot be built from a NULL argument with non-null length");
    }

    /// Construct a string ref from an std::string.
    StringRef(const std::string &str)
        : data_(str.data()), length_(str.length()) {}

    /// @}
    /// @name Iterators
    /// @{

    iterator begin() const { return data_; }

    iterator end() const { return data_ + length_; }

    /// @}
    /// @name String Operations
    /// @{

    /// data - Get a pointer to the start of the string (which may not be null
    /// terminated).
    const char *data() const { return data_; }

    /// empty - Check if the string is empty.
    bool empty() const { return length_ == 0; }

    /// size - Get the string size.
    size_t size() const { return length_; }

    /// front - Get the first character in the string.
    char front() const {
        assert(!empty());
        return data_[0];
    }

    /// back - Get the last character in the string.
    char back() const {
        assert(!empty());
        return data_[length_-1];
    }

    /// equals - Check for string equality, this is more efficient than
    /// compare() when the relative ordering of inequal strings isn't needed.
    bool equals(const StringRef &rhs) const {
        return length_ == rhs.length_ &&
            CompareMemory(data_, rhs.data_, rhs.length_) == 0;
    }

    /// compare - Compare two strings; the result is -1, 0, or 1 if this string
    /// is lexicographically less than, equal to, or greater than the \arg rhs.
    int compare(const StringRef &rhs) const {
        // Check the prefix for a mismatch.
        if (int Res = CompareMemory(data_, rhs.data_, min(length_, rhs.length_)))
            return Res < 0 ? -1 : 1;

        // Otherwise the prefixes match, so we only need to check the lengths.
        if (length_ == rhs.length_)
            return 0;
        return length_ < rhs.length_ ? -1 : 1;
    }

    /// str - Get the contents as an std::string.
    std::string str() const {
        if (data_ == 0) return std::string();
        return std::string(data_, length_);
    }

    /// @}
    /// @name Operator Overloads
    /// @{

    char operator[](size_t index) const {
        assert(index < length_ && "Invalid index!");
        return data_[index];
    }

    /// @}
    /// @name Type Conversions
    /// @{

    operator std::string() const {
        return str();
    }

    /// @}
    /// @name String Predicates
    /// @{

    /// startswith - Check if this string starts with the given \arg Prefix.
    bool startswith(const StringRef &prefix) const {
        return length_ >= prefix.length_ &&
            CompareMemory(data_, prefix.data_, prefix.length_) == 0;
    }

    /// endswith - Check if this string ends with the given \arg Suffix.
    bool endswith(const StringRef &suffix) const {
        return length_ >= suffix.length_ &&
            CompareMemory(end() - suffix.length_, suffix.data_, suffix.length_) == 0;
    }

    /// @}
    /// @name Substring Operations
    /// @{

    /// substr - Return a reference to the substring from [Start, Start + N).
    ///
    /// \param Start - The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param N - The number of characters to included in the substring. If N
    /// exceeds the number of characters remaining in the string, the string
    /// suffix (starting with \arg Start) will be returned.
    StringRef substr(size_t start, size_t n = npos) const {
        start = min(start, length_);
        return StringRef(data_ + start, min(n, length_ - start));
    }

    /// drop_front - Return a StringRef equal to 'this' but with the first
    /// elements dropped.
    StringRef drop_front(unsigned n = 1) const {
        assert(size() >= n && "Dropping more elements than exist");
        return substr(n);
    }

    /// drop_back - Return a StringRef equal to 'this' but with the last
    /// elements dropped.
    StringRef drop_back(unsigned n = 1) const {
        assert(size() >= n && "Dropping more elements than exist");
        return substr(0, size() - n);
    }

    /// slice - Return a reference to the substring from [Start, End).
    ///
    /// \param Start - The index of the starting character in the substring; if
    /// the index is npos or greater than the length of the string then the
    /// empty substring will be returned.
    ///
    /// \param End - The index following the last character to include in the
    /// substring. If this is npos, or less than \arg Start, or exceeds the
    /// number of characters remaining in the string, the string suffix
    /// (starting with \arg Start) will be returned.
    StringRef slice(size_t start, size_t end) const {
        start = min(start, length_);
        end = min(max(start, end), length_);
        return StringRef(data_ + start, end - start);
    }
    /// @}

private:
    /// The start of the string, in an external buffer.
    const char *data_;

    /// The length of the string.
    size_t length_;

    // Workaround PR5482: nearly all gcc 4.x miscompile StringRef and std::min()
    // Changing the arg of min to be an integer, instead of a reference to an
    // integer works around this bug.
    static size_t min(size_t a, size_t b) { return a < b ? a : b; }
    static size_t max(size_t a, size_t b) { return a > b ? a : b; }

    // Workaround memcmp issue with null pointers (undefined behavior)
    // by providing a specialized version
    static int CompareMemory(const char *lhs, const char *rhs, size_t length) {
        if (length == 0) { return 0; }
        return ::memcmp(lhs, rhs, length);
    }
};

/// @name StringRef Comparison Operators
/// @{

inline bool operator==(StringRef lhs, StringRef rhs) {
    return lhs.equals(rhs);
}

inline bool operator!=(StringRef lhs, StringRef rhs) {
    return !(lhs == rhs);
}

inline bool operator<(StringRef lhs, StringRef rhs) {
    return lhs.compare(rhs) == -1;
}

inline bool operator<=(StringRef lhs, StringRef rhs) {
    return lhs.compare(rhs) != 1;
}

inline bool operator>(StringRef lhs, StringRef rhs) {
    return lhs.compare(rhs) == 1;
}

inline bool operator>=(StringRef lhs, StringRef rhs) {
    return lhs.compare(rhs) != -1;
}

inline std::string &operator+=(std::string &buffer, StringRef string) {
    return buffer.append(string.data(), string.size());
}

/// @}

}

#endif

