#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "Exceptions.hpp"

struct Tracked
{
    static int alive;
    int value;
    Tracked(int v = 0) : value(v) { ++alive; }
    ~Tracked() { --alive; }
};
int Tracked::alive = 0;

#define CHECK(cond, msg)                                                                    \
    do                                                                                      \
    {                                                                                       \
        if (!(cond))                                                                        \
        {                                                                                   \
            std::cerr << "Провал: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            return false;                                                                   \
        }                                                                                   \
    } while (0)

bool test_shared_basic()
{
    Tracked::alive = 0;
    {
        SharedPtr<Tracked> p(new Tracked(42));
        CHECK(Tracked::alive == 1, "SharedPtr: объект создан");
        CHECK(p.getCountRef() == 1, "SharedPtr: счётчик ссылок = 1");
        CHECK(p->value == 42, "SharedPtr: доступ через ->");
        CHECK((*p).value == 42, "SharedPtr: доступ через *");
    }
    CHECK(Tracked::alive == 0, "SharedPtr: объект удалён после выхода из области");
    return true;
}
bool test_leak()
{
    SharedPtr<Tracked> p(new Tracked(1));
    new Tracked(2);
    return true;
}
bool test_shared_copy()
{
    Tracked::alive = 0;
    {
        SharedPtr<Tracked> a(new Tracked(1));
        SharedPtr<Tracked> b = a;
        CHECK(a.getCountRef() == 2, "SharedPtr copy: счётчик = 2");
        CHECK(b.getCountRef() == 2, "SharedPtr copy: счётчик = 2");
        CHECK(Tracked::alive == 1, "SharedPtr copy: объект один");

        SharedPtr<Tracked> c;
        c = a;
        CHECK(a.getCountRef() == 3, "SharedPtr assign: счётчик = 3");

        a.reset();
        CHECK(a.get() == nullptr, "SharedPtr reset: a пуст");
        CHECK(b.getCountRef() == 2, "SharedPtr reset: счётчик = 2");
        CHECK(Tracked::alive == 1, "SharedPtr reset: объект жив");
    }
    CHECK(Tracked::alive == 0, "SharedPtr copy: все объекты удалены");
    return true;
}

bool test_shared_move()
{
    Tracked::alive = 0;
    {
        SharedPtr<Tracked> a(new Tracked(10));
        SharedPtr<Tracked> b = std::move(a);
        CHECK(a.get() == nullptr, "SharedPtr move: источник пуст");
        CHECK(b.getCountRef() == 1, "SharedPtr move: счётчик = 1");
        CHECK(Tracked::alive == 1, "SharedPtr move: объект один");

        SharedPtr<Tracked> c;
        c = std::move(b);
        CHECK(b.get() == nullptr, "SharedPtr move assign: источник пуст");
        CHECK(c.getCountRef() == 1, "SharedPtr move assign: счётчик = 1");
    }
    CHECK(Tracked::alive == 0, "SharedPtr move: объект удалён");
    return true;
}

bool test_shared_reset()
{
    Tracked::alive = 0;
    {
        SharedPtr<Tracked> p(new Tracked(1));
        p.reset(new Tracked(2));
        CHECK(Tracked::alive == 1, "SharedPtr reset: старый удалён, новый создан");
        CHECK(p->value == 2, "SharedPtr reset: значение обновлено");
        CHECK(p.getCountRef() == 1, "SharedPtr reset: счётчик = 1");

        p.reset();
        CHECK(p.get() == nullptr, "SharedPtr reset(nullptr): указатель пуст");
        CHECK(Tracked::alive == 0, "SharedPtr reset(nullptr): объект удалён");
    }
    return true;
}

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

bool test_shared_array()
{
    Tracked::alive = 0;
    {
        SharedPtrArr<Tracked> arr(new Tracked[3]{{1}, {2}, {3}}, 3);
        CHECK(Tracked::alive == 3, "SharedPtrArr: создано 3 объекта");
        CHECK(arr.getSize() == 3, "SharedPtrArr: размер = 3");
        CHECK(arr[0].value == 1, "SharedPtrArr: arr[0] == 1");
        CHECK(arr[1].value == 2, "SharedPtrArr: arr[1] == 2");
        CHECK(arr[2].value == 3, "SharedPtrArr: arr[2] == 3");

        bool caught = false;
        try
        {
            arr[3];
        }
        catch (const IndexOutOfRange &)
        {
            caught = true;
        }
        CHECK(caught, "SharedPtrArr: исключение при выходе за границы");

        SharedPtrArr<Tracked> arr2 = arr;
        CHECK(arr.getCountRef() == 2, "SharedPtrArr copy: счётчик = 2");
        CHECK(Tracked::alive == 3, "SharedPtrArr copy: объекты не дублируются");

        arr.reset(new Tracked[2]{{4}, {5}}, 2);
        CHECK(arr.getSize() == 2, "SharedPtrArr reset: новый размер = 2");
        CHECK(arr[0].value == 4, "SharedPtrArr reset: arr[0] == 4");
        CHECK(Tracked::alive == 5, "SharedPtrArr reset: старые 3 сохранены(на них указывает arr2), новые 2 созданы");
    }
    CHECK(Tracked::alive == 0, "SharedPtrArr: все объекты удалены");
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
        if (test())
        {
            std::cout << "[Пройдено] " << name << "\n";
        }
        else
        {
            std::cout << "[Провалено] " << name << "\n";
            ++failed;
        }
    };

    run(test_shared_basic, "SharedPtr basic");
    run(test_shared_copy, "SharedPtr copy/assign");
    run(test_shared_move, "SharedPtr move");
    run(test_shared_reset, "SharedPtr reset");
    run(test_unique_basic, "UniquePtr basic");
    run(test_unique_move, "UniquePtr move");
    run(test_unique_release_reset, "UniquePtr release/reset");
    run(test_shared_array, "SharedPtrArr");
    run(test_unique_array, "UniquePtrArr");
    if (failed == 0)
    {
        std::cout << "\nВсе тесты пройдены успешно.\n";
        return 0;
    }
    else
    {
        std::cout << "\nПровалено тестов: " << failed << "\n";
        return 1;
    }
}
//g++ -std=c++17 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude source/tests.cpp -o build/tests_san
// valgrind --leak-check=full ./tests