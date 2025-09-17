#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <cstdint>  // uint8_t
#include <cstring>
#include <iterator> // std::advance


#define BUFSIZE 12
#define KEY_THRESHOLD 67


template <typename K, typename M>
using map_iterator = typename std::map<K,M>::iterator;

template <typename K, typename M>
using map_const_iterator = typename std::map<K,M>::const_iterator;

// Print map using iterators
template <typename K, typename M>
void print (const std::map<K,M> &map, char delim = ' ', char end = '\n');

template <typename M>
void print (const std::set<M> &set, char delim = ' ', char end = '\n');

template <typename K, typename M>
void print (const std::multimap<K,M> &mmap, char delim = ' ', char end = '\n');

template <typename K, typename M>
map_const_iterator<K,M> find_by_key (const K &key, const std::map<K,M> &map);

template <typename K, typename M>
map_const_iterator<K,M> find_by_val (const M &val, const std::map<K,M> &map);

template <typename K, typename M, typename Pred_t>
std::map<K,M> map_filter (Pred_t pred, const std::map<K,M> &map);

template <typename K, typename M>
std::map<K,M> map_filter_fast (const K &key, const std::map<K,M> &map);

bool key_cond (const char *key);

template <typename K, typename M>
std::set<M> get_unique_mapped (const std::map<K,M> &map);


template <typename K, typename M>
std::vector<M> get_all_by_key (const K &key, const std::multimap<K,M> &map);

template <typename K, typename M>
map_const_iterator<K,M> find_by_key (const K &key, const std::multimap<K,M> &map);

template <typename K, typename M>
map_const_iterator<K,M> find_by_val (const M &val, const std::multimap<K,M> &map);

template <typename K, typename M, typename Pred_t>
std::multimap<K,M> mmap_filter (Pred_t pred, const std::multimap<K,M> &map);


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

        bool operator == (const Program &rhs) const {
            return strcmp(name, rhs.name) == 0 &&
                strcmp(dev, rhs.dev) == 0 &&
                strcmp(codelang, rhs.codelang) == 0 &&
                price == rhs.price &&
                arm_compat == rhs.arm_compat &&
                ios_compat == rhs.ios_compat &&
                codeopen == rhs.codeopen &&
                ver == rhs.ver;
        }

        bool operator != (const Program &rhs) const {
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
    << "  [language]" << ": " << prog.codelang << '\n'
    << " [open code]" << ": " << prog.codeopen << '\n'
    << "[arm_compat]" << ": " << prog.arm_compat << '\n'
    << "[ios_compat]" << ": " << prog.ios_compat << '\n'
    << "     [price]" << ": " << prog.price;

    return os;
}


int main() {

    Program prog_array[BUFSIZE] = {
        { "[A]compiler", "Rustem", "asm", 1.0, true, true, true, 11 },
        { "[B]string library", "Rustem", "C", 2.0, true, false, true, 2 },
        { "[C]video stream", "Rustem", "Go", 3.0, true, true, false, 4 },
        { "[D]library container", "Rustem", "C++20", 4.0, true, true, true, 9 },
        { "[E]static analyzer", "Rustem", "C", 20.0, false, false, false, 4 },
        { "[F]mts", "Rustem", "C", 20.0, false, false, false, 4 },
        { "[G]mirea.pulse", "Rustem", "C", 20.0, false, false, false, 4 },
        { "[H]browser", "Rustem", "C", 20.0, false, false, false, 4 },
        { "[I]pdf reader", "Rustem", "C", 20.0, false, false, false, 4 },
        { "[J]encoder", "Rustem", "C", 20.0, false, false, false, 4 },
        { "[K]decoder", "Rustem", "C", 20.0, false, false, false, 4 },
        { "[L]Payment Hub", "Rustem", "C", 20.0, false, false, false, 4 },
    };

    char const *keyA = "A", *keyB = "B", *keyC = "C", *keyD = "D", *keyE = "E";
         // *keyF = "F", *keyG = "G", *keyH = "H", *keyI = "I", *keyJ = "J",
         // *keyK = "K", *keyL = "L";

#if HIDE == 0

    std::map<char const*, Program> prog_map = {
        { keyA, prog_array[0] },
        { keyB, prog_array[1] },
        { keyC, prog_array[2] },
        { keyD, prog_array[3] },
        { keyE, prog_array[4] },
        // { keyF, prog_array[5] },
        // { keyG, prog_array[6] },
        // { keyH, prog_array[7] },
        // { keyI, prog_array[8] },
        // { keyJ, prog_array[9] },
        // { keyK, prog_array[10] },
        // { keyL, prog_array[11] },
    };
    std::cout << "-- PRINT MAP USING ITERATORS --\n";
    print(prog_map, '\n');


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- FIND BY KEY " << keyA << " --\n";
    auto it = find_by_key(keyA, prog_map);
    std::cout << it->second << '\n';


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- FIND BY VAL --\n";
    it = find_by_val(prog_array[2], prog_map);
    std::cout << it->second << '\n';


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- FILTER --\n";
    std::map<char const*, Program> map_filtered = map_filter(key_cond, prog_map);
    std::cout << "-- PRINT FILTERED MAP --\n";
    print(map_filtered, '\n');


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- FAST FILTER --\n";
    std::map<char const*, Program> map_filtered_fast = map_filter_fast(keyD, prog_map);
    std::cout << "-- PRINT FILTERED MAP --\n";
    print(map_filtered_fast, '\n');


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- SET --\n";
    std::set<Program> prog_set = get_unique_mapped(prog_map);
    print(prog_set, '\n');

#endif // HIDE

    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- MULTIMAP --\n";
    std::multimap<char const*, Program> prog_mmap = {
        { keyA, prog_array[0] },
        { keyB, prog_array[1] },
        { keyB, prog_array[2] },
        { keyB, prog_array[3] },
        { keyC, prog_array[2] },
        { keyD, prog_array[3] },
        { keyE, prog_array[4] },
    };
    print(prog_mmap, '\n');


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- GET ALL VALUES BY KEY --\n";
    std::vector<Program> keyB_values = get_all_by_key(keyB, prog_mmap);
    for (const auto &el : keyB_values)
        std::cout << el << '\n';
    putchar('\n');


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- FIND BY KEY " << keyA << " --\n";
    auto it_ = find_by_key(keyA, prog_mmap);
    std::cout << it_->second << '\n';


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- FIND BY VAL --\n";
    it_ = find_by_val(prog_array[2], prog_mmap);
    std::cout << it_->second << '\n';


    for (int l = 0; l < 2; ++l)
        std::cout << '\n' << '\n';
    std::cout << "-- FILTER --\n";
    std::multimap<char const*, Program> mmap_filtered = mmap_filter(key_cond, prog_mmap);
    std::cout << "-- PRINT FILTERED MAP --\n";
    print(mmap_filtered, '\n');




    return 0;
}


