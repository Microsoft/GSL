///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <gsl/assert>  // for contracts, fail_fast (ptr only)
#include <gsl/pointers>    // for owner
#include <gsl/span>        // for span, dynamic_extent
#include <gsl/string_span> // for basic_string_span, operator==, ensure_z

#include <algorithm>   // for move, find
#include <cstddef>     // for size_t
#include <map>         // for map
#include <string>      // for basic_string, string, char_traits, operat...
#include <type_traits> // for remove_reference<>::type
#include <vector>      // for vector, allocator

using namespace std;
using namespace gsl;

namespace
{
static constexpr char deathstring[] = "Expected Death";
}
// Generic string functions

namespace generic
{

template <typename CharT>
auto strlen(const CharT* s)
{
    auto p = s;
    while (*p) ++p;
    return p - s;
}

template <typename CharT>
auto strnlen(const CharT* s, std::size_t n)
{
    return std::find(s, s + n, CharT{0}) - s;
}

} // namespace generic

namespace
{

template <typename T>
T move_wrapper(T&& t)
{
    return std::move(t);
}

// not used otherwise
#ifdef CONFIRM_COMPILATION_ERRORS

template <class T>
T create()
{
    return T{};
}

template <class T>
void use(basic_string_span<T, gsl::dynamic_extent>)
{
}
#endif

czstring_span<> CreateTempName(string_span<> span)
{
    Expects(span.size() > 1, Testing);

    std::size_t last = 0;
    if (span.size() > 4) {
        span[0] = 't';
        span[1] = 'm';
        span[2] = 'p';
        last = 3;
    }
    span[last] = '\0';

    auto ret = span.subspan(0, 4);
    return {ret};
}

cwzstring_span<> CreateTempNameW(wstring_span<> span)
{
    Expects(span.size() > 1, Testing);

    std::size_t last = 0;
    if (span.size() > 4) {
        span[0] = L't';
        span[1] = L'm';
        span[2] = L'p';
        last = 3;
    }
    span[last] = L'\0';

    auto ret = span.subspan(0, 4);
    return {ret};
}

cu16zstring_span<> CreateTempNameU16(u16string_span<> span)
{
    Expects(span.size() > 1, Testing);

    std::size_t last = 0;
    if (span.size() > 4) {
        span[0] = u't';
        span[1] = u'm';
        span[2] = u'p';
        last = 3;
    }
    span[last] = u'\0';

    auto ret = span.subspan(0, 4);
    return {ret};
}

cu32zstring_span<> CreateTempNameU32(u32string_span<> span)
{
    Expects(span.size() > 1, Testing);

    std::size_t last = 0;
    if (span.size() > 4) {
        span[0] = U't';
        span[1] = U'm';
        span[2] = U'p';
        last = 3;
    }
    span[last] = U'\0';

    auto ret = span.subspan(0, 4);
    return {ret};
}
} // namespace

TEST(string_span_tests, TestLiteralConstruction)
{
    cwstring_span<> v = ensure_z(L"Hello");
    EXPECT_TRUE(5 == v.length());

#ifdef CONFIRM_COMPILATION_ERRORS
    wstring_span<> v2 = ensure0(L"Hello");
#endif
}

TEST(string_span_tests, TestConstructFromStdString)
{
    std::string s = "Hello there world";
    cstring_span<> v = s;
    EXPECT_TRUE(v.length() == static_cast<cstring_span<>::size_type>(s.length()));
}

TEST(string_span_tests, TestConstructFromStdVector)
{
    std::vector<char> vec(5, 'h');
    string_span<> v{vec};
    EXPECT_TRUE(v.length() == static_cast<string_span<>::size_type>(vec.size()));
}

TEST(string_span_tests, TestStackArrayConstruction)
{
    wchar_t stack_string[] = L"Hello";

    {
        cwstring_span<> v = ensure_z(stack_string);
        EXPECT_TRUE(v.length() == 5);
    }

    {
        cwstring_span<> v = stack_string;
        EXPECT_TRUE(v.length() == 5);
    }

    {
        wstring_span<> v = ensure_z(stack_string);
        EXPECT_TRUE(v.length() == 5);
    }

    {
        wstring_span<> v = stack_string;
        EXPECT_TRUE(v.length() == 5);
    }
}

