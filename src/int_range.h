#ifndef SZYNKA_INT_RANGE_H
#define SZYNKA_INT_RANGE_H

namespace szynka {

struct int_range
{
    int first;
    int count;

    int end() const { return first + count; }

    int_range() = default;

    int_range(int first, int count) : first(first), count(count) { }
};

} // namespace szynka

#endif // SZYNKA_INT_RANGE_H
