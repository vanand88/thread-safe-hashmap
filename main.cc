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
    while (1)
    {
        map->insert(i, 'L');
        std::this_thread::sleep_for(std::chrono::seconds(1));
        i += 10;
    }
}

void f2(CHashMap<int, char>* map)
{
    int i = 3;
    while (1)
    {
        std::cout << "i = " << (*map)[i] << std::endl;
        map->remove(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        i += 10;
    }
}

int main()
{
    CHashMap<int, char> map(10);
    //std::thread t([&map]() -> void { map->insert(3, 'k'); });
    std::thread t(f, &map);
    //std::thread t2(f1, &map);
    //std::thread t1(f2, &map);

    t.join();
    //t1.join();
    //t2.join();

    map.print();
    return 0;
}