TEST(string_span_tests, TestConstructFromConstCharPointer)
{
    const char* s = "Hello";
    cstring_span<> v = ensure_z(s);
    EXPECT_TRUE(v.length() == 5);
}

TEST(string_span_tests, TestConversionToConst)
{
    char stack_string[] = "Hello";
    string_span<> v = ensure_z(stack_string);
    cstring_span<> v2 = v;
    EXPECT_TRUE(v.length() == v2.length());
}

TEST(string_span_tests, TestConversionFromConst)
{
    char stack_string[] = "Hello";
    cstring_span<> v = ensure_z(stack_string);
    (void) v;
#ifdef CONFIRM_COMPILATION_ERRORS
    string_span<> v2 = v;
    string_span<> v3 = "Hello";
#endif
}

TEST(string_span_tests, TestToString)
{
    auto s = gsl::to_string(cstring_span<>{});
    EXPECT_TRUE(s.length() == static_cast<size_t>(0));

    char stack_string[] = "Hello";
    cstring_span<> v = ensure_z(stack_string);
    auto s2 = gsl::to_string(v);
    EXPECT_TRUE(static_cast<cstring_span<>::size_type>(s2.length()) == v.length());
    EXPECT_TRUE(s2.length() == static_cast<size_t>(5));
}

TEST(string_span_tests, TestToBasicString)
{
    auto s = gsl::to_basic_string<char, std::char_traits<char>, ::std::allocator<char>>(
        cstring_span<>{});
    EXPECT_TRUE(s.length() == static_cast<size_t>(0));

    char stack_string[] = "Hello";
    cstring_span<> v = ensure_z(stack_string);
    auto s2 = gsl::to_basic_string<char, std::char_traits<char>, ::std::allocator<char>>(v);
    EXPECT_TRUE(static_cast<cstring_span<>::size_type>(s2.length()) == v.length());
    EXPECT_TRUE(s2.length() == static_cast<size_t>(5));
}

