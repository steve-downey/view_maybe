#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/iterator>
#include <experimental/ranges/concepts>


// Qualify everything with "std::experimental::ranges" if you like,
// I prefer to use a namespace alias:
namespace ranges = std::experimental::ranges;

template<class...> class show_type;

void foo(ranges::Readable&) {}

namespace std::ranges {

namespace __maybe {
struct fn_maybe_has_value {
    template<typename U>
    constexpr
    auto operator()(U* u) const -> bool {return (u != nullptr);}

    template<typename T>
    constexpr
    auto operator()(const std::optional<T>& o) const -> bool {return !!o;}
};

struct fn_maybe_value {
    template<typename U>
    constexpr
    auto operator()(U* u) const -> U& {}

    template<typename T>
    auto operator()(const std::optional<T>& o) -> T& {o.value();}

    template<typename T>
    constexpr
    auto operator()(std::optional<T>&&  o) const -> T& {o.value();}

};
}

inline constexpr __maybe::fn_maybe_value maybe_value{};
inline constexpr __maybe::fn_maybe_has_value maybe_has_value{};

template<typename T>
//requires is_object_v<T>
class maybe_view : public std::experimental::ranges::view_interface<maybe_view<T>> {
  private:
    T* value_; // exposition only
  public:
    maybe_view() = default;

    template<typename U>
    //    requires std::experimental::ranges::Constructible<T, decltype(maybe_value(std::declval<U>()))>
    constexpr maybe_view(const U& u);

    constexpr T* begin() const noexcept;

    constexpr T* end() const noexcept;

    constexpr static ptrdiff_t size() noexcept;

    constexpr const T* data() const noexcept;
};


template<class U>
explicit maybe_view(U) -> maybe_view<std::invoke_result<decltype(std::ranges::maybe_value), U>>;

// template<class U>
// explicit maybe_view(U&&) -> maybe_view<invoke_result<decltype(std::ranges::maybe_value), U>>;
}


int main() {
	auto v = std::vector<std::string>{"this", "is", "a", "test"};
	ranges::sort(v);
	auto out = ranges::ostream_iterator<std::string>{std::cout, " "};
	ranges::copy(v, out);
	std::cout << '\n';
	auto result = ranges::reverse_copy(v, out);
	std::cout << '\n';
	return !(result.in() == ranges::end(v));

    constexpr std::optional s{7};
    constexpr std::optional<int> e{};
    static_assert(std::ranges::maybe_has_value(s));
    static_assert(!std::ranges::maybe_has_value(e));


    std::ranges::maybe_view vs2{s};
    // for (auto i : s2)
    //     std::cout << i; // prints 4

    // std::ranges::maybe_view<int> e2{std::optional<int>{}};
    // for (int i : e2)
    //     std::cout << i; // does not print

}
