#include <smd/views/nullable.h>

#include <cassert>
#include <vector>
#include <iostream>

namespace ranges = std::ranges;
using namespace smd;

std::optional<int> possible_value() { return {7}; }

void use(int t) { assert(t == t); }

void before0() {
    {
        auto&& opt = possible_value();
        if (opt) {
            // a few dozen lines ...
            use(*opt); // is *opt OK ?
        }
    }
}

void after0() {
    for (auto&& opt : views::nullable(possible_value())) {
        // a few dozen lines ...
        use(opt); // opt is OK
    }
}

void before1() {
    std::optional o{7};
    if (o) {
        *o = 9;
        std::cout << "o=" << *o << " prints 9\n";
    }
    std::cout << "o=" << *o << " prints 9\n";
}

void after1() {
    std::optional o{7};
    for (auto&& i : views::nullable_view<std::optional<int>&>(o)) {
        i = 9;
        std::cout << "i=" << i << " prints 9\n";
    }
    std::cout << "o=" << *o << " prints 9\n";

    // if range for is too much magic
    if (auto v = views::nullable_view<std::optional<int>&>(o);
        std::begin(v) != std::end(v)) {
        auto itr = std::begin(v);
        *itr     = 10;
        std::cout << "*itr=" << *itr << " prints 10\n";
    }
    std::cout << "o=" << *o << " prints 10\n";
}

void before2() {
    std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
    auto             test = [](int i) -> std::optional<int> {
        switch (i) {
        case 1:
        case 3:
        case 7:
        case 9:
            return i;
        default:
            return {};
        }
    };

    auto&& r = v | ranges::views::transform(test) |
               ranges::views::filter([](auto x) { return bool(x); }) |
               ranges::views::transform([](auto x) { return *x; }) |
               ranges::views::transform([](int i) {
                   std::cout << i;
                   return i;
               });
    for (auto&& i : r) {
    };
}

void after2() {
    std::vector<int> v{2, 3, 4, 5, 6, 7, 8, 9, 1};
    auto             test = [](int i) -> std::optional<int> {
        switch (i) {
        case 1:
        case 3:
        case 7:
        case 9:
            return i;
        default:
            return {};
        }
    };

    auto&& r = v | ranges::views::transform(test) |
               ranges::views::transform(views::nullable) |
               ranges::views::join | ranges::views::transform([](int i) {
                   std::cout << i;
                   return i;
               });
    for (auto&& i : r) {
    };
    std::cout << std::endl;
}


void motivation1() {
    std::vector<std::optional<int>> v{std::optional<int>{42},
                                      std::optional<int>{},
                                      std::optional<int>{6 * 9}};

    auto r = std::views::join(std::views::transform(v, views::nullable));

    for (auto i : r) {
        std::cout << i << '\t'; // prints 42 and 54 skipping the empty optional
    }
    std::cout << std::endl;
}

void motivation1a() {
    int fortytwo = 42;
    int sixbynine = 54;

    std::vector<int*> v{&fortytwo, nullptr, &sixbynine};

    auto r = std::views::join(std::views::transform(v, views::nullable));

    for (auto i : r) {
        std::cout << i << '\t'; // prints 42 and 54 skipping the nullptr
    }
    std::cout << std::endl;
}

int main() {
    before0();
    after0();

    before1();
    after1();

    before2();
    after2();

    motivation1();
    motivation1a();
}
