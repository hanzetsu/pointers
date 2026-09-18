#include "TestCommon.hpp"

bool test_unique_basic()
{
    Tracked::alive = 0;
    {
        UniquePtr<Tracked> p(new Tracked(99));
        CHECK(Tracked::alive == 1, "UniquePtr: объект создан");
        CHECK(p->value == 99, "UniquePtr: доступ через ->");
        CHECK((*p).value == 99, "UniquePtr: доступ через *");
    }
    CHECK(Tracked::alive == 0, "UniquePtr: объект удалён");
    return true;
}

bool test_unique_move()
{
    Tracked::alive = 0;
    {
        UniquePtr<Tracked> a(new Tracked(5));
        UniquePtr<Tracked> b = std::move(a);
        CHECK(a.get() == nullptr, "UniquePtr move: источник пуст");
        CHECK(b->value == 5, "UniquePtr move: объект перемещён");

        UniquePtr<Tracked> c;
        c = std::move(b);
        CHECK(b.get() == nullptr, "UniquePtr move assign: источник пуст");
        CHECK(c->value == 5, "UniquePtr move assign: объект перемещён");
    }
    CHECK(Tracked::alive == 0, "UniquePtr move: объект удалён");
    return true;
}

bool test_unique_release_reset()
{
    Tracked::alive = 0;
    {
        UniquePtr<Tracked> p(new Tracked(7));
        Tracked *raw = p.release();
        CHECK(p.get() == nullptr, "UniquePtr release: указатель пуст");
        CHECK(Tracked::alive == 1, "UniquePtr release: объект жив");
        delete raw;
        CHECK(Tracked::alive == 0, "UniquePtr release: объект удалён вручную");

        UniquePtr<Tracked> q(new Tracked(8));
        q.reset(new Tracked(9));
        CHECK(Tracked::alive == 1, "UniquePtr reset: старый удалён, новый создан");
        CHECK(q->value == 9, "UniquePtr reset: значение обновлено");

        q.reset();
        CHECK(q.get() == nullptr, "UniquePtr reset(nullptr): указатель пуст");
        CHECK(Tracked::alive == 0, "UniquePtr reset(nullptr): объект удалён");
    }
    return true;
}

bool test_unique_array()
{
    Tracked::alive = 0;
    {
        UniquePtrArr<Tracked> arr(new Tracked[2]{{10}, {20}});
        CHECK(Tracked::alive == 2, "UniquePtrArr: создано 2 объекта");
        CHECK(arr[0].value == 10, "UniquePtrArr: arr[0] == 10");
        CHECK(arr[1].value == 20, "UniquePtrArr: arr[1] == 20");

        UniquePtrArr<Tracked> arr2 = std::move(arr);
        CHECK(arr.get() == nullptr, "UniquePtrArr move: источник пуст");
        CHECK(arr2[0].value == 10, "UniquePtrArr move: объекты перемещены");

        arr2.reset(new Tracked[1]{{30}});
        CHECK(Tracked::alive == 1, "UniquePtrArr reset: старые удалены, новый создан");
        CHECK(arr2[0].value == 30, "UniquePtrArr reset: значение обновлено");
    }
    CHECK(Tracked::alive == 0, "UniquePtrArr: все объекты удалены");
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

    run(test_unique_basic, "UniquePtr basic");
    run(test_unique_move, "UniquePtr move");
    run(test_unique_release_reset, "UniquePtr release/reset");
    run(test_unique_array, "UniquePtrArr");

    if (failed == 0)
    {
        std::cout << "\nВсе тесты UniquePtr пройдены успешно.\n";
        return 0;
    }
    std::cout << "\nПровалено тестов: " << failed << "\n";
    return 1;
}

// g++ -std=c++17 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude source/test_unique.cpp -o build/test_unique_san