template <typename K, typename M>
void print (const std::map<K,M> &map, char delim, char end) {
    for (auto it = map.begin(); it != map.end(); ++it)
        std::cout << "[KEY]: " << it->first << " "
            << "[VALUE]: " << it->second << delim;
    putchar(end);

    return;
}

template <typename M>
void print (const std::set<M> &set, char delim, char end) {
    for (auto it = set.begin(); it != set.end(); ++it)
        std::cout << *it << delim;
    putchar(end);

    return;
}

template <typename K, typename M>
void print (const std::multimap<K,M> &mmap, char delim, char end) {
    for (const auto &it : mmap)
        std::cout << "[KEY]: " << it.first << " "
            << "[VALUE]: " << it.second << delim;
    putchar(end);

    return;
}

template <typename K, typename M>
map_const_iterator<K,M> find_by_key (const K &key, const std::map<K,M> &map) {
    bool flag_found = false;
    auto it = map.begin();
    while (flag_found == false && it != map.end()) {
        if (it->first == key)
            flag_found = true;
        else
            ++it;
    }

    return it;
}

template <typename K, typename M>
map_const_iterator<K,M> find_by_val (const M &val, const std::map<K,M> &map) {
    bool flag_found = false;
    auto it = map.begin();
    while (flag_found == false && it != map.end()) {
        if (it->second == val)
            flag_found = true;
        else
            ++it;
    }

    return it;
}

template <typename K, typename M, typename Pred_t>
std::map<K,M> map_filter (Pred_t pred, const std::map<K,M> &map) {
    std::map<K,M> rmap;

    for (auto it = map.begin(); it != map.end(); ++it) {
        if ( pred(it->first) )
            rmap.emplace(it->first, it->second);
    }

    return rmap;
}

template <typename K, typename M>
std::map<K,M> map_filter_fast (const K &key, const std::map<K,M> &map) {
    std::map<K,M> rmap;

    for (auto it = map.find(key); it != map.end(); ++it) // OK
    // for (auto it = map.find(key); it != map.end(); it = map.upper_bound(it->first)) // OK
        rmap.emplace(it->first, it->second);

    return rmap;
}

bool key_cond (const char *key) {
    return *key > KEY_THRESHOLD;
}

template <typename K, typename M>
std::set<M> get_unique_mapped (const std::map<K,M> &map) {
    std::set<M> rset;

    for (auto it = map.begin(); it != map.end(); ++it)
        rset.emplace(it->second);

    return rset;
}


template <typename K, typename M>
std::vector<M> get_all_by_key (const K &key, const std::multimap<K,M> &map) {
    std::vector<M> rvec;
    for (auto it = map.begin(); it != map.end(); ++it)
        if (it->first == key)
            rvec.emplace_back(it->second);

    return rvec;
}

template <typename K, typename M>
map_const_iterator<K,M> find_by_key (const K &key, const std::multimap<K,M> &map) {
    bool flag_found = false;
    auto it = map.begin();
    while (flag_found == false && it != map.end()) {
        if (it->first == key)
            flag_found = true;
        else
            ++it;
    }

    return it;
}

template <typename K, typename M>
map_const_iterator<K,M> find_by_val (const M &val, const std::multimap<K,M> &map) {
    bool flag_found = false;
    auto it = map.begin();
    while (flag_found == false && it != map.end()) {
        if (it->second == val)
            flag_found = true;
        else
            ++it;
    }

    return it;
}

template <typename K, typename M, typename Pred_t>
std::multimap<K,M> mmap_filter (Pred_t pred, const std::multimap<K,M> &map) {
    std::multimap<K,M> rmap;

    for (auto it = map.begin(); it != map.end(); ++it) {
        if ( pred(it->first) )
            rmap.emplace(it->first, it->second);
    }

    return rmap;
}

