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



template<typename Maybe, std::experimental::ranges::CopyConstructible T>
requires std::is_object_v<T>
class maybe_view : public std::experimental::ranges::view_interface<maybe_view<Maybe, T>> {
  private:
    Maybe  value_; // exposition only
  public:
    maybe_view() = default;

    //    template<typename U>
    //    requires std::experimental::ranges::Constructible<T, decltype(maybe_value(std::declval<U>()))>
    constexpr maybe_view(const Maybe& maybe) : value_(maybe)
    {}

    constexpr T* begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T* end() noexcept { if (data()) return data() + 1; else return data(); }
    constexpr const T* end() const noexcept { if (data()) return data() + 1; else return data(); }
    constexpr std::ptrdiff_t size() noexcept { if (value_) return 1; else return 0;}

    constexpr T* data() noexcept { if (value_) return std::addressof(*value_); else return 0;}
    constexpr const T* data() const noexcept { if (value_) return std::addressof(*value_); else return 0;}
};


template<class Maybe>
//maybe_view(const Maybe&) -> maybe_view<Maybe, typename Maybe::value_type>;
maybe_view(const Maybe&) -> maybe_view<Maybe, typename Maybe::value_type>;

// template<class Maybe>
// maybe_view(const Maybe&) -> maybe_view<Maybe, std::remove_pointer_t<Maybe>>;

 // template<class U>
// explicit maybe_view(U&&) -> maybe_view<invoke_result<decltype(std::ranges::maybe_value), U>>;


int main() {
	auto v = std::vector<std::string>{"this", "is", "not", "a", "test"};
	ranges::sort(v);
	auto out = ranges::ostream_iterator<std::string>{std::cout, " "};
	ranges::copy(v, out);
	std::cout << '\n';
	auto result = ranges::reverse_copy(v, out);
	std::cout << '\n';
    //	return !(result.in() == ranges::end(v));

    constexpr std::optional s{7};
    constexpr std::optional<int> e{};
    // static_assert(std::ranges::maybe_has_value(s));
    // static_assert(!std::ranges::maybe_has_value(e));


    //    std::ranges::maybe_view<std::optional<int>, int> vs2{s};
    maybe_view vs2{s};
    std::cout << *begin(vs2) << '\n';
	// std::cout << '\n';
    for (auto i : vs2)
        std::cout << "i=" << i << '\n'; // prints 4

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        std::cout << "i=" <<  i << '\n'; // does not print

    // int i = 7;
    // int * pi = &i;
    // std::ranges::maybe_view vpi{pi};
    // for (auto i : vpi)
    //     std::cout << i; // prints 7


}
