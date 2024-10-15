#include <smd/maybe.h>
#include <smd/maybe.h>

#include <gtest/gtest.h>

#include <optional>
#include <memory>
#include <array>
#include <expected>

using namespace smd;

void func();

TEST(MaybeConceptsTest, TestGTest) { ASSERT_EQ(1, 1); }

TEST(MaybeConceptsTest, Constructors) {}

TEST(MaybeConceptsTest, Concept) {
    static_assert(maybe<std::optional<int>>);
    static_assert(maybe<std::optional<const int>>);
    static_assert(maybe<std::optional<volatile int>>);
    static_assert(maybe<std::optional<const volatile int>>);

    static_assert(maybe<int*>);
    static_assert(maybe<const int*>);
    static_assert(maybe<const int*>);

    static_assert(maybe<decltype(func)>);

    static_assert(maybe<std::shared_ptr<int>>);
    static_assert(maybe<std::unique_ptr<int>>);
}

TEST(MaybeConceptsTest, YieldIf) {
    bool t = true;
    bool f = false;
    int  i = 5;

    auto v1 = smd::yield_if(t, 5);
    auto v2 = smd::yield_if(t, i);

    auto e1 = smd::yield_if(f, i);
    auto e2 = smd::yield_if(f, 5);

    ASSERT_EQ(v1, v2);
    ASSERT_NE(v1, e1);
    ASSERT_EQ(e1, e2);

    int  a[] = {5};
    auto a1  = smd::yield_if(t, a);

    std::optional<int*> o1 = std::make_optional(a);

    ASSERT_EQ(a1, a1);
    ASSERT_EQ(a1, o1);

    std::array<int, 5> ar = {0, 1, 2, 3, 4};
    auto               a2 = smd::yield_if(t, ar);
    ASSERT_EQ(a2, a2);
}

TEST(MaybeConceptsTest, YieldIfExpected) {
    bool t = true;
    bool f = false;
    int  i = 5;

    auto v1 = smd::yield_if<int, std::expected<int, int>>(t, 5);
    auto v2 = smd::yield_if<int&, std::expected<int, int>>(t, i);

    auto e1 = smd::yield_if<int&, std::expected<int, int>>(f, i);
    auto e2 = smd::yield_if<int, std::expected<int, int>>(f, 5);

    ASSERT_EQ(v1, v2);
    ASSERT_NE(v1, e1);
    ASSERT_EQ(e1, e2);
}

int   ifunc();
int&& irfunc();

std::string& testLVal(std::string& l) {return l;}


TEST(MaybeConceptsTest, ReferenceOr) {
    std::optional<int> e;
    std::optional<int> five{5};

    const int fortytwo = 42;
    int       nine     = 9;

    // auto&& r1 = smd::reference_or(e, nine);
    // auto&& r2 = smd::reference_or(five, fortytwo);
    // auto&& r3 = smd::reference_or(five, ifunc());

    static_assert(std::same_as<int&, decltype(smd::reference_or(e, nine))>);
    static_assert(
        std::same_as<const int&, decltype(smd::reference_or(five, fortytwo))>);
    // static_assert(
    //     std::same_as<const int&, decltype(smd::reference_or(five, ifunc()))>);
    // static_assert(
    //     std::same_as<const int&, decltype(smd::reference_or(five, irfunc()))>);

    std::string longString{"A very long string that is not short at all and will allocate"};
    std::optional<std::string> os{"data"};
    std::optional<std::string> es{};

    std::string s1 = smd::reference_or(os, testLVal(longString));
    std::string s2 = smd::reference_or(es, testLVal(longString));

    static_assert(std::same_as<std::string &, decltype(smd::reference_or(os, testLVal(longString)))>);

    std::string& r1 = smd::reference_or(es, testLVal(longString));
    std::string& r2 = smd::reference_or(os, testLVal(longString));

    ASSERT_EQ(r1, longString);
    ASSERT_EQ(r2, "data");

    r1 = "assign a string";
    r2 = "assign a different string";

    ASSERT_EQ(r1, longString);
    ASSERT_EQ(longString, "assign a string");
    ASSERT_NE(r2, "data");
    ASSERT_EQ(os, "assign a different string");

}