TEST(string_span_tests, EqualityAndImplicitConstructors)
{
    {
        cstring_span<> span = "Hello";
        cstring_span<> span1;

        // comparison to empty span
        EXPECT_TRUE(span1 != span);
        EXPECT_TRUE(span != span1);
    }

    {
        cstring_span<> span = "Hello";
        cstring_span<> span1 = "Hello1";

        // comparison to different span
        EXPECT_TRUE(span1 != span);
        EXPECT_TRUE(span != span1);
    }

    {
        cstring_span<> span = "Hello";

        const char ar[] = {'H', 'e', 'l', 'l', 'o'};
        const char ar1[] = "Hello";
        const char ar2[10] = "Hello";
        const char* ptr = "Hello";
        const std::string str = "Hello";
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        gsl::span<const char> sp = ensure_z("Hello");

        // comparison to  literal
        EXPECT_TRUE(span == cstring_span<>("Hello"));

        // comparison to static array with no null termination
        EXPECT_TRUE(span == cstring_span<>(ar));

        // comparison to static array with null at the end
        EXPECT_TRUE(span == cstring_span<>(ar1));

        // comparison to static array with null in the middle
        EXPECT_TRUE(span == cstring_span<>(ar2));

        // comparison to null-terminated c string
        EXPECT_TRUE(span == cstring_span<>(ptr, 5));

        // comparison to string
        EXPECT_TRUE(span == cstring_span<>(str));

        // comparison to vector of charaters with no null termination
        EXPECT_TRUE(span == cstring_span<>(vec));

        // comparison to span
        EXPECT_TRUE(span == cstring_span<>(sp));

        // comparison to string_span
        EXPECT_TRUE(span == span);
    }

    {
        char ar[] = {'H', 'e', 'l', 'l', 'o'};

        string_span<> span = ar;

        char ar1[] = "Hello";
        char ar2[10] = "Hello";
        char* ptr = ar;
        std::string str = "Hello";
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        gsl::span<char> sp = ensure_z(ar1);

        // comparison to static array with no null termination
        EXPECT_TRUE(span == string_span<>(ar));

        // comparison to static array with null at the end
        EXPECT_TRUE(span == string_span<>(ar1));

        // comparison to static array with null in the middle
        EXPECT_TRUE(span == string_span<>(ar2));

        // comparison to null-terminated c string
        EXPECT_TRUE(span == string_span<>(ptr, 5));

        // comparison to string
        EXPECT_TRUE(span == string_span<>(str));

        // comparison to vector of charaters with no null termination
        EXPECT_TRUE(span == string_span<>(vec));

        // comparison to span
        EXPECT_TRUE(span == string_span<>(sp));

        // comparison to string_span
        EXPECT_TRUE(span == span);
    }

    {
        const char ar[] = {'H', 'e', 'l', 'l', 'o'};
        const char ar1[] = "Hello";
        const char ar2[10] = "Hello";
        const std::string str = "Hello";
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const gsl::span<const char> sp = ensure_z("Hello");

        cstring_span<> span = "Hello";

        // const span, const other type

        EXPECT_TRUE(span == "Hello");
        EXPECT_TRUE(span == ar);
        EXPECT_TRUE(span == ar1);
        EXPECT_TRUE(span == ar2);
#ifdef CONFIRM_COMPILATION_ERRORS
        const char* ptr = "Hello";
        EXPECT_TRUE(span == ptr);
#endif
        EXPECT_TRUE(span == str);
        EXPECT_TRUE(span == vec);
        EXPECT_TRUE(span == sp);

        EXPECT_TRUE("Hello" == span);
        EXPECT_TRUE(ar == span);
        EXPECT_TRUE(ar1 == span);
        EXPECT_TRUE(ar2 == span);
#ifdef CONFIRM_COMPILATION_ERRORS
        EXPECT_TRUE(ptr == span);
#endif
        EXPECT_TRUE(str == span);
        EXPECT_TRUE(vec == span);
        EXPECT_TRUE(sp == span);

        // const span, non-const other type

        char _ar[] = {'H', 'e', 'l', 'l', 'o'};
        char _ar1[] = "Hello";
        char _ar2[10] = "Hello";
        char* _ptr = _ar;
        std::string _str = "Hello";
        std::vector<char> _vec = {'H', 'e', 'l', 'l', 'o'};
        gsl::span<char> _sp{_ar, 5};

        EXPECT_TRUE(span == _ar);
        EXPECT_TRUE(span == _ar1);
        EXPECT_TRUE(span == _ar2);
#ifdef CONFIRM_COMPILATION_ERRORS
        EXPECT_TRUE(span == _ptr);
#endif
        EXPECT_TRUE(span == _str);
        EXPECT_TRUE(span == _vec);
        EXPECT_TRUE(span == _sp);

        EXPECT_TRUE(_ar == span);
        EXPECT_TRUE(_ar1 == span);
        EXPECT_TRUE(_ar2 == span);
#ifdef CONFIRM_COMPILATION_ERRORS
        EXPECT_TRUE(_ptr == span);
#endif
        EXPECT_TRUE(_str == span);
        EXPECT_TRUE(_vec == span);
        EXPECT_TRUE(_sp == span);

        string_span<> _span{_ptr, 5};

        // non-const span, non-const other type

        EXPECT_TRUE(_span == _ar);
        EXPECT_TRUE(_span == _ar1);
        EXPECT_TRUE(_span == _ar2);
#ifdef CONFIRM_COMPILATION_ERRORS
        EXPECT_TRUE(_span == _ptr);
#endif
        EXPECT_TRUE(_span == _str);
        EXPECT_TRUE(_span == _vec);
        EXPECT_TRUE(_span == _sp);

        EXPECT_TRUE(_ar == _span);
        EXPECT_TRUE(_ar1 == _span);
        EXPECT_TRUE(_ar2 == _span);
#ifdef CONFIRM_COMPILATION_ERRORS
        EXPECT_TRUE(_ptr == _span);
#endif
        EXPECT_TRUE(_str == _span);
        EXPECT_TRUE(_vec == _span);
        EXPECT_TRUE(_sp == _span);

        // non-const span, const other type

        EXPECT_TRUE(_span == "Hello");
        EXPECT_TRUE(_span == ar);
        EXPECT_TRUE(_span == ar1);
        EXPECT_TRUE(_span == ar2);
#ifdef CONFIRM_COMPILATION_ERRORS
        EXPECT_TRUE(_span == ptr);
#endif
        EXPECT_TRUE(_span == str);
        EXPECT_TRUE(_span == vec);
        EXPECT_TRUE(_span == sp);

        EXPECT_TRUE("Hello" == _span);
        EXPECT_TRUE(ar == _span);
        EXPECT_TRUE(ar1 == _span);
        EXPECT_TRUE(ar2 == _span);
#ifdef CONFIRM_COMPILATION_ERRORS
        EXPECT_TRUE(ptr == _span);
#endif
        EXPECT_TRUE(str == _span);
        EXPECT_TRUE(vec == _span);
        EXPECT_TRUE(sp == _span);

        // two spans

        EXPECT_TRUE(_span == span);
        EXPECT_TRUE(span == _span);
    }

    {
        std::vector<char> str1 = {'H', 'e', 'l', 'l', 'o'};
        cstring_span<> span1 = str1;
        std::vector<char> str2 = std::move(str1);
        cstring_span<> span2 = str2;

        // comparison of spans from the same vector before and after move (ok)
        EXPECT_TRUE(span1 == span2);
    }
}

