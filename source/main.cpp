#include <cstddef>
class Human {
    private:
    int age;
    public:
    Human(int a) : age(a){}
};

class Engineer : private Human{
    private:
    int years_of_work;
    public:
    Engineer(int a, int years) : age(a), years_of_work(years){}
};