TEST(MaybeConceptsTest, ValueOr) {
    std::optional<int> e;
    std::optional<int> five{5};

    const int fortytwo = 42;
    int       nine     = 9;

    // auto&& r1 = smd::reference_or(e, nine);
    // auto&& r2 = smd::reference_or(five, fortytwo);
    // auto&& r3 = smd::reference_or(five, ifunc());

    static_assert(std::same_as<int, decltype(smd::value_or(e, nine))>);
    static_assert(
        std::same_as<int, decltype(smd::value_or(five, fortytwo))>);
    static_assert(
        std::same_as<int, decltype(smd::value_or(five, ifunc()))>);
    static_assert(
        std::same_as<int, decltype(smd::value_or(five, irfunc()))>);

    std::string longString{"A very long string that is not short at all and will allocate"};
    std::optional<std::string> os{"data"};
    std::optional<std::string> es{};

    std::string s1 = smd::value_or(os, testLVal(longString));
    std::string s2 = smd::value_or(es, testLVal(longString));

    static_assert(std::same_as<std::string , decltype(smd::value_or(os, testLVal(longString)))>);
    ASSERT_EQ(longString, testLVal(longString));

    std::string r1 = smd::value_or(es, testLVal(longString));
    std::string r2 = smd::value_or(os, testLVal(longString));

    ASSERT_EQ(r1, longString);
    ASSERT_EQ(r2, "data");

    r1 = "assign a string";
    r2 = "assign a different string";

    ASSERT_EQ(r1, "assign a string");
    ASSERT_EQ(longString, "A very long string that is not short at all and will allocate");
    ASSERT_NE(r2, "data");
    ASSERT_EQ(r2, "assign a different string");
    ASSERT_EQ(os, "data");

}

TEST(MaybeConceptsTest, ValueOrExpected) {
    std::expected<int, std::monostate> e;
    std::expected<int, std::monostate> five{5};

    const int fortytwo = 42;
    int       nine     = 9;

    // auto&& r1 = smd::reference_or(e, nine);
    // auto&& r2 = smd::reference_or(five, fortytwo);
    // auto&& r3 = smd::reference_or(five, ifunc());

    static_assert(std::same_as<int, decltype(smd::value_or(e, nine))>);
    static_assert(
        std::same_as<int, decltype(smd::value_or(five, fortytwo))>);
    static_assert(
        std::same_as<int, decltype(smd::value_or(five, ifunc()))>);
    static_assert(
        std::same_as<int, decltype(smd::value_or(five, irfunc()))>);

    std::string longString{"A very long string that is not short at all and will allocate"};
    std::expected<std::string, int> os{"data"};
    std::expected<std::string, int> es{std::unexpect, -1 };

    std::string s1 = smd::value_or(os, testLVal(longString));
    std::string s2 = smd::value_or(es, testLVal(longString));
    static_assert(std::same_as<std::string , decltype(smd::value_or(os, testLVal(longString)))>);

    std::string r1 = smd::value_or(es, testLVal(longString));
    std::string r2 = smd::value_or(os, testLVal(longString));

    ASSERT_EQ(r1, longString);
    ASSERT_EQ(r2, "data");

    r1 = "assign a string";
    r2 = "assign a different string";

    ASSERT_EQ(r1, "assign a string");
    ASSERT_EQ(longString, "A very long string that is not short at all and will allocate");
    ASSERT_NE(r2, "data");
    ASSERT_EQ(r2, "assign a different string");
    ASSERT_EQ(os, "data");

}

int returnNine(){return 9;}
int returnFortyTwo(){return 42;}

TEST(MaybeConceptsTest, OrInvoke) {
    std::optional<int> e;
    std::optional<int> five{5};

    const int fortytwo = 42;
    int       nine     = 9;

    auto r = smd::or_invoke(e, returnNine);

    static_assert(std::same_as<int, decltype(smd::or_invoke(e, returnNine))>);
    static_assert(
        std::same_as<int, decltype(smd::or_invoke(five, returnFortyTwo))>);
    static_assert(
        std::same_as<int, decltype(smd::or_invoke(five, ifunc))>);
    static_assert(
        std::same_as<int, decltype(smd::or_invoke(five, irfunc))>);

    std::string longString{"A very long string that is not short at all and will allocate"};
    std::optional<std::string> os{"data"};
    std::optional<std::string> es{};

    std::string s1 = smd::or_invoke(os, [&](){return testLVal(longString);});
    std::string s2 = smd::or_invoke(es, [&](){return testLVal(longString);});

    static_assert(std::same_as<std::string , decltype(smd::or_invoke(os, [&](){return testLVal(longString);}))>);

    std::string r1 = smd::or_invoke(es, [&](){return testLVal(longString);});
    std::string r2 = smd::or_invoke(os, [&](){return testLVal(longString);});

    ASSERT_EQ(r1, longString);
    ASSERT_EQ(r2, "data");

    r1 = "assign a string";
    r2 = "assign a different string";

    ASSERT_EQ(r1, "assign a string");
    ASSERT_EQ(longString, "A very long string that is not short at all and will allocate");
    ASSERT_NE(r2, "data");
    ASSERT_EQ(r2, "assign a different string");
    ASSERT_EQ(os, "data");

}
