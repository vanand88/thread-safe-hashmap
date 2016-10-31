#ifndef HASHMAP_H
#define HASHMAP_H

#include <shared_mutex>
#include <iostream>

// thread-safe hashmap class
// template arguments - key type and value type
// uses fixed-size C-style array to store elements
// collisions are handled using chaining
// hash function is implemented as modulus of the key with respect to array size
// supports insert, remove, operator[], clear, print, assignment, move
// supports resize operation to change array size - complexity O(N), where N is the number of elements in the hashmap
// thread-safety is gained using shared_mutex, which is locked using shared_lock during read operations, and locked using unique_lock during write operations,
// so that multiple readers can read at the same time, but only one writer writes at a time
template <class KeyType, class ValueType>
class CHashMap
{
public:
    CHashMap(int tableSize);
    ValueType& insert(const KeyType& key, const ValueType& value);
    ValueType& operator[](const KeyType& key);
    void remove(const KeyType& key);
    // assignment operator
    CHashMap& operator=(const CHashMap& other);
    // move assignment operator
    CHashMap& operator=(CHashMap&& other);

    void resize(int newSize);
    void clear();

    void print() const;

private:
    int hash(const KeyType& value) const;


    // node structure for implementing hashmap chaining
    // singly-linked list
    // contains: key, value, a link to the next node in list
    template <class KeyType, class ValueType>
    struct ListNode
    {
        ListNode(const KeyType& k, const ValueType& v)
            : key(k), value(v), pNext(0) {}
        KeyType key;
        ValueType value;
        ListNode* pNext;
    };

private:
    // fixed size table
    int m_TableSize;
    ListNode<KeyType, ValueType>** m_Table;
    // shared mutex
    mutable std::shared_mutex m_Mutex;
};

// implementation of CHashMap class
// put in the same file because of template-separate-compilation problem

// constructor, creates empty hashmap with given array size
template <class KeyType, class ValueType>
CHashMap<KeyType, ValueType>::CHashMap(int tableSize)
    : m_TableSize(tableSize)
{
    m_Table = new ListNode<KeyType, ValueType>*[m_TableSize] {0};
}

// copy assignment, duplicates the entire map
template <class KeyType, class ValueType>
CHashMap<KeyType, ValueType>& CHashMap<KeyType, ValueType>::operator=(const CHashMap& other)
{
    clear();

    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    m_TableSize = other.m_TableSize;
    m_Table = new ListNode<KeyType, ValueType>*[m_TableSize] {0};

    // copy all nodes, deep-copy
    for (int i = 0; i < m_TableSize; ++i)
    {
        ListNode<KeyType, ValueType>* node = other.m_Table[i];
        ListNode<KeyType, ValueType>* previousNode = 0;
        while (node)
        {
            ListNode<KeyType, ValueType>* newNode = new ListNode<KeyType, ValueType>(node->key, node->value);
            if (!previousNode)
                m_Table[i] = newNode;
            else
                previousNode.pNext = newNode
            previousNode = newNode;
            node = node->pNext;
        }
    }
    return *this;
}

// move assignment operator, used in resize operation
template <class KeyType, class ValueType>
CHashMap<KeyType, ValueType>& CHashMap<KeyType, ValueType>::operator=(CHashMap&& other)
{
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    m_TableSize = std::move(other.m_TableSize);
    m_Table = other.m_Table; // shallow-copy
    other.m_Table = 0;
    return *this;
}

// hash function - modulus of key with respect to table size
template <class KeyType, class ValueType>
int CHashMap<KeyType, ValueType>::hash(const KeyType& value) const
{
    return abs(value % m_TableSize); // abs to ensure non-negative result
}

// inserts (key, value) pair into hashmap
template <class KeyType, class ValueType>
ValueType& CHashMap<KeyType, ValueType>::insert(const KeyType& key, const ValueType& value)
{
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    int index = hash(key);
    ListNode<KeyType, ValueType>* node = m_Table[index];
    if (!node) // first element at index, insert new node here
    {
        ListNode<KeyType, ValueType>* newNode = new ListNode<KeyType, ValueType>(key, value);
        m_Table[index] = newNode;
        return newNode->value;
    }
    // chaining
    while (1)
    {
        if (node->key == key) // the key already is in the map, change the value
            return node->value = value;

        if (!node->pNext)
            break;
        node = node->pNext;
    }
    // insert a new node at the end of chain
    ListNode<KeyType, ValueType>* newNode = new ListNode<KeyType, ValueType>(key, value);
    node->pNext = newNode;
    return newNode->value;
}

// returns a reference to the value of the element with specified key
// if key is not found, insert a new element at key with default value of type ValueType
template <class KeyType, class ValueType>
ValueType& CHashMap<KeyType, ValueType>::operator[](const KeyType& key)
{
    std::shared_lock<std::shared_mutex> lock(m_Mutex);
    int index = hash(key);
    ListNode<KeyType, ValueType>* node = m_Table[index];
    // search in the list
    while (node)
    {
        if (node->key == key)
            return node->value;
        node = node->pNext;
    }
    // not found, create a new element and insert into hashmap
    ValueType val;
    lock.unlock(); // release the lock so that insert() can acquire it
    return insert(key, val);
}

// removes the element with specified key
template <class KeyType, class ValueType>
void CHashMap<KeyType, ValueType>::remove(const KeyType& key)
{
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    int index = hash(key);
    ListNode<KeyType, ValueType>* node = m_Table[index];
    ListNode<KeyType, ValueType>* prev = 0;
    // search in the list
    while (node)
    {
        if (node->key == key)
        {
            if (prev == 0)
                m_Table[index] = node->pNext;
            else
                prev->pNext = node->pNext;
            delete node;
            return;
        }
        prev = node;
        node = node->pNext;
    }
    return;
}

// prints all the elements to standard output
template <class KeyType, class ValueType>
void CHashMap<KeyType, ValueType>::print() const
{
    std::shared_lock<std::shared_mutex> lock(m_Mutex);
    for (int i = 0; i < m_TableSize; ++i)
    {
        ListNode<KeyType, ValueType>* node = m_Table[i];
        while (node)
        {
            std::cout << '(' << node->key << ',' << node->value << ")->";
            node = node->pNext;
        }
        std::cout << "0\n";
    }
}

// resize - changes the size of underlying array
// all the elements are rearranged based on new size (hash function values for existing keys are (possibly) changed)
// a new local hashmap with new size is created, filled, and moved out into the current map using move assignment operation
template <class KeyType, class ValueType>
void CHashMap<KeyType, ValueType>::resize(int newSize)
{
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    CHashMap<KeyType, ValueType> newMap(newSize);
    for (int i = 0; i < m_TableSize; ++i)
    {
        ListNode<KeyType, ValueType>* node = m_Table[i];
        while (node)
        {
            newMap.insert(node->key, node->value);
            node = node->pNext;
        }
    }
    lock.unlock(); // release the lock so that clear() can acquire it
    clear();
    *this = std::move(newMap);
}

// clears the hashmap, frees allocated momory
template <class KeyType, class ValueType>
void CHashMap<KeyType, ValueType>::clear()
{
    std::unique_lock<std::shared_mutex> lock(m_Mutex);
    for (int i = 0; i < m_TableSize; ++i)
    {
        ListNode<KeyType, ValueType>* node = m_Table[i];
        while (node)
        {
            ListNode<KeyType, ValueType>* temp = node->pNext;
            delete node;
            node = temp;
        }
        m_Table[i] = 0;
    }
    delete [] m_Table;
    m_Table = 0;
}

#endif // HASHMAP_H
