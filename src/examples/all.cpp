#include <experimental/ranges/ranges>
#include <stl2/view/all.hpp>

#include <iostream>

namespace ranges = std::experimental::ranges;

int main() {
    std::vector<int> v = {1,2,3,4};
    std::cout << ( v[0] + v[1] + v[2] + v[3]) << '\n' ;
    auto a = ranges::views::all(v);
    int& r = *(begin(a));
    r = 3;
    std::cout << ( v[0] + v[1] + v[2] + v[3]) << '\n' ;
    for (auto && i : a) {
        i *= 2;
    }

    std::cout << ( v[0] + v[1] + v[2] + v[3]) << '\n' ;
}
