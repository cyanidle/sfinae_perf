#include "type_traits"
#include <cstddef>
#include <utility>

template<typename T>
struct Has {
    void method(T);
};

template<typename T>
struct HasNot {

};

template<typename T>
struct HasGlobal {

};

template<typename T>
struct HasNotGlobal {

};

template<typename T>
void method(HasGlobal<T>, T);



// Victims

template <typename T, typename V> struct has_member_oldschool {
private:
    static int detect(...);
    template<typename U,
             typename ResultType = decltype(std::declval<U>().method(std::declval<V>()))
             >
    static constexpr ResultType detect(const U&);
public:
    static constexpr bool value = std::is_same<void, decltype(detect(std::declval<T>()))>::value;
};

template <typename T, typename V> struct has_global_oldschool {
private:
    static int detect(...);
    template<typename U,
             typename ResultType = decltype(method(std::declval<U>(), std::declval<V>()))
             >
    static constexpr ResultType detect(const U&);

public:
    static constexpr bool value = std::is_same<void, decltype(detect(std::declval<T>()))>::value;
};

// Short

auto has_method_short(...) -> std::false_type;
template<typename T, typename V>
auto has_method_short(T o, V v) -> decltype(o.method(v), std::true_type{});

auto has_global_short(...) -> std::false_type;
template<typename T, typename V>
auto has_global_short(T o, V v) -> decltype(method(o, v), std::true_type{});


// Classic

template<typename T, typename V, typename = void> struct has_method_classic : std::false_type {};
template<typename T, typename V>
struct has_method_classic<T, V, decltype(std::declval<T>().method(std::declval<V>()))> : std::true_type {};

template<typename T, typename V, typename = void> struct has_global_classic : std::false_type {};
template<typename T, typename V>
struct has_global_classic<T, V, decltype(method(std::declval<T>(), std::declval<V>()))> : std::true_type {};

// Checks

template<typename T, typename V>
struct check_member_oldschool : has_member_oldschool<T, V> {};

template<typename T, typename V>
struct check_global_oldschool : has_global_oldschool<T, V> {};

template<typename T, typename V>
struct check_member_short : decltype(has_method_short(std::declval<T>(), std::declval<V>())) {};

template<typename T, typename V>
struct check_global_short : decltype(has_global_short(std::declval<T>(), std::declval<V>())) {};

template<typename T, typename V>
struct check_member_classic : has_method_classic<T, V> {};

template<typename T, typename V>
struct check_global_classic : has_global_classic<T, V> {};

// Tests

template<typename...Ts>
struct TypeList {};

template<size_t I> struct ITag {};

template<
    template<typename, typename> class Check,
    template<typename, typename> class CheckGlobal,
    typename...Ts
    >
[[maybe_unused]] bool test_one(TypeList<Ts...>) {
    return (size_t(Check<Has<Ts>, Ts>::value) + ...)
    + (size_t(Check<HasNot<Ts>, Ts>::value) + ...)
    + (size_t(CheckGlobal<HasGlobal<Ts>, Ts>::value) + ...)
    + (size_t(CheckGlobal<HasNotGlobal<Ts>, Ts>::value) + ...);
}

template<size_t offs, size_t...Is>
auto generate_input(std::index_sequence<Is...>) -> TypeList<ITag<Is + offs>...>;

template<size_t offs, size_t Is>
auto generate_input() -> decltype(generate_input<offs>(std::make_index_sequence<Is>{}));

constexpr auto InputSize = 256;

static_assert(check_member_oldschool<Has<int>, int>::value);
static_assert(!check_member_oldschool<HasNot<int>, int>::value);
static_assert(!check_member_oldschool<HasGlobal<int>, int>::value);
static_assert(!check_member_oldschool<HasNotGlobal<int>, int>::value);

static_assert(!check_global_oldschool<Has<int>, int>::value);
static_assert(!check_global_oldschool<HasNot<int>, int>::value);
static_assert(check_global_oldschool<HasGlobal<int>, int>::value);
static_assert(!check_global_oldschool<HasNotGlobal<int>, int>::value);

static_assert(check_member_classic<Has<int>, int>::value);
static_assert(!check_member_classic<HasNot<int>, int>::value);
static_assert(!check_member_classic<HasGlobal<int>, int>::value);
static_assert(!check_member_classic<HasNotGlobal<int>, int>::value);

static_assert(!check_global_classic<Has<int>, int>::value);
static_assert(!check_global_classic<HasNot<int>, int>::value);
static_assert(check_global_classic<HasGlobal<int>, int>::value);
static_assert(!check_global_classic<HasNotGlobal<int>, int>::value);

static_assert(check_member_short<Has<int>, int>::value);
static_assert(!check_member_short<HasNot<int>, int>::value);
static_assert(!check_member_short<HasGlobal<int>, int>::value);
static_assert(!check_member_short<HasNotGlobal<int>, int>::value);

static_assert(!check_global_short<Has<int>, int>::value);
static_assert(!check_global_short<HasNot<int>, int>::value);
static_assert(check_global_short<HasGlobal<int>, int>::value);
static_assert(!check_global_short<HasNotGlobal<int>, int>::value);


using Input1 = decltype(generate_input<0, InputSize>());
using Input2 = decltype(generate_input<1000, InputSize>());
using Input3 = decltype(generate_input<2000, InputSize>());
using Input4 = decltype(generate_input<3000, InputSize>());
using Input5 = decltype(generate_input<4000, InputSize>());

void test_oldschool() {
    test_one<check_member_oldschool, check_global_oldschool>(Input1{});
    test_one<check_member_oldschool, check_global_oldschool>(Input2{});
    test_one<check_member_oldschool, check_global_oldschool>(Input3{});
    test_one<check_member_oldschool, check_global_oldschool>(Input4{});
    test_one<check_member_oldschool, check_global_oldschool>(Input5{});
}

void test_classic() {
    test_one<check_member_classic, check_global_classic>(Input1{});
    test_one<check_member_classic, check_global_classic>(Input2{});
    test_one<check_member_classic, check_global_classic>(Input3{});
    test_one<check_member_classic, check_global_classic>(Input4{});
    test_one<check_member_classic, check_global_classic>(Input5{});
}

void test_short() {
    test_one<check_member_short, check_global_short>(Input1{});
    test_one<check_member_short, check_global_short>(Input2{});
    test_one<check_member_short, check_global_short>(Input3{});
    test_one<check_member_short, check_global_short>(Input4{});
    test_one<check_member_short, check_global_short>(Input5{});
}

int main() {}
