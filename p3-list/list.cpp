#include <cstring>
#include <iostream>
#include <cstdint>
#include <exception>
#include <algorithm>


#define BUFSIZE 12


template <typename Value_type>
class List_Node {
public:
    using value_type = Value_type;
    using Nodeptr = List_Node<value_type>*;

public:
    List_Node () = default;

    List_Node (const value_type &Val, Nodeptr Prev, Nodeptr Next)
        : Myval(Val), Prev(Prev), Next(Next) { }

    List_Node (const List_Node&) = delete;
    List_Node& operator= (const List_Node&) = delete;

    static Nodeptr Buy_Head_Node() {
        Nodeptr Pnode = new List_Node<value_type>();
        if (Pnode == nullptr)
            throw std::bad_alloc();
        // throw std::bad_alloc("Can't allocate memory for new node");

        Pnode->Prev = Pnode;
        Pnode->Next = Pnode;

        return Pnode;
    }

    static Nodeptr Construct_Node (const value_type &Val) {
        Nodeptr Pnode = new List_Node<value_type>(Val, nullptr, nullptr);
        if (Pnode == nullptr)
            throw std::bad_alloc();
            // throw std::bad_alloc("Can't allocate memory for new node");

        Pnode->Prev = Pnode;
        Pnode->Next = Pnode;

        return Pnode;
    }

    static void Sell_Node (Nodeptr Ptr) {
        Ptr->Myval.~Value_type();
        Ptr->Next = nullptr;
        Ptr->Prev = nullptr;

        delete Ptr;
        Ptr = nullptr;

        return;
    }

    static void Sell_Non_Head (Nodeptr Head) {
        Head->Prev->Next = nullptr;

        Nodeptr Cnode = Head->Next;
        for (Nodeptr Nnode; Cnode; Cnode = Nnode) {
            Nnode = Cnode->Next;
            Sell_Node(Cnode);
        }

        return;
    }


public:
    value_type Myval;
    Nodeptr Prev;
    Nodeptr Next;
};


template <typename List_traits>
class List_Const_Iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;

    using Nodeptr    = typename List_traits::Nodeptr;
    using value_type = typename List_traits::value_type;
    using pointer    = typename List_traits::const_pointer;
    using reference  = typename List_traits::const_reference;

public:
    // List_Const_Iterator (void) = delete;
    List_Const_Iterator (Nodeptr Parent) : Myptr(Parent) {}

    reference operator*() const {
        if (Myptr == nullptr)
            throw std::runtime_error("Can't dereference null iterator");
        return Myptr->Myval;
    }

    pointer operator->() const {
        if (Myptr == nullptr)
            throw std::runtime_error("Can't accessing through null iterator");
        return &(Myptr->Myval);
    }

    List_Const_Iterator& operator++() {
        Myptr = Myptr->Next;
        return *this;
    }

    List_Const_Iterator operator++(int) {
        List_Const_Iterator Temp = *this;
        ++(*this); // this->operator++();
        return Temp;
    }

    List_Const_Iterator& operator--() {
        Myptr = Myptr->Prev;
        return *this;
    }

    List_Const_Iterator operator--(int) {
        List_Const_Iterator Temp = *this;
        --(*this);
        return Temp;
    }

    bool operator== (const List_Const_Iterator& Rhs) const {
        return Myptr == Rhs.Myptr;
    }

    bool operator!= (const List_Const_Iterator& Rhs) const {
        return Myptr != Rhs.Myptr;
    }


public:
    Nodeptr Myptr;
};

template <typename List_traits>
class List_Iterator : public List_Const_Iterator<List_traits> {
public:
    using iterator_category = std::bidirectional_iterator_tag;

    using Nodeptr    = typename List_traits::Nodeptr;
    using value_type = typename List_traits::value_type;
    using pointer    = typename List_traits::pointer;
    using reference  = typename List_traits::reference;

    using Mybase = List_Const_Iterator<List_traits>;
    using Mybase::Mybase;

public:
    reference operator*() const {
        return const_cast<reference>(Mybase::operator*());
    }

    pointer operator->() const {
        return const_cast<pointer>(Mybase::operator->());
    }

    List_Iterator& operator++() {
        Mybase::operator++();
        return *this;
    }

