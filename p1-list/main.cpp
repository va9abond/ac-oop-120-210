#include <iostream>
#include <list>
#include <cstdint>  // uint8_t
#include <cstring>
#include <iterator> // std::advance


#define BUFSIZE 4


template <typename T>
using pred_t = int (*)(T);

// Print list using iterators
template <typename T>
void print (const std::list<T> &list, char delim = ' ', char end = '\n');

template <typename T>
void push_sorted (std::list<T> &list, T);

template <typename T>
typename std::list<T>::const_iterator pop_sorted (std::list<T> &list, const T &item);

template <typename T>
void pop_front_sorted (std::list<T> &list);

template <typename T>
void pop_back_sorted (std::list<T> &list);

template <typename T>
std::list<T> filter (const std::list<T> &list, pred_t<T>);

int xislower (char);

class Program {
private:
    using u8 = uint8_t;

public:
    Program() :
        name(nullptr),
        dev(nullptr),
        codelang(nullptr),
        price(0.0),
        arm_compat(false),
        ios_compat(false),
        codeopen(false),
        ver(0)
    { };

    Program(char const *_name,
            char const *_dev,
            char const *_codelang,
            float _price,
            bool  _arm_compat,
            bool  _ios_compat,
            bool  _codeopen,
            u8    _ver) :
        name(_name),
        dev(_dev),
        codelang(_codelang),
        price(_price),
        arm_compat(_arm_compat),
        ios_compat(_ios_compat),
        codeopen(_codeopen),
        ver(_ver)
    { };

    bool operator == (const Program &rhs) {
        return strcmp(name, rhs.name) == 0 &&
            strcmp(dev, rhs.dev) == 0 &&
            strcmp(codelang, rhs.codelang) == 0 &&
            price == rhs.price &&
            arm_compat == rhs.arm_compat &&
            ios_compat == rhs.ios_compat &&
            codeopen == rhs.codeopen &&
            ver == rhs.ver;
    }

    bool operator != (const Program &rhs) {
        return !(*this == rhs);
    }

    bool operator < (const Program &rhs) const {
        if (price < rhs.price)
            return true;
        else if (price == rhs.price) {
            if (ver < rhs.ver)
                return true;
            else if (ver == rhs.ver)
                if (strcmp(name, rhs.name) < 0)
                    return true;
        }

        return false;
    }

    // friend void print (const Program &prog);

    friend std::ostream& operator << (std::ostream &, const Program &);


private:
    char  const *name;
    char  const *dev;
    char  const *codelang;
    float price;
    bool  arm_compat;
    bool  ios_compat;
    bool  codeopen;
    u8    ver;
};

std::ostream& operator << (std::ostream &os, const Program &prog) {
    os << "\"" << prog.name << "\"" << " by " << prog.dev
        << ", ver " << (unsigned int)prog.ver << '\n'
    << "  [language]" << " : " << prog.codelang << '\n'
    << " [open code]" << " : " << prog.codeopen << '\n'
    << "[arm_compat]" << " : " << prog.arm_compat << '\n'
    << "[ios_compat]" << " : " << prog.ios_compat << '\n'
    << "     [price]" << " : " << prog.price;

    return os;
}


int main (void) {

    std::list<char> chars;

    // Push items, list remains sorted
    for (int i = 0; i < 10; ++i)
        push_sorted(chars, (char)('z' - i));
    for (int i = 0; i < 10; ++i)
        push_sorted(chars, (char)('a' + i));
    for (int i = 0; i < 10; ++i)
        push_sorted(chars, (char)('z' - i));

    print(chars);


    // Pop items, list remains sorted
    (void) pop_sorted(chars, 'z');
    (void) pop_sorted(chars, 'z');

    pop_front_sorted(chars);
    pop_back_sorted(chars);

    print(chars);


    // Filter items
    if (auto it = chars.begin(); it != chars.end()) {
        // up some chars in list
        for (int i = 0; it != chars.end() && i < 10; ++i, ++it)
            *it = toupper(*it);

        print(chars);
        print(filter(chars, xislower));
    }



    Program prog_array[BUFSIZE] = {
        { "Acompiler", "Rustem", "asm", 1.0, true, true, true, 11 },
        { "Bstring library", "Rustem", "C", 2.0, true, false, true, 2 },
        { "Cvideo stream", "Rustem", "Go", 3.0, true, true, false, 4 },
        { "Dlibrary container", "Rustem", "C++20", 4.0, true, true, true, 9 },
    };

    std::list<Program> prog_list;
    for (int i = BUFSIZE-1; i >= 0 ; --i)
        push_sorted(prog_list, prog_array[i]);
    // ASSERT: list sorted here
    print(prog_list, '\n');

    std::cout << '\n';
    std::cout << '\n';

    auto it_item_C = prog_list.begin();
    std::advance(it_item_C, 2);

    // pop_sort return iterator on max priority item
    auto rit = pop_sorted(prog_list, prog_array[2]); // erase item C
    std::cout << *rit << '\n'; // max priority item is always at the begining of the list

    // ASSERT: list contains only 3 items: A, B, D
    print(prog_list, '\n');



    return 0;
}

template <typename T>
void print (const std::list<T> &list, char delim, char end) {
    for (auto it = list.begin(); it != list.end(); ++it)
        std::cout << *it << delim;
    putchar(end);

    return;
}

template <typename T>
void push_sorted (std::list<T> &list, T item) {
    auto where = list.begin();
    while (where != list.end() && *where < item)
        ++where;

    list.insert(where, item);

    return;
}

template <typename T>
typename std::list<T>::const_iterator pop_sorted (std::list<T> &list, const T &item) {
    auto where = list.begin();
    while (where != list.end() && *where != item)
        ++where;

    if (where != list.end())
        list.erase(where);

    return list.begin();
}

template <typename T>
void pop_front_sorted (std::list<T> &list) {
    return list.pop_front();
}

template <typename T>
void pop_back_sorted (std::list<T> &list) {
    return list.pop_back();
}

template <typename T>
std::list<T> filter (const std::list<T> &list, pred_t<T> pred) {
    std::list<T> rlist;

    for (auto it = list.begin(); it != list.end(); ++it)
        if (pred(*it))
            rlist.push_back(*it);

    return rlist;
}

int xislower (char ch) {
    return islower(ch);
}

