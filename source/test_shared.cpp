#include "TestCommon.hpp"

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

int main()
{
    size_t failed = 0;
    auto run = [&failed](bool (*test)(), const char *name)
    {
        if (test())
            std::cout << "[Пройдено] " << name << "\n";
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
    run(test_shared_array, "SharedPtrArr");

    if (failed == 0)
    {
        std::cout << "\nВсе тесты SharedPtr пройдены успешно.\n";
        return 0;
    }
    std::cout << "\nПровалено тестов: " << failed << "\n";
    return 1;
}

// g++ -std=c++17 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude source/test_shared.cpp -o build/test_shared_san
