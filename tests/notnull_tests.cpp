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

#include <UnitTest++/UnitTest++.h> 
#include <gsl/gsl>
#include <vector>
#include <memory>

using namespace gsl;

struct MyBase {};
struct MyDerived : public MyBase {};
struct Unrelated {};

// stand-in for a user-defined ref-counted class
template<typename T>
struct RefCounted
{
    RefCounted(T* p) : p_(p) {}
    operator T*() { return p_; }
    T* p_;
};

SUITE(NotNullTests)
{

    bool helper(not_null<int*> p)
    {
        return *p == 12;
    }

    TEST(TestNotNullConstructors)
    {
#ifdef CONFIRM_COMPILATION_ERRORS
        not_null<int*> p = nullptr; // yay...does not compile!
        not_null<std::vector<char>*> p = 0; // yay...does not compile!
        not_null<int*> p; // yay...does not compile!
        std::unique_ptr<int> up = std::make_unique<int>(120);
        not_null<int*> p = up;

        // Forbid non-nullptr assignable types
        not_null<std::vector<int>> f(std::vector<int>{1});
        not_null<int> z(10);
        not_null<std::vector<int>> y({1,2});
#endif
      int i = 12; 
      auto rp = RefCounted<int>(&i);
      not_null<int*> p(rp);
      CHECK(p.get() == &i);

      not_null<std::shared_ptr<int>> x(std::make_shared<int>(10)); // shared_ptr<int> is nullptr assignable
    }

    TEST(TestNotNullCasting)
    {
        MyBase base;
	MyDerived derived;
	Unrelated unrelated;
	not_null<Unrelated*> u = &unrelated;
        (void)u;
	not_null<MyDerived*> p = &derived;
        not_null<MyBase*> q = &base;
	q = p; // allowed with heterogeneous copy ctor
        CHECK(q == p);

#ifdef CONFIRM_COMPILATION_ERRORS
	q = u; // no viable conversion possible between MyBase* and Unrelated*
	p = q; // not possible to implicitly convert MyBase* to MyDerived*

        not_null<Unrelated*> r = p;
        not_null<Unrelated*> s = reinterpret_cast<Unrelated*>(p);
#endif
        not_null<Unrelated*> t = reinterpret_cast<Unrelated*>(p.get());
        CHECK(reinterpret_cast<void*>(p.get()) == reinterpret_cast<void*>(t.get()));
    }

    TEST(TestNotNullAssignment)
    {
        int i = 12;
        not_null<int*> p = &i; 
        CHECK(helper(p));

        int* q = nullptr;
        CHECK_THROW(p = q, fail_fast);
    }

    TEST(TestNotNullRawPointerComparison)
    {
        int x = 1;
        const int y = 2;
        auto p1 = &x;
        auto p2 = &y;

        using NotNull1 = not_null<decltype(p1)>;
        using NotNull2 = not_null<decltype(p2)>;

        CHECK((NotNull1(p1) == NotNull1(p1)) == true);
        CHECK((NotNull1(p1) == NotNull2(p2)) == false);

        CHECK((NotNull1(p1) != NotNull1(p1)) == false);
        CHECK((NotNull1(p1) != NotNull2(p2)) == true);

        CHECK((NotNull1(p1) < NotNull1(p1)) == false);
        CHECK((NotNull1(p1) < NotNull2(p2)) == (p1 < p2));
        CHECK((NotNull2(p2) < NotNull1(p1)) == (p2 < p1));

        CHECK((NotNull1(p1) > NotNull1(p1)) == false);
        CHECK((NotNull1(p1) > NotNull2(p2)) == (p1 > p2));
        CHECK((NotNull2(p2) > NotNull1(p1)) == (p2 > p1));

        CHECK((NotNull1(p1) <= NotNull1(p1)) == true);
        CHECK((NotNull1(p1) <= NotNull2(p2)) == (p1 <= p2));
        CHECK((NotNull2(p2) <= NotNull1(p1)) == (p2 <= p1));

        CHECK((NotNull1(p1) >= NotNull1(p1)) == true);
        CHECK((NotNull1(p1) >= NotNull2(p2)) == (p1 >= p2));
        CHECK((NotNull2(p2) >= NotNull1(p1)) == (p2 >= p1));
    }

    TEST(TestNotNullSharedPtrComparison)
    {
        auto sp1 = std::make_shared<int>(1);
        auto sp2 = std::make_shared<const int>(3);

        using NotNullSp1 = not_null<decltype(sp1)>;
        using NotNullSp2 = not_null<decltype(sp2)>;

        CHECK((NotNullSp1(sp1) == NotNullSp1(sp1)) == true);
        CHECK((NotNullSp1(sp1) == NotNullSp2(sp2)) == false);

        CHECK((NotNullSp1(sp1) != NotNullSp1(sp1)) == false);
        CHECK((NotNullSp1(sp1) != NotNullSp2(sp2)) == true);

        CHECK((NotNullSp1(sp1) < NotNullSp1(sp1)) == false);
        CHECK((NotNullSp1(sp1) < NotNullSp2(sp2)) == (sp1 < sp2));
        CHECK((NotNullSp2(sp2) < NotNullSp1(sp1)) == (sp2 < sp1));

        CHECK((NotNullSp1(sp1) > NotNullSp1(sp1)) == false);
        CHECK((NotNullSp1(sp1) > NotNullSp2(sp2)) == (sp1 > sp2));
        CHECK((NotNullSp2(sp2) > NotNullSp1(sp1)) == (sp2 > sp1));

        CHECK((NotNullSp1(sp1) <= NotNullSp1(sp1)) == true);
        CHECK((NotNullSp1(sp1) <= NotNullSp2(sp2)) == (sp1 <= sp2));
        CHECK((NotNullSp2(sp2) <= NotNullSp1(sp1)) == (sp2 <= sp1));

        CHECK((NotNullSp1(sp1) >= NotNullSp1(sp1)) == true);
        CHECK((NotNullSp1(sp1) >= NotNullSp2(sp2)) == (sp1 >= sp2));
        CHECK((NotNullSp2(sp2) >= NotNullSp1(sp1)) == (sp2 >= sp1));
    }
}

int main(int, const char *[])
{
    return UnitTest::RunAllTests();
}
