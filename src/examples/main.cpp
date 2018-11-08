#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges/algorithm>
#include <experimental/ranges/iterator>
#include <experimental/ranges/concepts>
#include <experimental/ranges/ranges>

#include <array>

// Qualify everything with "std::experimental::ranges" if you like,
// I prefer to use a namespace alias:
namespace ranges = std::experimental::ranges;

template<class...> class show_type;

void foo(ranges::Readable&) {}



template<std::experimental::ranges::CopyConstructible Maybe,  typename T>
requires std::is_object_v<T>
class maybe_view : public std::experimental::ranges::view_interface<maybe_view<Maybe, T>> {
  private:
    std::experimental::ranges::detail::semiregular_box<Maybe> value_;

    //    Maybe value_; // exposition only
  public:
    maybe_view() = default;

    constexpr maybe_view(const Maybe& maybe) : value_(maybe)
    {}

    constexpr maybe_view(Maybe&& maybe) : value_(std::move(maybe))
    {}

    constexpr T* begin() noexcept { return data(); }
    constexpr const T* begin() const noexcept { return data(); }
    constexpr T* end() noexcept { if (data()) return data() + 1; else return data(); }
    constexpr const T* end() const noexcept { if (data()) return data() + 1; else return data(); }
    constexpr std::ptrdiff_t size() noexcept { if (value_.get()) return 1; else return 0;}

    constexpr T* data() noexcept { if (value_.get()) return std::addressof(*value_.get()); else return 0;}
    constexpr const T* data() const noexcept { if (value_.get()) return std::addressof(*value_.get()); else return 0;}
};


template< class T >
struct dereference {
    typedef std::remove_reference_t<decltype(*std::declval<T>())> type;
};



template<class Maybe>
//maybe_view(const Maybe&) -> maybe_view<Maybe, typename Maybe::value_type>;
maybe_view(const Maybe&) -> maybe_view<Maybe, typename dereference<Maybe>::type>;

// template<class Maybe>
// maybe_view(const Maybe&) -> maybe_view<Maybe, std::remove_pointer_t<Maybe>>;

 // template<class U>
// explicit maybe_view(U&&) -> maybe_view<invoke_result<decltype(std::ranges::maybe_value), U>>;

namespace view {
struct __maybe_fn {
    template<class T>
    constexpr auto operator()(T&& t) const
        STL2_NOEXCEPT_REQUIRES_RETURN(
            maybe_view{std::forward<T>(t)}
			)
		};

inline constexpr __maybe_fn maybe {};
}

template <typename CHECK>
void check(CHECK k)
{
    show_type<CHECK> _;

}

int main() {

    std::optional s{7};
    std::optional<int> e{};

    // maybe_view vs2{s};
    // std::cout << *begin(vs2) << '\n';

    // for (auto i : vs2)
    //     std::cout << "i=" << i << '\n'; // prints 4

    // for (auto i : view::maybe(s))
    //     std::cout << "i=" << i << '\n'; // prints 4

    maybe_view e2{std::optional<int>{}};
    for (int i : e2)
        std::cout << "i=" <<  i << '\n'; // does not print

    // auto oe = std::optional<int>{};
    // for (int i : view::maybe(oe))
    //     std::cout << "i=" <<  i << '\n'; // does not print

    int j = 7;
    int * pj = &j;
    // maybe_view vpj{pj};
    // for (auto i : vpj)
    //     std::cout << "i=" << i << '\n'; // prints 7

    // for (auto i : view::maybe(pj))
    //     std::cout << "i=" << i << '\n'; // prints 7

    // std::cout << "j=" << j << '\n'; // prints 7

    for (auto&& i : view::maybe(pj))
    {
        i = 27;
        std::cout << "i=" << i << '\n'; // prints 7
    }

    std::cout << "j=" << j << '\n'; // prints 7


    for (auto&& i : view::maybe(s))
    {
        i = 9;
        std::cout << "i=" << i << '\n'; // prints 7
    }
    std::cout << "s=" << *s << '\n'; // prints 7

    // for (auto&& i : std::experimental::ranges::view::single(j))
    // {
    //     i = 19;
    //     std::cout << "i=" << i << '\n'; // prints 7
    // }
    // std::cout << "j=" << *s << '\n'; // prints 7

     {
         auto && __range = view::maybe(s) ;
         auto __begin = std::begin(__range) ;
         auto __end = std::end(__range) ;
         for ( ; __begin != __end; ++__begin) {
             auto&& i = *__begin;
             i = 90;
         }
     }

     // std::array<int, 2> a2 = {2,3};
     // for (auto&& i : view::maybe(a2))
     // {
     //     i = 9;
     //     std::cout << "i=" << i << '\n'; // prints 7
     // }

}