    List_Iterator operator++(int) {
        List_Iterator Temp = *this;
        Mybase::operator++();
        return Temp;
    }

    List_Iterator& operator--() {
        Mybase::operator--();
        return *this;
    }

    List_Iterator operator--(int) {
        List_Iterator Temp = *this;
        Mybase::operator--();
        return Temp;
    }
};


template <typename Value_type>
struct List_Traits {
    using value_type = Value_type;

    using pointer         = value_type*;
    using const_pointer   = const pointer;
    using reference       = value_type&;
    using const_reference = const reference;
    using size_type       = std::size_t;

    using Node    = List_Node<value_type>;
    using Nodeptr = Node*;
};


template <typename Ty>
class list {
public:
    using List_traits = List_Traits<Ty>;

    using value_type      = typename List_traits::value_type;
    using size_type       = typename List_traits::size_type;
    using pointer         = typename List_traits::pointer;
    using const_pointer   = typename List_traits::const_pointer;
    using reference       = typename List_traits::reference;
    using const_reference = typename List_traits::const_reference;

    using Node    = typename List_traits::Node;
    using Nodeptr = typename List_traits::Nodeptr;

    using iterator       = List_Iterator<List_traits>;
    using const_iterator = List_Const_Iterator<List_traits>;

public:
    list() : Myhead(Node::Buy_Head_Node()), Mysize(0) { }

    list (std::initializer_list<value_type> ilist) :
        Myhead(Node::Buy_Head_Node()), Mysize(0) {
        for (auto it = ilist.begin(); it != ilist.end(); ++it) {
            Emplace(Myhead, *it);
        }
    }

    list (const list &other) : Myhead(Node::Buy_Head_Node()), Mysize(0) {
        std::cout << "[DEBUG] copy ctor" << '\n';
        for (
                Nodeptr Pnode = other.Myhead->Next;
                Pnode != other.Myhead;
                Pnode = Pnode->Next
        )
            Emplace(Myhead, Pnode->Myval);
    }

    list (list &&other) : Myhead(Node::Buy_Head_Node()), Mysize(0) {
        std::cout << "[DEBUG] move ctor" << '\n';
        Swap_Heads(other);
    }

    list& operator= (const list &other) {
        std::cout << "[DEBUG] copy assignment" << '\n';
        if (this == &other)
            return *this;

        clear();

        for (
                Nodeptr Pnode = other.Myhead->Next;
                Pnode != other.Myhead;
                Pnode = Pnode->Next
        )
            Emplace(Myhead, Pnode->Myval);

        return *this;
    }

    list& operator= (list &&other) {
        std::cout << "[DEBUG] move assignment" << '\n';
        if (this == &other)
            return *this;

        clear();
        Swap_Heads(other);

        return *this;
    }

    ~list() {
        clear();
        Node::Sell_Node(Myhead);
    }


private:
    void Swap_Heads (list &Rhs) {
        Nodeptr Temp_head = Rhs.Myhead;
        Rhs.Myhead = Myhead;
        Myhead = Temp_head;

        size_type Temp_size = Rhs.Mysize;
        Rhs.Mysize = Mysize;
        Mysize = Temp_size;

        return;
    }

    void Attach_Before (const Nodeptr Where, const Nodeptr What) {
        Nodeptr Attach_after = Where->Prev;
        Attach_after->Next = What;
        Where->Prev = What;

        What->Prev = Attach_after;
        What->Next = Where;

        return;
    }

    Nodeptr Emplace (const Nodeptr Where, const value_type &Val) {
        if (Mysize == max_size())
            throw std::runtime_error("list too long");

        Nodeptr Pnode = Node::Construct_Node(Val);
        Attach_Before(Where, Pnode);
        ++Mysize;

        return Pnode;
    }

public:
    iterator insert (const_iterator Where, const value_type &Val) {
        Nodeptr Inserted = Emplace(Where.Myptr, Val);
        return iterator(Inserted);
    }

    void push_front (const value_type &Val) {
        Emplace(Myhead->Next, Val);
        return;
    }