TEST(string_span_tests, ComparisonAndImplicitConstructors)
{
    {
        cstring_span<> span = "Hello";

        const char ar[] = {'H', 'e', 'l', 'l', 'o'};
        const char ar1[] = "Hello";
        const char ar2[10] = "Hello";
        const char* ptr = "Hello";
        const std::string str = "Hello";
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};

        // comparison to  literal
        EXPECT_TRUE(span < cstring_span<>("Helloo"));
        EXPECT_TRUE(span > cstring_span<>("Hell"));

        // comparison to static array with no null termination
        EXPECT_TRUE(span >= cstring_span<>(ar));

        // comparison to static array with null at the end
        EXPECT_TRUE(span <= cstring_span<>(ar1));

        // comparison to static array with null in the middle
        EXPECT_TRUE(span >= cstring_span<>(ar2));

        // comparison to null-terminated c string
        EXPECT_TRUE(span <= cstring_span<>(ptr, 5));

        // comparison to string
        EXPECT_TRUE(span >= cstring_span<>(str));

        // comparison to vector of charaters with no null termination
        EXPECT_TRUE(span <= cstring_span<>(vec));
    }

    {
        char ar[] = {'H', 'e', 'l', 'l', 'o'};

        string_span<> span = ar;

        char larr[] = "Hell";
        char rarr[] = "Helloo";

        char ar1[] = "Hello";
        char ar2[10] = "Hello";
        char* ptr = ar;
        std::string str = "Hello";
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};

        // comparison to static array with no null termination
        EXPECT_TRUE(span <= string_span<>(ar));
        EXPECT_TRUE(span < string_span<>(rarr));
        EXPECT_TRUE(span > string_span<>(larr));

        // comparison to static array with null at the end
        EXPECT_TRUE(span >= string_span<>(ar1));

        // comparison to static array with null in the middle
        EXPECT_TRUE(span <= string_span<>(ar2));

        // comparison to null-terminated c string
        EXPECT_TRUE(span >= string_span<>(ptr, 5));

        // comparison to string
        EXPECT_TRUE(span <= string_span<>(str));

        // comparison to vector of charaters with no null termination
        EXPECT_TRUE(span >= string_span<>(vec));
    }
}

TEST(string_span_tests, ConstrutorsEnsureZ)
{
    // remove z from literals
    {
        cstring_span<> sp = "hello";
        EXPECT_TRUE(sp.length() == 5);
    }

    // take the string as is
    {
        auto str = std::string("hello");
        cstring_span<> sp = str;
        EXPECT_TRUE(sp.length() == 5);
    }

    // ensure z on c strings
    {
        gsl::owner<char*> ptr = new char[3];

        ptr[0] = 'a';
        ptr[1] = 'b';
        ptr[2] = '\0';

        string_span<> span = ensure_z(ptr);
        EXPECT_TRUE(span.length() == 2);

        delete[] ptr;
    }
}

