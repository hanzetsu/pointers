#include <cstddef>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <memory>
#include <cstdlib>
#include <new>

#include "UniquePtr.hpp"
#include "SharedPtr.hpp"

// ============================================================
// Подсчёт памяти: переопределяем operator new / delete
// ============================================================
static size_t g_bytes{0};
static bool g_track = false;

void *operator new(size_t size)
{
    if (g_track)
    {
        g_bytes += size;
    }
    void *p = std::malloc(size);
    if (!p)
        throw std::bad_alloc();
    return p;
}
void *operator new[](size_t size)
{
    if (g_track)
    {
        g_bytes += size;
    }
    void *p = std::malloc(size);
    if (!p)
        throw std::bad_alloc();
    return p;
}
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }

struct Payload
{
    int a = 1;
    double b = 2.0;
    char c[32] = {};
};

volatile void *g_sink = nullptr;

template <typename Func>
double time_ms(Func &&f, size_t iterations)
{
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i)
        f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

struct Result
{
    double ms;
    size_t bytes;
};

template <typename Func>
Result run_bench(Func &&f, size_t iterations)
{
    g_bytes = 0;
    g_track = true;
    double ms = time_ms(std::forward<Func>(f), iterations);
    g_track = false;
    return {ms, g_bytes};
}

void print_row(const char *name, const Result &r)
{
    std::cout << std::left << std::setw(28) << name
              << std::right << std::setw(12) << std::fixed << std::setprecision(2) << r.ms
              << std::setw(14) << r.bytes << "\n";
}

void print_header()
{
    std::cout << std::left << std::setw(28) << "Вариант"
              << std::right << std::setw(28) << "Время, мс"
              << std::setw(15) << "Байт" << "\n";
    std::cout << std::string(54, '-') << "\n";
}

// создание и удаление одного объекта
void bench_single(size_t N)
{
    std::cout << "\n[1] Создание/удаление одного объекта, N = " << N << "\n";
    print_header();

    print_row("Raw new/delete", run_bench([&]
                                          {
        Payload* p = new Payload;
        g_sink = p;
        delete p; }, N));

    print_row("UniquePtr", run_bench([&]
                                     {
        UniquePtr<Payload> p(new Payload);
        g_sink = p.get(); }, N));

    print_row("std::unique_ptr", run_bench([&]
                                           {
        auto p = std::make_unique<Payload>();
        g_sink = p.get(); }, N));

    print_row("SharedPtr", run_bench([&]
                                     {
        SharedPtr<Payload> p(new Payload);
        g_sink = p.get(); }, N));

    print_row("std::shared_ptr", run_bench([&]
                                           {
        auto p = std::make_shared<Payload>();
        g_sink = p.get(); }, N));
}

// массив из N объектов
void bench_array(size_t N)
{
    std::cout << "\n[2] Массив из " << N << " объектов\n";
    print_header();

    print_row("UniquePtrArr", run_bench([&]
                                        {
        UniquePtrArr<Payload> arr(new Payload[N]);
        g_sink = arr.get(); }, 1));

    print_row("SharedPtrArr", run_bench([&]
                                        {
        SharedPtrArr<Payload> arr(new Payload[N], N);
        g_sink = arr.get(); }, 1));

    print_row("std::unique_ptr<T[]>", run_bench([&]
                                                {
        auto arr = std::make_unique<Payload[]>(N);
        g_sink = arr.get(); }, 1));
    print_row("Raw new[]/delete[]", run_bench([&]
                                              {
        Payload* arr = new Payload[N];
        g_sink = arr;
        delete[] arr; }, 1));
}

int main()
{
    std::cout << "Бенчмарк умных указателей\n";
    std::cout << std::fixed << std::setprecision(2);

    for (size_t N : {1000UL, 10000UL, 100000UL, 1000000UL})
    {
        bench_single(N);
    }

    for (size_t N : {10000UL, 100000UL, 1000000UL})
    {
        bench_array(N);
    }

    return 0;
}