    void push_back (const value_type &Val) {
        Emplace(Myhead, Val);
        return;
    }

private:
    Nodeptr Erase_At (const Nodeptr Where) {
        Nodeptr Erase_before = Where->Prev;
        Nodeptr Erase_after  = Where->Next;

        Erase_before->Next = Erase_after;
        Erase_after->Prev = Erase_before;

        Node::Sell_Node(Where);
        --Mysize;

        return Erase_after;
    }

public:
    iterator erase (const_iterator Where) {
        if (Where.Myptr == Myhead)
            throw std::runtime_error("Can't use end() as erase position");

        Nodeptr Erase_after = Erase_At(Where.Myptr);

        return iterator(Erase_after);
    }

    void pop_front() {
        if (Mysize == 0)
            throw std::runtime_error("pop_front() called on empty list");

        Erase_At(Myhead->Next);
        return;
    }

    void pop_back() {
        if (Mysize == 0)
            throw std::runtime_error("pop_back() called on empty list");

        Erase_At(Myhead->Prev);
        return;
    }

    iterator begin() {
        return iterator(Myhead->Next);
    }

    const_iterator begin() const {
        return const_iterator(Myhead->Next);
    }

    iterator end() {
        return iterator(Myhead);
    }

    const_iterator end() const {
        return const_iterator(Myhead);
    }

    size_type size() const {
        return Mysize;
    }

    size_type max_size() const {
        return static_cast<size_type>(100);
    }

    void clear() {
        Node::Sell_Non_Head(Myhead);

        Myhead->Next = Myhead;
        Myhead->Prev = Myhead;
        Mysize = 0;

        return;
    }


public:
    Nodeptr   Myhead; // = Tail
    size_type Mysize;
};


template <typename Ty>
class queue : public list<Ty> {
public:
    using Mybase = list<Ty>;
    using Mybase::Mybase;

    using value_type      = typename Mybase::value_type;
    using size_type       = typename Mybase::size_type;
    using pointer         = typename Mybase::pointer;
    using const_pointer   = typename Mybase::const_pointer;
    using reference       = typename Mybase::reference;
    using const_reference = typename Mybase::const_reference;

    using Node    = typename Mybase::Node;
    using Nodeptr = typename Mybase::Nodeptr;

    using iterator       = typename Mybase::iterator;
    using const_iterator = typename Mybase::const_iterator;

public:
    // queue (const queue&) = delete;
    // queue (const queue&&) = delete;
    // queue& operator= (const queue&) = delete;
    // queue& operator= (const queue&&) = delete;

public:
    void push (const value_type &Val) {
        Mybase::push_back(Val);
        return;
    }

    void pop() {
        Mybase::pop_front();
        return;
    }
};


template <typename Ty>
class priority_queue : public queue<Ty> {
public:
    using Mybase = queue<Ty>;
    using Mybase::Mybase;

    using value_type      = typename Mybase::value_type;
    using size_type       = typename Mybase::size_type;
    using pointer         = typename Mybase::pointer;
    using const_pointer   = typename Mybase::const_pointer;
    using reference       = typename Mybase::reference;
    using const_reference = typename Mybase::const_reference;

    using Node    = typename Mybase::Node;
    using Nodeptr = typename Mybase::Nodeptr;

    using iterator       = typename Mybase::iterator;
    using const_iterator = typename Mybase::const_iterator;

public:
    priority_queue (std::initializer_list<value_type> ilist) = delete;
    // priority_queue (const priority_queue&) = delete;
    // priority_queue (const priority_queue&&) = delete;
    // priority_queue& operator= (const priority_queue&) = delete;
    // priority_queue& operator= (const priority_queue&&) = delete;

public:
    void push (const value_type &Val) {
        Nodeptr Where = Mybase::Myhead->Next;
        while (Where != Mybase::Myhead && Where->Myval > Val) {
            Where = Where->Next;
        }

        Mybase::insert(iterator(Where), Val);
        return;
    }

    void pop() {
        Mybase::pop_front();
        return;
    }
};


template <typename Ty>
class stack : public list<Ty> {
public:
    using Mybase = list<Ty>;
    using Mybase::Mybase;

    using value_type      = typename Mybase::value_type;
    using size_type       = typename Mybase::size_type;
    using pointer         = typename Mybase::pointer;
    using const_pointer   = typename Mybase::const_pointer;
    using reference       = typename Mybase::reference;
    using const_reference = typename Mybase::const_reference;