TEST(string_span_tests, Constructors)
{
    // creating cstring_span

    // from span of a final extent
    {
        span<const char, 6> sp = "Hello";
        cstring_span<> span = sp;
        EXPECT_TRUE(span.length() == 6);
    }

// from const span of a final extent to non-const string_span
#ifdef CONFIRM_COMPILATION_ERRORS
    {
        span<const char, 6> sp = "Hello";
        string_span<> span = sp;
        EXPECT_TRUE(span.length() == 6);
    }
#endif

// from string temporary
#ifdef CONFIRM_COMPILATION_ERRORS
    {
        cstring_span<> span = std::string("Hello");
    }
#endif

    // default
    {
        cstring_span<> span;
        EXPECT_TRUE(span.length() == 0);
    }

    // from string literal
    {
        cstring_span<> span = "Hello";
        EXPECT_TRUE(span.length() == 5);
    }

    // from const static array
    {
        const char ar[] = {'H', 'e', 'l', 'l', 'o'};
        cstring_span<> span = ar;
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const static array
    {
        char ar[] = {'H', 'e', 'l', 'l', 'o'};
        cstring_span<> span = ar;
        EXPECT_TRUE(span.length() == 5);
    }

    // from const ptr and length
    {
        const char* ptr = "Hello";
        cstring_span<> span{ptr, 5};
        EXPECT_TRUE(span.length() == 5);
    }

    // from const ptr and length, include 0
    {
        const char* ptr = "Hello";
        cstring_span<> span{ptr, 6};
        EXPECT_TRUE(span.length() == 6);
    }

    // from const ptr and length, 0 inside
    {
        const char* ptr = "He\0lo";
        cstring_span<> span{ptr, 5};
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const ptr and length
    {
        char ar[] = {'H', 'e', 'l', 'l', 'o'};
        char* ptr = ar;
        cstring_span<> span{ptr, 5};
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const ptr and length, 0 inside
    {
        char ar[] = {'H', 'e', '\0', 'l', 'o'};
        char* ptr = ar;
        cstring_span<> span{ptr, 5};
        EXPECT_TRUE(span.length() == 5);
    }

    // from const string
    {
        const std::string str = "Hello";
        const cstring_span<> span = str;
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const string
    {
        std::string str = "Hello";
        const cstring_span<> span = str;
        EXPECT_TRUE(span.length() == 5);
    }

    // from const vector
    {
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const cstring_span<> span = vec;
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const vector
    {
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const cstring_span<> span = vec;
        EXPECT_TRUE(span.length() == 5);
    }

    // from const span
    {
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const span<const char> inner = vec;
        const cstring_span<> span = inner;
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const span
    {
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const span<char> inner = vec;
        const cstring_span<> span = inner;
        EXPECT_TRUE(span.length() == 5);
    }

    // from const string_span
    {
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const cstring_span<> tmp = vec;
        const cstring_span<> span = tmp;
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const string_span
    {
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        string_span<> tmp = vec;
        cstring_span<> span = tmp;
        EXPECT_TRUE(span.length() == 5);
    }

    // creating string_span

    // from string literal
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        string_span<> span = "Hello";
#endif
    }

    // from const static array
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        const char ar[] = {'H', 'e', 'l', 'l', 'o'};
        string_span<> span = ar;
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from non-const static array
    {
        char ar[] = {'H', 'e', 'l', 'l', 'o'};
        string_span<> span = ar;
        EXPECT_TRUE(span.length() == 5);
    }

    // from const ptr and length
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        const char* ptr = "Hello";
        string_span<> span{ptr, 5};
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from non-const ptr and length
    {
        char ar[] = {'H', 'e', 'l', 'l', 'o'};
        char* ptr = ar;
        string_span<> span{ptr, 5};
        EXPECT_TRUE(span.length() == 5);
    }

    // from const string
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        const std::string str = "Hello";
        string_span<> span = str;
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from non-const string
    {
        std::string str = "Hello";
        string_span<> span = str;
        EXPECT_TRUE(span.length() == 5);
    }

    // from const vector
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        string_span<> span = vec;
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from non-const vector
    {
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        string_span<> span = vec;
        EXPECT_TRUE(span.length() == 5);
    }

    // from const span
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const span<const char> inner = vec;
        string_span<> span = inner;
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from non-const span
    {
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        span<char> inner = vec;
        string_span<> span = inner;
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const span of non-const data from const vector
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const span<char> inner = vec;
        string_span<> span = inner;
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from const string_span
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        cstring_span<> tmp = vec;
        string_span<> span = tmp;
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from non-const string_span
    {
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const string_span<> tmp = vec;
        const string_span<> span = tmp;
        EXPECT_TRUE(span.length() == 5);
    }

    // from non-const string_span from const vector
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        const std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        string_span<> tmp = vec;
        string_span<> span = tmp;
        EXPECT_TRUE(span.length() == 5);
#endif
    }

    // from const string_span of non-const data
    {
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        const string_span<> tmp = vec;
        const string_span<> span = tmp;
        EXPECT_TRUE(span.length() == 5);
    }
}

TEST(string_span_tests, MoveConstructors)
{
    // move string_span
    {
        cstring_span<> span = "Hello";
        const auto span1 = std::move(span);
        EXPECT_TRUE(span1.length() == 5);
    }
    {
        cstring_span<> span = "Hello";
        const auto span1 = move_wrapper(std::move(span));
        EXPECT_TRUE(span1.length() == 5);
    }
    {
        cstring_span<> span = "Hello";
        const auto span1 = move_wrapper(std::move(span));
        EXPECT_TRUE(span1.length() == 5);
    }

    // move span
    {
        span<const char> span = ensure_z("Hello");
        const cstring_span<> span1 = std::move(span);
        EXPECT_TRUE(span1.length() == 5);
    }
    {
        span<const char> span = ensure_z("Hello");
        const cstring_span<> span2 = move_wrapper(std::move(span));
        EXPECT_TRUE(span2.length() == 5);
    }

    // move string
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        std::string str = "Hello";
        string_span<> span = std::move(str);
        EXPECT_TRUE(span.length() == 5);
#endif
    }
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        std::string str = "Hello";
        string_span<> span = move_wrapper<std::string>(std::move(str));
        EXPECT_TRUE(span.length() == 5);
#endif
    }
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        use<char>(create<string>());
#endif
    }

    // move container
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        string_span<> span = std::move(vec);
        EXPECT_TRUE(span.length() == 5);
#endif
    }
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        std::vector<char> vec = {'H', 'e', 'l', 'l', 'o'};
        string_span<> span = move_wrapper<std::vector<char>>(std::move(vec));
        EXPECT_TRUE(span.length() == 5);
#endif
    }
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        use<char>(create<std::vector<char>>());
#endif
    }
}

