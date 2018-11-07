#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges/concepts>
#include <type_traits>

namespace ranges = std::experimental::ranges;

// template <typename T, typename F>
// requires ranges::Invocable<F, T>
// auto fmap(F, std::vector<T>);


template<class F, class R, class... Args>
concept bool MyRegularInvocableR =
    ranges::Invocable<F, Args...> &&
    ranges::Same<R, std::result_of_t<F&&(Args&&...)>>;

// template <typename F,
//           template <typename> typename Functor,
//           typename T,
//           typename U>
// requires MyRegularInvocableR<F, U, T>
// auto fmap(F, Functor<T>);


template <typename F,
          template <typename> typename Functor,
          typename T,
          typename U = std::result_of_t<F&&(T&&)>
          >
requires MyRegularInvocableR<F, U, T>
auto fmap(F f, Functor<T> range) -> Functor<U>
{
    Functor<U> out;
    for(auto&& r: range)
    {
        out.emplace_back(f(r));
    }
    return out;
}


int main() {
    std::vector<int> v{1,2,3,4};
    auto i2d = [](int x){return double(x);};
    std::vector<double> k = fmap(i2d, v);
    std::cout << k[0];
}
