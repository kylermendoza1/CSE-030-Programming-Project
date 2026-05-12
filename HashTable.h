#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <ArrayList.h>
#include <type_traits>
#include <cmath>

template <class T> 
class HashTable;

template <class T>
std::ostream &operator<<(std::ostream &os, const HashTable<T> &ht);

template <class T> 
class HashTable {
    ArrayList<ArrayList<T>> table;
    int capacity;
    int size;

    int extractInt(int x) {
        return x;
    }

    int extractInt(float x) {
        int result = 0;

        unsigned char *c = (unsigned char*) &x;

        for (int i = 0; i < 4; i++) {
            result += (int) *c * pow(256, i);
            c++;
        }

        return result;
    }

    int extractInt(std::string x) {
        int result = 0;

        for (int i = 0; i < std::min((int)x.length(), 4); i++) {
            result += x[i] * pow(256, i);
        }

        return result;
    }

    int f(T x) {
        int temp = extractInt(x);

        int key = temp % capacity;

        if (key < 0) {
            key += capacity;
        }

        return key;
    }

    void inflate(){
        if (size / capacity >= 3) {
            // step 1: copy items into temp storage
            ArrayList<T> temp;

            for (int i = 0; i < table.size(); i++) {
                for (int j = 0; j < table[i].size(); j++) {
                    temp.append(table[i][j]);
                }
            }

            // step 2: remove items from hash table
            for (int i = 0; i < table.size(); i++) {
                while (table[i].size() > 0) {
                    table[i].removeLast();
                }
            }

            // step 3: double capacity and reset size
            capacity *= 2;
            size = 0;

            // step 4: inflate table to match capacity
            while (table.size() < capacity) {
                table.append(ArrayList<T>());
            }

            // step 5: insert items back into hash table
            for (int i = 0; i < temp.size(); i++) {
                insert(temp[i]);
            }
        }
    }

    static_assert(std::is_same<T, int>::value ||
                      std::is_same<T, float>::value ||
                      std::is_same<T, std::string>::value,
                  "Template arugments should only be int, float, or string");

public:
    HashTable(int k = 10) {
        capacity = k;
        size = 0;

        for (int i = 0; i < k; i++) {
            table.append(ArrayList<T>());
        }
    }

    void insert(T value){
        int index = f(value);
        table[index].append(value);
        size++;

        inflate();
    }

    bool search(T value) {
        int index = f(value);
        return table[index].search(value);
    }

    friend std::ostream &operator<< <>(std::ostream &os,
                                       const HashTable<T> &ht);

    friend std::string minVal(HashTable<std::string> ht);
};

// Explicit instantiation of allowed templates
template class HashTable<int>;
template class HashTable<float>;
template class HashTable<std::string>;

template <class T>
inline std::ostream &operator<<(std::ostream &os, const HashTable<T> &ht) {
    for (int i = 0; i < ht.capacity; i++) {
        os << i << ": " << ht.table[i] << "\n";
    }

    return os;
}

#endif