TEST(string_span_tests, Conversion)
{
#ifdef CONFIRM_COMPILATION_ERRORS
    cstring_span<> span = "Hello";
    cwstring_span<> wspan{span};
    EXPECT_TRUE(wspan.length() == 5);
#endif
}

TEST(string_span_tests, zstring)
{
    std::set_terminate([] {
        std::cerr << "Expected Death. zstring";
        std::abort();
    });

    // create zspan from zero terminated string
    {
        char buf[1];
        buf[0] = '\0';

        zstring_span<> zspan({buf, 1});

        EXPECT_TRUE(generic::strlen(zspan.assume_z()) == 0);
        EXPECT_TRUE(zspan.as_string_span().size() == 0);
        EXPECT_TRUE(zspan.ensure_z().size() == 0);
    }

    // create zspan from non-zero terminated string
    {
        char buf[1];
        buf[0] = 'a';

        auto workaround_macro = [&]() { const zstring_span<> zspan({buf, 1}); };
        EXPECT_DEATH(workaround_macro(), deathstring);
    }

    // usage scenario: create zero-terminated temp file name and pass to a legacy API
    {
        char buf[10];

        auto name = CreateTempName({buf, 10});
        if (!name.empty()) {
            czstring<> str = name.assume_z();
            EXPECT_TRUE(generic::strlen(str) == 3);
            EXPECT_TRUE(*(str + 3) == '\0');
        }
    }
}

