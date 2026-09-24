    Бенчмарк умных указателей

[1] Создание/удаление одного объекта, N = 1000
Вариант              Время, мс      Байт
------------------------------------------------------
Raw new/delete                      0.01         48000
UniquePtr                           0.01         48000
std::unique_ptr                     0.05         48000
SharedPtr                           0.02         72000
std::shared_ptr                     0.05         64000

[1] Создание/удаление одного объекта, N = 10000
Вариант              Время, мс      Байт
------------------------------------------------------
Raw new/delete                      0.08        480000
UniquePtr                           0.11        480000
std::unique_ptr                     0.46        480000
SharedPtr                           0.19        720000
std::shared_ptr                     0.49        640000

[1] Создание/удаление одного объекта, N = 100000
Вариант              Время, мс      Байт
------------------------------------------------------
Raw new/delete                      0.75       4800000
UniquePtr                           1.07       4800000
std::unique_ptr                     4.73       4800000
SharedPtr                           1.96       7200000
std::shared_ptr                     5.10       6400000

[1] Создание/удаление одного объекта, N = 1000000
Вариант              Время, мс      Байт
------------------------------------------------------
Raw new/delete                      7.52      48000000
UniquePtr                          11.03      48000000
std::unique_ptr                    54.10      48000000
SharedPtr                          24.20      72000000
std::shared_ptr                    60.11      64000000

[2] Массив из 10000 объектов
Вариант              Время, мс      Байт
------------------------------------------------------
Raw new[]/delete[]                  0.04        480000
UniquePtrArr                        0.04        480000
SharedPtrArr                        0.04        480008
std::unique_ptr<T[]>                0.05        480000

[2] Массив из 100000 объектов
Вариант              Время, мс      Байт
------------------------------------------------------
Raw new[]/delete[]                  0.46       4800000
UniquePtrArr                        0.46       4800000
SharedPtrArr                        0.46       4800008
std::unique_ptr<T[]>                0.41       4800000

[2] Массив из 1000000 объектов
Вариант              Время, мс      Байт
------------------------------------------------------
Raw new[]/delete[]                 13.69      48000000
UniquePtrArr                       13.59      48000000
SharedPtrArr                       13.69      48000008
std::unique_ptr<T[]>               14.05      48000000

[3] Операции с weak-указателями, N = 10000
Вариант              Время, мс      Байт
------------------------------------------------------
WeakPtr ctor                        0.29        720000
std::weak_ptr ctor                  0.75        640000
WeakPtr lock                        0.51        720000
std::weak_ptr lock                  0.98        640000
WeakPtr expired                     0.30        720000
std::weak_ptr expired               0.81        640000

[3] Операции с weak-указателями, N = 100000
Вариант              Время, мс      Байт
------------------------------------------------------
WeakPtr ctor                        3.02       7200000
std::weak_ptr ctor                  7.60       6400000
WeakPtr lock                        4.33       7200000
std::weak_ptr lock                  9.65       6400000
WeakPtr expired                     3.42       7200000
std::weak_ptr expired               7.96       6400000

[3] Операции с weak-указателями, N = 1000000
Вариант              Время, мс      Байт
------------------------------------------------------
WeakPtr ctor                       26.87      72000000
std::weak_ptr ctor                 65.49      64000000
WeakPtr lock                       36.22      72000000
std::weak_ptr lock                 95.89      64000000
WeakPtr expired                    28.16      72000000
std::weak_ptr expired              70.10      64000000

Тесты с санитайзером 
[Пройдено] SharedPtr basic
[Пройдено] SharedPtr copy/assign
[Пройдено] SharedPtr move
[Пройдено] SharedPtr reset
[Пройдено] SharedPtrArr
[Пройдено] SharedPtr subtyping

Все тесты SharedPtr пройдены успешно.

[Пройдено] UniquePtr basic
[Пройдено] UniquePtr move
[Пройдено] UniquePtr release/reset
[Пройдено] UniquePtrArr

Все тесты UniquePtr пройдены успешно.

[Пройдено] WeakPtr basic
[Пройдено] WeakPtr expired
[Пройдено] WeakPtr copy
[Пройдено] WeakPtr subtyping

Все тесты WeakPtr пройдены успешно.

