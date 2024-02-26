#include "slice.hpp"

#include <fmt/format.h>
#include <stdexcept>

namespace core
{

/// @brief Convert the Slice object to a string representation "{start}:{stop}:{step}".
std::string Slice::to_string() const
{
    return fmt::format("{}:{}:{}",
                       start ? std::to_string(start.value()) : "",
                       stop ? std::to_string(stop.value()) : "",
                       step ? std::to_string(step.value()) : "");
}

/// @brief Whether two slices are equal.
/// @note Input should be normalized.
bool operator==(Slice const &lhs, Slice const &rhs)
{
    return lhs.start == rhs.start && lhs.stop == rhs.stop && lhs.step == rhs.step;
}

namespace detail
{

/// @brief Calculate number of elements included by slicing.
/// @param n The length of array
/// @return Number of elements included by slicing
/// @note Input should be normalized.
int64_t calcNumel(Slice const &slice, int64_t n)
{
    int64_t step = slice.step.value_or(1);
    assert(step != 0);

    int64_t numel;
    if (step > 0) {
        int64_t start = slice.start.value_or(0);
        int64_t stop = slice.stop.value_or(n);

        int64_t q = (stop - start) / step;
        int64_t r = (stop - start) % step;
        numel = q + (r > 0);
    } else /* step < 0 */
    {
        int64_t start = slice.start.value_or(n - 1);
        int64_t stop = slice.stop.value_or(-1);

        int64_t q = (-(stop - start)) / (-step);
        int64_t r = (-(stop - start)) % (-step);
        numel = q + (r > 0);
    }

    return numel;
}

/// @brief Modify start and stop using positive index or default value.
/// @param slice Input reference and modify its value to normalize
/// @param n The length of array
/// @return Slice to be normalized.
/// @note Check for empty slice and non-0 step.
/// @example {:: -> 0:n:1 };                {:-1: -> n-1:-1:-1}; 
///          {-5:2:-2 -> (-5+n):2:(-5+n)};  {-2n:1:3n -> 0:1:n};
/// @note    Not empty slice and non-0 step.
Slice normalize(Slice const &slice, int64_t n)
{
    if (n <= 0)
        throw std::invalid_argument("n must be non negative");

    Slice ans;
    int64_t step;
    int64_t start;
    int64_t stop;

    step = slice.step.value_or(1);
    if (step > 0) {

        start = slice.start.value_or(0);
        if (start < 0)
            start += n;
        start = std::max(start, 0l);

        stop = slice.stop.value_or(n);
        if (stop < 0)
            stop += n;
        stop = std::min(stop, n);

        // check for empty slice
        // 0 <= start && stop <= n is guaranteed
        if (start >= stop)
            throw std::invalid_argument("emtpy slice");

        ans.step = step;
        ans.start = start;
        ans.stop = stop;

    } else if (step < 0) {

        start = slice.start.value_or(n - 1);
        if (start < 0)
            start += n;
        start = std::min(start, n - 1);

        if (slice.stop.has_value()) {
            stop = slice.stop.value();
            if (stop < 0)
                stop += n;
            stop = std::max(stop, -1l);
        } else {
            stop = -1;
        }

        // check for empty slice
        // -1 <= stop && start <= n-1 is guaranteed
        if (stop >= start)
            throw std::invalid_argument("empty slice");

        // use null for default value
        ans.step = step;
        ans.start = start;
        if (stop != -1)
            ans.stop = stop;

    } else {
        throw std::invalid_argument("slice step must be non zero");
    }

    return ans;
}

/// @brief Modify start and stop of slice using positive index or default value, modify int64_t using positive index.
/// @param indicies Input reference and modify its value to normalize, default to full slicing
/// @param shape Array consisting of the length of the array
/// @return Unified representation using normalized slices and int64_t
/// @note int64_t n range (-n,n-1).
std::vector<std::variant<Slice, int64_t>> normalize(
    std::vector<std::variant<Slice, int64_t>> const &indicies,
    std::vector<int64_t> const &shape)
{
    assert(shape.size() > 0);

    if (indicies.size() > shape.size())
        throw std::invalid_argument("too much indicies");

    std::vector<std::variant<Slice, int64_t>> ans;
    for (int64_t i = 0; i < shape.size(); ++i) {
        std::variant<Slice, int64_t> index = Slice{.start=0, .step=1};
        if (i < indicies.size()) {
            switch (indicies[i].index()) {
                case 0: { // slice
                    auto const &raw = std::get<0>(indicies[i]);
                    index = normalize(raw, shape[i]);
                    break;
                }
                case 1: { // int
                    int64_t pos = std::get<1>(indicies[i]);
                    if(pos < 0) pos += shape[i];
                    if(pos < 0 || pos >= shape[i]) {
                        throw std::out_of_range("");
                    }
                    index = pos;
                    break;
                }
            }
        }
        ans.push_back(index);
    }

    return ans;
}

} // namespace detail

} // namspace core
