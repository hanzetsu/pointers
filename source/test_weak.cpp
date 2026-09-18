#include "TestCommon.hpp"

bool test_weak_basic()
{
    Tracked::alive = 0;
    {
        SharedPtr<Tracked> sp(new Tracked(42));
        WeakPtr<Tracked> wp = sp;

        CHECK(Tracked::alive == 1, "WeakPtr: объект создан");
        CHECK(sp.getCountRef() == 1, "WeakPtr: shared_count = 1");
        CHECK(wp.getCountWeak() == 1, "WeakPtr: weak_count = 1");
        CHECK(!wp.expired(), "WeakPtr: не expired");

        SharedPtr<Tracked> locked = wp.lock();
        CHECK(locked.get() == sp.get(), "WeakPtr lock: тот же объект");
        CHECK(sp.getCountRef() == 2, "WeakPtr lock: shared_count = 2");
    }
    CHECK(Tracked::alive == 0, "WeakPtr: объект удалён");
    return true;
}

bool test_weak_expired()
{
    Tracked::alive = 0;
    WeakPtr<Tracked> wp;
    {
        SharedPtr<Tracked> sp(new Tracked(7));
        wp = sp;
        CHECK(!wp.expired(), "WeakPtr expired: не expired пока sp жив");
        CHECK(wp.lock().get() != nullptr, "WeakPtr expired: lock успешен");
    }
    CHECK(Tracked::alive == 0, "WeakPtr expired: объект удалён");
    CHECK(wp.expired(), "WeakPtr expired: expired после смерти sp");
    CHECK(wp.lock().get() == nullptr, "WeakPtr expired: lock вернул пустой");
    return true;
}

bool test_weak_copy()
{
    Tracked::alive = 0;
    {
        SharedPtr<Tracked> sp(new Tracked(1));
        WeakPtr<Tracked> wp1 = sp;
        WeakPtr<Tracked> wp2 = wp1;
        WeakPtr<Tracked> wp3;
        wp3 = wp2;

        CHECK(wp1.getCountWeak() == 3, "WeakPtr copy: weak_count = 3");
        CHECK(sp.getCountRef() == 1, "WeakPtr copy: shared_count = 1");

        SharedPtr<Tracked> locked = wp3.lock();
        CHECK(sp.getCountRef() == 2, "WeakPtr copy lock: shared_count = 2");
    }
    CHECK(Tracked::alive == 0, "WeakPtr copy: объект удалён");
    return true;
}
bool test_weak_subtyping()
{
    Base::alive = 0;
    {
        SharedPtr<Derived> d(new Derived);
        WeakPtr<Base> wb = d;
        CHECK(d.getCountRef() == 1, "WeakPtr subtyping: shared_count = 1");
        CHECK(wb.getCountWeak() == 1, "WeakPtr subtyping: weak_count = 1");
        CHECK(!wb.expired(), "WeakPtr subtyping: не expired");

        SharedPtr<Base> locked = wb.lock();
        CHECK(locked.get() == d.get(), "WeakPtr subtyping lock: тот же объект");
        CHECK(locked->base_value == 1, "WeakPtr subtyping lock: base_value");
        CHECK(d.getCountRef() == 2, "WeakPtr subtyping lock: shared_count = 2");
    }
    CHECK(Base::alive == 0, "WeakPtr subtyping: объект удалён");

    Base::alive = 0;
    {
        SharedPtr<Derived> d(new Derived);
        WeakPtr<Derived> wd = d;
        WeakPtr<Base> wb;
        wb = wd;
        CHECK(wd.getCountWeak() == 2, "WeakPtr subtyping assign: weak_count = 2");
        CHECK(wb.expired() == false, "WeakPtr subtyping assign: не expired");

        d.reset();
        CHECK(wd.expired(), "WeakPtr subtyping assign: expired после reset");
        CHECK(wb.expired(), "WeakPtr subtyping assign: expired после reset");
    }

    return true;
}
int main()
{
    size_t failed = 0;
    auto run = [&failed](bool (*test)(), const char *name)
    {
        try
        {
            test();
            std::cout << "[Пройдено] " << name << "\n";
        }
        catch (const TestFailure &e)
        {
            std::cerr << e.msg << "\n";
            std::cout << "[Провалено] " << name << "\n";
            ++failed;
        }
    };

    run(test_weak_basic, "WeakPtr basic");
    run(test_weak_expired, "WeakPtr expired");
    run(test_weak_copy, "WeakPtr copy");
    run(test_weak_subtyping, "WeakPtr subtyping");
    if (failed == 0)
    {
        std::cout << "\nВсе тесты WeakPtr пройдены успешно.\n";
        return 0;
    }
    std::cout << "\nПровалено тестов: " << failed << "\n";
    return 1;
}

// g++ -std=c++17 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude source/test_weak.cpp   -o build/test_weak_san