TEST(string_span_tests, wzstring)
{
    std::set_terminate([] {
        std::cerr << "Expected Death. wzstring";
        std::abort();
    });

    // create zspan from zero terminated string
    {
        wchar_t buf[1];
        buf[0] = L'\0';

        wzstring_span<> zspan({buf, 1});

        EXPECT_TRUE(generic::strnlen(zspan.assume_z(), 1) == 0);
        EXPECT_TRUE(zspan.as_string_span().size() == 0);
        EXPECT_TRUE(zspan.ensure_z().size() == 0);
    }

    // create zspan from non-zero terminated string
    {
        wchar_t buf[1];
        buf[0] = L'a';

        const auto workaround_macro = [&]() { const wzstring_span<> zspan({buf, 1}); };
        EXPECT_DEATH(workaround_macro(), deathstring);
    }

    // usage scenario: create zero-terminated temp file name and pass to a legacy API
    {
        wchar_t buf[10];

        const auto name = CreateTempNameW({buf, 10});
        if (!name.empty()) {
            cwzstring<> str = name.assume_z();
            EXPECT_TRUE(generic::strnlen(str, 10) == 3);
            EXPECT_TRUE(*(str + 3) == L'\0');
        }
    }
}

TEST(string_span_tests, u16zstring)
{
    std::set_terminate([] {
        std::cerr << "Expected Death. u16zstring";
        std::abort();
    });

    // create zspan from zero terminated string
    {
        char16_t buf[1];
        buf[0] = L'\0';

        u16zstring_span<> zspan({buf, 1});

        EXPECT_TRUE(generic::strnlen(zspan.assume_z(), 1) == 0);
        EXPECT_TRUE(zspan.as_string_span().size() == 0);
        EXPECT_TRUE(zspan.ensure_z().size() == 0);
    }

    // create zspan from non-zero terminated string
    {
        char16_t buf[1];
        buf[0] = u'a';

        const auto workaround_macro = [&]() { const u16zstring_span<> zspan({buf, 1}); };
        EXPECT_DEATH(workaround_macro(), deathstring);
    }

    // usage scenario: create zero-terminated temp file name and pass to a legacy API
    {
        char16_t buf[10];

        const auto name = CreateTempNameU16({buf, 10});
        if (!name.empty()) {
            cu16zstring<> str = name.assume_z();
            EXPECT_TRUE(generic::strnlen(str, 10) == 3);
            EXPECT_TRUE(*(str + 3) == L'\0');
        }
    }
}

TEST(string_span_tests, u32zstring)
{
    std::set_terminate([] {
        std::cerr << "Expected Death. u31zstring";
        std::abort();
    });

    // create zspan from zero terminated string
    {
        char32_t buf[1];
        buf[0] = L'\0';

        u32zstring_span<> zspan({buf, 1});

        EXPECT_TRUE(generic::strnlen(zspan.assume_z(), 1) == 0);
        EXPECT_TRUE(zspan.as_string_span().size() == 0);
        EXPECT_TRUE(zspan.ensure_z().size() == 0);
    }

    // create zspan from non-zero terminated string
    {
        char32_t buf[1];
        buf[0] = u'a';

        const auto workaround_macro = [&]() { const u32zstring_span<> zspan({buf, 1}); };
        EXPECT_DEATH(workaround_macro(), deathstring);
    }

    // usage scenario: create zero-terminated temp file name and pass to a legacy API
    {
        char32_t buf[10];

        const auto name = CreateTempNameU32({buf, 10});
        if (!name.empty()) {
            cu32zstring<> str = name.assume_z();
            EXPECT_TRUE(generic::strnlen(str, 10) == 3);
            EXPECT_TRUE(*(str + 3) == L'\0');
        }
    }
}

TEST(string_span_tests, Issue305)
{
    std::map<gsl::cstring_span<>, int> foo = {{"foo", 0}, {"bar", 1}};
    EXPECT_TRUE(foo["foo"] == 0);
    EXPECT_TRUE(foo["bar"] == 1);
}