    using Node    = typename Mybase::Node;
    using Nodeptr = typename Mybase::Nodeptr;

    using iterator       = typename Mybase::iterator;
    using const_iterator = typename Mybase::const_iterator;

public:
    // stack (const stack&) = delete;
    // stack (const stack&&) = delete;
    // stack& operator= (const stack&) = delete;
    // stack& operator= (const stack&&) = delete;

public:
    void push (const value_type &Val) {
        Mybase::push_front(Val);
        return;
    }

    void pop() {
        Mybase::pop_front();
        return;
    }
};


template <typename T>
void print (const list<T> &list, char delim = ' ', char end = '\n') {
    std::cout << "[" << list.size() << "] ";
    for (auto it = list.begin(); it != list.end(); ++it)
        std::cout << *it << delim;
    putchar(end);

    return;
}

template <typename T>
void print (const queue<T> &queue, char delim = ' ', char end = '\n') {
    std::cout << "[" << queue.size() << "] ";
    for (auto it = queue.begin(); it != queue.end(); ++it)
        std::cout << *it << delim;
    putchar(end);

    return;
}

template <typename T>
void print (const stack<T> &stack, char delim = ' ', char end = '\n') {
    std::cout << "[" << stack.size() << "] ";
    for (auto it = stack.begin(); it != stack.end(); ++it)
        std::cout << *it << delim;
    putchar(end);

    return;
}


template <typename Cont_t, typename Pred_t>
queue<typename Cont_t::value_type> filter (Cont_t const &Cont, Pred_t Pred) {
    using Value_type = typename Cont_t::value_type;

    queue<Value_type> rqueue;

    for (auto it = Cont.begin(); it != Cont.end(); ++it)
        if (Pred(*it))
            rqueue.push(*it);

    return rqueue;
}

int predicate (int x) {
    return x > 11;
}


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

        bool operator > (const Program &rhs) const {
            return !(*this < rhs) && *this != rhs;
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
    list<int> l;
    stack<int> s;
    priority_queue<int> pq;

    queue<int> q = {10, 11, 12, 13, 14, 15, 16};
    print(q);

    print(filter(q, predicate));

    Program prgs[BUFSIZE] = {
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

    priority_queue<Program> qp;
    qp.push(prgs[0]);
    qp.push(prgs[1]);
    qp.push(prgs[2]);
    qp.push(prgs[3]);
    qp.push(prgs[4]);
    print(qp, '\n');

    qp.pop();
    print(qp, '\n');




    // puts("[DEBUG] HERE 1");
    // queue<int> dq(std::move(q));
    // print(dq);
    // puts("[DEBUG] HERE 2");
    // queue<int> dqq; dqq = std::move(dq);
    // print(dqq);




    // s.push(10);
    // s.push(11);
    // s.push(12);
    // s.push(13);
    // s.push(14);
    // print(s);
    //
    // s.pop();
    // s.pop();
    // s.pop();
    // print(s);
    //
    //
    // l.insert(l.begin(), 10);
    // l.insert(l.begin(), 11);
    // l.insert(l.begin(), 12);
    // l.insert(l.begin(), 13);
    // l.insert(l.begin(), 14);
    // l.insert(l.begin(), 15);
    // l.insert(l.begin(), 16);
    // l.insert(l.begin(), 17);
    // print(l);
    //
    // l.push_front(100);
    // l.push_front(101);
    // l.push_front(102);
    // l.push_front(103);
    // print(l);
    //
    // l.push_back(104);
    // l.push_back(105);
    // l.push_back(106);
    // l.push_back(107);
    // print(l);
    //
    // l.pop_front();
    // l.pop_front();
    // l.pop_front();
    // l.pop_front();
    // print(l);
    //
    // l.pop_back();
    // l.pop_back();
    // l.pop_back();
    // l.pop_back();
    // print(l);
    //
    // auto it = l.begin();
    //
    // l.erase(it++);
    // print(l);
    //
    // for (int i = 0; i < 4; ++i)
    //     ++it;
    // it = l.erase(it);
    // print(l);
    //
    // l.insert(it, 100);
    // print(l);
    //
    // l.clear();
    // print(l);

    return 0;
}

