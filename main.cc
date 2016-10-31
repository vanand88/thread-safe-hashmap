// hashmap usage

#include "hashmap.h"
#include <thread>
#include <chrono>

void f(CHashMap<int, char>* map)
{
    map->insert(3, 'k');
    map->insert(13, 't');
    map->insert(23, 'h');
    map->insert(33, 'k');
    map->insert(43, 'r');
    map->insert(53, 'y');
    map->insert(63, 'k');
    map->insert(4, 'q');
    map->insert(25, 'd');
    map->insert(77, 'd');
    map->resize(6);
}

void f1(CHashMap<int, char>* map)
{
    int i = 3;
    while (i < 40)
    {
        map->insert(i, 'L');
        i += 3;
    }
}

void f2(CHashMap<int, char>* map)
{
    int i = 3;
    while (i < 40)
    {
        map->remove(i);
        i += 2;
    }
}

int main()
{
    CHashMap<int, char> map(10);
    CHashMap<int, char> map1(16);
    CHashMap<int, char> map2(14);

    //std::thread t4([&map]() -> void { std::cout << map.at(45); });
    std::thread t(f, &map2);
    std::thread t1(f1, &map);
    std::thread t2(f2, &map2);

    t.join();
    t1.join();
    t2.join();
    //t4.join();

    map.print();
    map1.print();
    map2.print();
    map1 = map2;
    map2 = std::move(map);
    map.print();
    map1.print();
    map2.print();

    return 0;
}