TEST(string_span_tests, char16_t_type)
{
    gsl::cu16string_span<> ss1 = gsl::ensure_z(u"abc");
    EXPECT_TRUE(ss1.size() == 3);
    EXPECT_TRUE(ss1.size_bytes() == 6);

    std::u16string s1 = gsl::to_string(ss1);
    EXPECT_TRUE(s1 == u"abc");

    std::u16string s2 = u"abc";
    gsl::u16string_span<> ss2 = s2;
    EXPECT_TRUE(ss2.size() == 3);

    gsl::u16string_span<> ss3 = ss2.subspan(1, 1);
    EXPECT_TRUE(ss3.size() == 1);
    EXPECT_TRUE(ss3[0] == u'b');

    char16_t buf[4]{u'a', u'b', u'c', u'\0'};
    gsl::u16string_span<> ss4{buf, 4};
    EXPECT_TRUE(ss4[3] == u'\0');

    gsl::cu16zstring_span<> ss5(u"abc");
    EXPECT_TRUE((ss5.as_string_span().size()) == 3);

    gsl::cu16string_span<> ss6 = ss5.as_string_span();
    EXPECT_TRUE(ss6 == ss1);

    std::vector<char16_t> v7 = {u'a', u'b', u'c'};
    gsl::cu16string_span<> ss7{v7};
    EXPECT_TRUE(ss7 == ss1);

    gsl::cu16string_span<> ss8 = gsl::ensure_z(u"abc");
    gsl::cu16string_span<> ss9 = gsl::ensure_z(u"abc");
    EXPECT_TRUE(ss8 == ss9);

    ss9 = gsl::ensure_z(u"abd");
    EXPECT_TRUE(ss8 < ss9);
    EXPECT_TRUE(ss8 <= ss9);
    EXPECT_TRUE(ss8 != ss9);
}

TEST(string_span_tests, char32_t_type)
{
    gsl::cu32string_span<> ss1 = gsl::ensure_z(U"abc");
    EXPECT_TRUE(ss1.size() == 3);
    EXPECT_TRUE(ss1.size_bytes() == 12);

    std::u32string s1 = gsl::to_string(ss1);
    EXPECT_TRUE(s1 == U"abc");

    std::u32string s2 = U"abc";
    gsl::u32string_span<> ss2 = s2;
    EXPECT_TRUE(ss2.size() == 3);

    gsl::u32string_span<> ss3 = ss2.subspan(1, 1);
    EXPECT_TRUE(ss3.size() == 1);
    EXPECT_TRUE(ss3[0] == U'b');

    char32_t buf[4]{U'a', U'b', U'c', U'\0'};
    gsl::u32string_span<> ss4{buf, 4};
    EXPECT_TRUE(ss4[3] == u'\0');

    gsl::cu32zstring_span<> ss5(U"abc");
    EXPECT_TRUE(ss5.as_string_span().size() == 3);

    gsl::cu32string_span<> ss6 = ss5.as_string_span();
    EXPECT_TRUE(ss6 == ss1);

    gsl::cu32string_span<> ss8 = gsl::ensure_z(U"abc");
    gsl::cu32string_span<> ss9 = gsl::ensure_z(U"abc");
    EXPECT_TRUE(ss8 == ss9);

    ss9 = gsl::ensure_z(U"abd");
    EXPECT_TRUE(ss8 < ss9);
    EXPECT_TRUE(ss8 <= ss9);
    EXPECT_TRUE(ss8 != ss9);
}

TEST(string_span_tests, as_bytes)
{
    cwzstring_span<> v(L"qwerty");
    const auto s = v.as_string_span();
    const auto bs = as_bytes(s);
    EXPECT_TRUE(static_cast<const void*>(bs.data()) == static_cast<const void*>(s.data()));
    EXPECT_TRUE(bs.size() == s.size_bytes());
}

TEST(string_span_tests, as_writable_bytes)
{
    wchar_t buf[]{L"qwerty"};
    wzstring_span<> v(buf);
    const auto s = v.as_string_span();
    const auto bs = as_writable_bytes(s);
    EXPECT_TRUE(static_cast<const void*>(bs.data()) == static_cast<const void*>(s.data()));
    EXPECT_TRUE(bs.size() == s.size_bytes());
}
