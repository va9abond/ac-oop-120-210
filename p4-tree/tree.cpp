#include <iostream>
#include <utility>
#include <cstdint>
#include <cstring>
#include <type_traits>

#define DEBUG 0


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


template <typename Value_type>
class Tree_Node {
public:
    using value_type = Value_type;
    using size_type = std::size_t;
    using Nodeptr = Tree_Node<value_type>*;


public:
    Tree_Node () = default;

    Tree_Node (const value_type &Val) :
        Myval(Val), Left(nullptr), Right(nullptr), Parent(nullptr), Ishead(false), Height(0) {}

    Tree_Node (const Tree_Node&) = delete;
    Tree_Node& operator= (const Tree_Node&) = delete;

    static Nodeptr Buy_Head_Node() {
        Nodeptr Pnode = new Tree_Node();
        if (Pnode == nullptr)
            throw std::bad_alloc();

        Pnode->Ishead = true;
        Pnode->Height = 0;
        Pnode->Left = Pnode;
        Pnode->Right = Pnode;
        Pnode->Parent = Pnode;

        return Pnode;
    }

    static Nodeptr Buy_Node (Nodeptr Myhead, const value_type &Val) {
        Nodeptr Pnode = new Tree_Node(Val);
        if (Pnode == nullptr)
            throw std::bad_alloc();

        Pnode->Ishead = false;
        Pnode->Left = Myhead;
        Pnode->Right = Myhead;
        Pnode->Parent = Myhead;
        Pnode->Height = 0;

        return Pnode;
    }

    static void Sell_Node0 (Nodeptr Ptr) {
        Ptr->Left   = nullptr;
        Ptr->Right  = nullptr;
        Ptr->Parent = nullptr;

        delete Ptr;
        Ptr = nullptr;

        return;
    }

    static void Sell_Node (Nodeptr Ptr) {
        Ptr->Myval.~value_type();
        Sell_Node0(Ptr);

        return;
    }


public:
    value_type Myval; // the stored value, unused if head
    Nodeptr Left;     // left subtree, or smallest element if head
    Nodeptr Right;    // right subtree, or largest element if head
    Nodeptr Parent;   // parent, or root of the tree if head
    bool Ishead;      // true only if head node
    size_type Height; // to easy detect unbalanced tree
};


template <typename Kty, typename Ty>
struct Tree_Traits {
    using key_type = Kty;
    using mapped_type = Ty;
    using value_type = std::pair<const key_type, Ty>;

    using pointer         = value_type*;
    using const_pointer   = const pointer;
    using reference       = value_type&;
    using const_reference = const reference;
    using size_type       = std::size_t;

    using Node    = Tree_Node<value_type>;
    using Nodeptr = Node*;


    static const key_type& Extract_Key (const Nodeptr Pnode) {
        return Pnode->Myval.first;
    }
};


enum class Traversal_Order_Tag : uint8_t {
    in_order,
    pre_order,
    post_order
};


template <
    typename Tree_traits,
    Traversal_Order_Tag Order = Traversal_Order_Tag::in_order
>
class Tree_Const_Iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;

    using Nodeptr    = typename Tree_traits::Nodeptr;
    using value_type = typename Tree_traits::value_type;
    using pointer    = typename Tree_traits::const_pointer;
    using reference  = typename Tree_traits::const_reference;


public:
    Tree_Const_Iterator (Nodeptr Parent) : Myptr(Parent) { }

    // Tree_Const_Iterator& operator= (const Tree_Const_Iterator &Rhs) = delete;

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

    Tree_Const_Iterator& operator++ () {
        switch (Order) {

            case Traversal_Order_Tag::in_order:
                if (Nodeptr Pnode = Myptr; Myptr->Right->Ishead) {

                    while ( !(Pnode = Myptr->Parent)->Ishead && Myptr == Pnode->Right )
                        Myptr = Pnode;
                    Myptr = Pnode;

                } else {

                    Pnode = Myptr->Right;
                    while (!Pnode->Left->Ishead)
                        Pnode = Pnode->Left;
                    Myptr = Pnode;

                }

                return *this;
                break;

            case Traversal_Order_Tag::pre_order:
                break;

            case Traversal_Order_Tag::post_order:
                break;
        }
    }

    Tree_Const_Iterator operator++ (int) {
        Nodeptr Temp = Myptr;
        ++(*this);
        return Tree_Const_Iterator(Temp);
    }

    Tree_Const_Iterator& operator-- () {
        switch (Order) {

            case Traversal_Order_Tag::in_order:
                if (Myptr->Ishead) // --end() -> rightmost node
                    Myptr = Myptr->Right;
                else if (Myptr->Left->Ishead) {

                    Nodeptr Pnode = nullptr;
                    while (!(Pnode = Myptr->Parent)->Ishead && Myptr == Pnode->Left)
                        Myptr = Pnode;

                    if (!Myptr->Ishead) { // decrement non-begin()
                        Myptr = Pnode;    // if Myptr->Ishead, then Myptr was
                                          // always a left child during we have
                                          // been climbind up the tree
                    }
                } else {

                    Nodeptr Pnode = Myptr->Left;
                    while (!Pnode->Right->Ishead)
                        Pnode = Pnode->Right;
                    Myptr = Pnode;

                }

                return *this;
                break;

            case Traversal_Order_Tag::pre_order:
                break;

            case Traversal_Order_Tag::post_order:
                break;
        }
    }

    Tree_Const_Iterator operator-- (int) {
        Nodeptr Temp = Myptr;
        --(*this);
        return Tree_Const_Iterator(Temp);
    }

    bool operator== (const Tree_Const_Iterator &Rhs) const {
        return Myptr == Rhs.Myptr;
    }

    bool operator!= (const Tree_Const_Iterator &Rhs) const {
        return Myptr != Rhs.Myptr;
    }

    Nodeptr Unwrapped() const {
        return Myptr;
    }


public:
    Nodeptr Myptr;
};


template <
    typename Tree_traits,
    Traversal_Order_Tag Order = Traversal_Order_Tag::in_order
>
class Tree_Iterator : public Tree_Const_Iterator<Tree_traits, Order> {
public:
    using iterator_category = std::bidirectional_iterator_tag;

    using Nodeptr    = typename Tree_traits::Nodeptr;
    using value_type = typename Tree_traits::value_type;
    using pointer    = typename Tree_traits::pointer;
    using reference  = typename Tree_traits::reference;

    using Mybase = Tree_Const_Iterator<Tree_traits, Order>;
    using Mybase::Mybase;


public:
    reference operator* () const {
        return const_cast<reference>(Mybase::operator*());
    }

    pointer operator->() const {
        return const_cast<pointer>(Mybase::operator->());
    }

    Tree_Iterator& operator++ () {
        Mybase::operator++();
        return *this;
    }

    Tree_Iterator& operator-- () {
        Mybase::operator--();
        return *this;
    }

    Tree_Iterator operator++ (int) {
        Nodeptr Temp = Mybase::Myptr;
        Mybase::operator++();
        return Tree_Iterator(Temp);
    }

    Tree_Iterator operator-- (int) {
        Nodeptr Temp = Mybase::Myptr;
        Mybase::operator--();
        return Tree_Iterator(Temp);
    }
};


template <
    typename Kty,
    typename Ty
> class tree {
public:
    using Tree_traits = Tree_Traits<Kty, Ty>;

    using key_type        = typename Tree_traits::key_type;
    using mapped_type     = typename Tree_traits::mapped_type;
    using value_type      = typename Tree_traits::value_type;
    using size_type       = typename Tree_traits::size_type;
    using pointer         = typename Tree_traits::pointer;
    using const_pointer   = typename Tree_traits::const_pointer;
    using reference       = typename Tree_traits::reference;
    using const_reference = typename Tree_traits::const_reference;

    using Node    = typename Tree_traits::Node;
    using Nodeptr = typename Tree_traits::Nodeptr;

    using iterator = Tree_Iterator<Tree_traits, Traversal_Order_Tag::in_order>;
    using const_iterator = Tree_Const_Iterator<Tree_traits, Traversal_Order_Tag::in_order>;


public:
    tree() : Myhead(Node::Buy_Head_Node()), Mysize(0) { }

    tree (std::initializer_list<value_type> ilist) :
        Myhead(Node::Buy_Head_Node()), Mysize(0) {

        for (auto it = ilist.begin(); it != ilist.end(); ++it)
            Emplace(*it);

        return;
    }

    // tree (const tree &other) {
    //     // TODO
    // }
    //
    // tree (tree &&other) : Myhead(nullptr), Mysize(0) {
    //     // TODO
    // }
    //
    // tree& operator= (const tree &other) {
    //     // TODO
    // }
    //
    // tree& operator= (const tree &&other) {
    //     // TODO
    // }

    ~tree() {
        clear();
        Node::Sell_Node0(Myhead);
    }


protected:
    enum class Tree_Child_Tag : uint8_t {
        Right,
        Left
    };

    struct Tree_Find_Result {
        Nodeptr Parent;
        Nodeptr Bound;
        Tree_Child_Tag Child_tag;
    };


protected:
    static Nodeptr Max (Nodeptr Pnode) { // rightmost node in subtree
        while (!Pnode->Right->Ishead)
            Pnode = Pnode->Right;

        return Pnode;
    }

    static Nodeptr Min (Nodeptr Pnode) { // leftmost node in subtree
        while (!Pnode->Left->Ishead)
            Pnode = Pnode->Left;

        return Pnode;
    }

    Nodeptr Next (Nodeptr Mynode) const { // return in-order successor of Node
        Nodeptr Result = Mynode;

        if ( Result->Right->Ishead ) {

            Nodeptr Pnode = nullptr;
            while ( !(Pnode = Result->Parent)->Ishead && Result == Pnode->Right )
                Result = Pnode;
            Result = Pnode;

        } else
            Result = Min(Result->Right);

        return Result;
    }

    Nodeptr Prev (Nodeptr Mynode) const noexcept { // return in-order predecessor
        Nodeptr Result = Mynode;

        if ( Result->Left->Ishead ) {

            Nodeptr Pnode = nullptr;
            while ( !(Pnode = Result->Parent)->Ishead && Result == Pnode->Left )
                Result = Pnode;

            if (!Result->Ishead)
                Result = Pnode;

        } else
            Result = Max(Result->Left);

        return Result;
    }

    // Find suitable place for node insertion (adoption)
    // Parent: parent for new node;
    // Bound:  first node that is NOT LESS than the given key;
    // Tag:    new node should be right or left child of the parent?
    Tree_Find_Result Find_Lower_Bound (const key_type &Key) const {
        Tree_Find_Result Loc = { Myhead->Parent, Myhead, Tree_Child_Tag::Right };
        Nodeptr Pnode = Loc.Parent;

        while (!Pnode->Ishead) {
            Loc.Parent = Pnode;

            if (Tree_traits::Extract_Key(Pnode) < Key) {
                Loc.Child_tag = Tree_Child_Tag::Right;
                Pnode = Pnode->Right;
            } else {
                Loc.Child_tag = Tree_Child_Tag::Left;
                Loc.Bound = Pnode;
                Pnode = Pnode->Left;
            }
        }

        return Loc;
    }

    Nodeptr Find (const key_type &Key) {
        const Tree_Find_Result Loc = Find_Lower_Bound(Key);
        return !Loc.Bound->Ishead && (Tree_traits::Extract_Key(Loc.Bound) == Key) ?
            Loc.Bound : Myhead;
    }


protected:
    void Swap_Heads (tree &Rhs) {
        // TODO
        return;
    }

    void Adopt_Node (const Tree_Find_Result &Loc, Nodeptr Pnode) {
        Pnode->Parent = Loc.Parent;

        if (Loc.Parent == Myhead) { // <=> Mysize == 0
            Myhead->Parent = Pnode;
            Myhead->Right = Pnode;
            Myhead->Left = Pnode;
        }

        if (Loc.Child_tag == Tree_Child_Tag::Right) {
            Loc.Parent->Right = Pnode;
            if (Myhead->Right == Loc.Parent)
                Myhead->Right = Pnode;
        } else {
            Loc.Parent->Left = Pnode;
            if (Myhead->Left == Loc.Parent)
                Myhead->Left = Pnode;
        }

        ++Mysize;
        Balance_Tree_Up(Loc.Parent);
        return;
    }

    // Promote left child to root of subtree
    // Return new root (ex left child)
    Nodeptr Rrotate (Nodeptr Where) {
        Nodeptr Newroot = Where->Left; // Newroot <=> x, Where <=> y
        Where->Left = Newroot->Right;

        if (!Newroot->Right->Ishead)
            Newroot->Right->Parent = Where;

        Newroot->Parent = Where->Parent;

        if (Where == Myhead->Parent)
            Myhead->Parent = Newroot;
        else if (Where == Where->Parent->Right)
            Where->Parent->Right = Newroot;
        else
            Where->Parent->Left = Newroot;

        Newroot->Right = Where;
        Where->Parent = Newroot;

        Update_Height(Where);
        Update_Height(Newroot);
        return Newroot;
    }

    // Promote right child to root of subtree
    Nodeptr Lrotate (Nodeptr Where) {
        Nodeptr Newroot = Where->Right; // Newroot <=> y, Where <=> x

        // transfer B subtree
        Where->Right = Newroot->Left;

        // if subtree B not nullptr then change is's parent
        if (!Newroot->Left->Ishead) {
            Newroot->Left->Parent = Where;
        }

        // change Parent of Newroot
        Newroot->Parent = Where->Parent;

        // change Parent to Newroot
        if (Where == Myhead->Parent) // if Where is root of entire tree
            Myhead->Parent = Newroot;
        else if (Where->Parent->Right == Where) // if Where is right child
            Where->Parent->Right = Newroot;
        else // if Where is left child
            Where->Parent->Left = Newroot;

        Newroot->Left = Where;
        Where->Parent = Newroot;

        Update_Height(Where);
        Update_Height(Newroot);
        return Newroot;
    }

    // Return new subroot
    Nodeptr Balance (Nodeptr Pnode) {
        const int Balance_factor = Skew(Pnode);

        // left subtree higher then right
        if (Balance_factor > 1) {

            if (Skew(Pnode->Right) < 0) // case 3: skew(z) = -1
                Rrotate(Pnode->Right); // Rrotate(z)
            return Lrotate(Pnode); // Lrotate(x)

        } else if (Balance_factor < -1) {

            if (Skew(Pnode->Left) > 0)
                Lrotate(Pnode->Left); // Rrotate(z)
            return Rrotate(Pnode); // Lrotate(x)

        }

        return Pnode;
    }

    void Balance_Tree_Up (Nodeptr Pnode) {
        while (!Pnode->Ishead) {
            Update_Height(Pnode);
            Pnode = Balance(Pnode);
            Pnode = Pnode->Parent;
        }

        return;
    }

    unsigned Subtree_Height (Nodeptr Subroot) const {
        return Subroot->Ishead ? 0 : Subroot->Height;
    }

    // Balance factor
    int Skew (Nodeptr Subroot) const {
        return Subroot->Ishead ?
            0 : Subtree_Height(Subroot->Right) - Subtree_Height(Subroot->Left);
    }

    void Update_Height (Nodeptr Where) {
        Where->Height = (
            Where->Left->Ishead && Where->Right->Ishead ?
                0 : 1 + std::max(Subtree_Height(Where->Left), Subtree_Height(Where->Right))
        );

        return;
    }


protected:
    std::pair<Nodeptr, bool> Emplace (const value_type &Val) {
        if (Mysize == max_size())
            throw std::runtime_error("tree too big");

        const key_type &Pnode_key = Val.first;
        Tree_Find_Result Loc = Find_Lower_Bound(Pnode_key);

        // Value with the key already exists
        if (!Loc.Bound->Ishead && (Tree_traits::Extract_Key(Loc.Bound) == Val.first)) {
            return { Myhead, false };
        }

        Nodeptr Pnode = Node::Buy_Node(Myhead, Val);
        Adopt_Node(Loc, Pnode);

        return { Pnode, true };
    }

    std::pair<Nodeptr, bool> Erase (const key_type& Key) {

        // first of all check is node with Key exists
        const Tree_Find_Result Loc = Find_Lower_Bound(Key);
        if (Loc.Bound->Ishead || Tree_traits::Extract_Key(Loc.Bound) != Key)
            return { Myhead, false };


        // case 1: Erased is leaf (no childs) ==> just erase return in-order successor
        // case 2: Erased has only one child ==> replace with it child ==> return in-order successor
        // case 3: has 2 childs ==> replace with in-order successor ==> return in-order successor

        Nodeptr Erased    = Loc.Bound;
        Nodeptr Pnode     = Erased->Parent;
        Nodeptr Result    = Next(Erased); // the node to return
        Nodeptr Rnode     = Myhead; // the node which replace Erased
        Nodeptr Startnode = Myhead; // the node from which the balancing starts

        int is_root         = 1 * (Myhead->Parent == Erased);
        bool is_left_child  = !Erased->Left->Ishead;
        bool is_right_child = !Erased->Right->Ishead;
        unsigned case_index = is_left_child * 100 + is_right_child * 200;

        switch (case_index) {
            case 0: // case 1: no childs
            switch (is_root) {
                case 1: // Erased = root (<=> Mysize = 1)
                    // Startnode = Myhead
                    // Rnode = Myhead

                    Pnode->Right = Rnode;
                    Pnode->Left = Rnode;
                    Pnode->Parent = Rnode;

                    break;
                case 0: // Erased != root
                    Startnode = Pnode;
                    // Rnode = Myhead;

                    // Erased can be both rightmost or leftmost
                    if (Myhead->Left == Erased) { Myhead->Left = Pnode; } // Next(Erased) = Pnode
                    if (Myhead->Right == Erased) { Myhead->Right = Pnode; } // Prev(Erased) = Pnode

                    if (Pnode->Left == Erased) { // Erased is left child
                        Pnode->Left = Rnode;
                    } else {
                        Pnode->Right = Rnode;
                    }

                    break;
            }
                break;

            case 100: // case 2: only left child (child is leaf!)
                Startnode = Pnode; // Pnode = Myhead
                Rnode = Erased->Left;
                switch (is_root) {
                    case 1: // Mysize = 2
                        // Erased is rightmost
                        Myhead->Right = Rnode;

                        Myhead->Parent = Rnode;
                        Rnode->Parent = Pnode; // = Myhead

                        break;
                    case 0:
                        // Erased can be rightmost, but not leftmost
                        if (Myhead->Right == Erased) {
                            Myhead->Right = Rnode;
                        }

                        Rnode->Parent = Pnode;
                        if (Pnode->Right == Erased) {
                            Pnode->Right = Rnode;
                        } else {
                            Pnode->Left = Rnode;
                        }

                        break;
                }
                break;
            case 200: // case 2: only right child (child is leaf!)
            Startnode = Pnode;
            Rnode = Erased->Right;
                switch (is_root) {
                    case 1: // Mysize = 2
                        // Erased is leftmost
                        Myhead->Left = Rnode;

                        Myhead->Parent = Rnode;
                        Rnode->Parent = Pnode; // = Myhead

                        break;
                    case 0:
                        // Erased can be leftmost. but not rightmost
                        if (Myhead->Left == Erased) {
                            Myhead->Left = Rnode;
                        }

                        Rnode->Parent = Pnode;
                        if (Pnode->Right == Erased) {
                            Pnode->Right = Rnode;
                        } else {
                            Pnode->Left = Rnode;
                        }

                        break;
                }
                break;

            case 300: // case 3: both childs
                Rnode = Result;
                Startnode = (Rnode->Parent == Erased ? Rnode : Rnode->Parent);
                switch (is_root) {
                    case 1:
                        // Erased can't be rightmost or leftmost

                        if (Erased->Right == Rnode) {
                            Myhead->Parent = Rnode;
                            Rnode->Parent = Myhead;
                            Erased->Left->Parent = Rnode;
                            Rnode->Left = Erased->Left;
                        } else {
                            Rnode->Parent->Left = Myhead;
                            Myhead->Parent = Rnode;
                            Rnode->Parent = Myhead;
                            Erased->Right->Parent = Rnode;
                            Rnode->Right = Erased->Right;
                            Erased->Left->Parent = Rnode;
                            Rnode->Left = Erased->Left;
                        }

                        break;
                    case 0:
                        // Erased can't be rightmost or leftmost

                        if (Pnode->Right == Erased) {
                            Pnode->Right = Rnode;
                        } else {
                            Pnode->Left = Rnode;
                        }

                        if (Erased->Right == Rnode) {
                            Rnode->Parent = Pnode;
                            Erased->Left->Parent = Rnode;
                            Rnode->Left = Erased->Left;
                        } else {
                            Rnode->Parent->Left = Myhead;
                            Rnode->Parent = Pnode;
                            Rnode->Right = Erased->Right;
                            Erased->Left->Parent = Rnode;
                            Rnode->Left = Erased->Left;
                        }
                        break;
                }
                break;
        }

        Update_Height(Rnode);
        Node::Sell_Node(Erased);
        --Mysize;
        Balance_Tree_Up(Startnode);

        return { Result, true };
    }


public:
    std::pair<iterator, bool> insert (const value_type &Val) {
        const auto Result = Emplace(Val);
        return { iterator(Result.first), Result.second };
    }

    std::pair<iterator, bool> erase (const key_type &Key) {
        const auto& [Nextnode, Success] = Erase(Key);
        return { iterator(Nextnode), Success };
    }


protected:
    void Erase_Tree (Nodeptr Root) {
        while (!Root->Ishead) {
            Erase_Tree(Root->Right);
            Node::Sell_Node(std::exchange(Root, Root->Left));
        }
    }

    void Erase_Head() {
        Erase_Tree(Myhead->Parent);
        Node::Sell_Node0(Myhead);
    }


public:
    void clear() {
        Erase_Tree(Myhead->Parent);

        Myhead->Parent = Myhead;
        Myhead->Left   = Myhead;
        Myhead->Right  = Myhead;
        Mysize = 0;
    }


public:
    const_iterator begin() const { // in_order traversal
        return const_iterator(Myhead->Left);
    }

    iterator begin() { // in_order traversal
        return iterator(Myhead->Left);
    }

    const_iterator end() const {
        return const_iterator(Myhead);
    }

    iterator end() {
        return iterator(Myhead);
    }

    size_type size() const {
        return Mysize;
    }

    size_type max_size() const {
        return static_cast<size_type>(100);
    }

    bool empty() const {
        return Mysize == 0;
    }

    const_iterator max() const {
        return const_iterator(Myhead->Right);
    }

    iterator max() {
        return iterator(Myhead->Right);
    }

    const_iterator min() const {
        return const_iterator(Myhead->Left);
    }

    iterator min() {
        return iterator(Myhead->Left);
    }

    const_iterator find (const key_type &Key) const {
        return const_iterator(Find(Key));
    }

    iterator find (const key_type &Key) {
        return iterator(Find(Key));
    }


public:
    iterator operator[] (const mapped_type &Mapped) const {
        iterator It = iterator(Myhead->Left);
        while (It != end() && It->second != Mapped)
            ++It;

        return It;
    }

    template <typename Pred_t>
    iterator find_by_mapped (const mapped_type &Mapped, Pred_t Pred) const {
        iterator It = iterator(Myhead->Left);
        while (It != end() && !Pred(It->second, Mapped))
            ++It;

        return It;
    }


public:
    // https://www.techiedelight.com/c-program-print-binary-tree/
    struct Trunk {
        Trunk *prev;
        std::string str;

        Trunk (Trunk *prev, std::string str) {
            this->prev = prev;
            this->str = str;
        }
    };

    // Helper function to print branches of the binary tree
    void showTrunks (Trunk *p) {
        using std::cout;

        if (p == nullptr) {
            return;
        }

        showTrunks(p->prev);
        cout << p->str;
    }

    void printTree (Nodeptr root, Trunk *prev, bool isLeft) {
        using std::string;
        using std::endl;
        using std::cout;

        if (root->Ishead) {
            return;
        }

        string prev_str = "    ";
        Trunk *trunk = new Trunk(prev, prev_str);

        printTree(root->Right, trunk, true);

        if (!prev) {
            trunk->str = "———";
        } else if (isLeft) {
            trunk->str = ".———";
            prev_str = "   |";
        } else {
            trunk->str = "`———";
            prev->str = prev_str;
        }

        showTrunks(trunk);

        const auto mv = root->Myval;
        std::cout << "{" << mv.first << ",h:" << root->Height << "}\n";
        // printf("{%d,%d; %ld}\n", mv.first, mv.second, root->Height);

        if (prev) {
            prev->str = prev_str;
        }

        trunk->str = "   |";

        printTree(root->Left, trunk, false);
        delete(trunk);
    }


public:
    void display() {
        if (Mysize == 0) {
            std::cout << "empty tree\n";
            return;
        }

        printTree(Myhead->Parent, nullptr, false);
    }


protected:
    Nodeptr Myhead; // pointer to head node
                    // Myhead->Parent = Root
                    // Myhead->Parent->Parent = Myhead
                    // Myhead->Right = Max
                    // Myhead->Left = Min
    size_type Mysize; // number of elements
};


template <
    typename Kty,
    typename Ty
> class multitree : public tree<Kty, Ty> {
public:
    using Mybase = tree<Kty, Ty>;
    using Mybase::Mybase;

    using Tree_traits = Mybase::Tree_traits;

    using key_type        = typename Tree_traits::key_type;
    using mapped_type     = typename Tree_traits::mapped_type;
    using value_type      = typename Tree_traits::value_type;
    using size_type       = typename Tree_traits::size_type;
    using pointer         = typename Tree_traits::pointer;
    using const_pointer   = typename Tree_traits::const_pointer;
    using reference       = typename Tree_traits::reference;
    using const_reference = typename Tree_traits::const_reference;

    using Node    = typename Tree_traits::Node;
    using Nodeptr = typename Tree_traits::Nodeptr;

    using iterator = Mybase::iterator;
    using const_iterator = Mybase::const_iterator;


public:
    multitree (std::initializer_list<value_type> ilist) : Mybase() {
        for (auto it = ilist.begin(); it != ilist.end(); ++it)
            Emplace(*it);

        return;
    }


protected:
    std::pair<Nodeptr, bool> Emplace (const value_type &Val) {
        if (Mybase::Mysize == Mybase::max_size())
            throw std::runtime_error("tree too big");

        Nodeptr Pnode = Node::Buy_Node(Mybase::Myhead, Val);
        const key_type &Pnode_key = Tree_traits::Extract_Key(Pnode);

        // No check if node with key already exists, just adopt
        typename Mybase::Tree_Find_Result Loc =
            Mybase::Find_Lower_Bound(Pnode_key);
        Mybase::Adopt_Node(Loc, Pnode);

        return { Pnode, true };
    }


public:
    // (O(N) case) multitree<int, int> mmap {
    //     {10, 1}, {10, 2}, {10, 3}, {10, 4}, {10, 5},
    //     {10, 6}, {10, 6}, {10, 7}, {10, 8}, {10, 9},
    // };
    priority_queue<mapped_type> operator[] (const key_type &Key) const {
        priority_queue<mapped_type> queue;

        for (auto It = Mybase::begin(); It != Mybase::end(); ++It)
            if (It->first == Key)
                queue.push(It->second);

        return queue;
    }
};


template <
    typename Kty,
    typename Ty
> class splay_tree : public tree<Kty, Ty> {
public:
    using Mybase = tree<Kty, Ty>;
    using Mybase::Mybase;

    using Tree_traits = Mybase::Tree_traits;

    using key_type        = typename Tree_traits::key_type;
    using mapped_type     = typename Tree_traits::mapped_type;
    using value_type      = typename Tree_traits::value_type;
    using size_type       = typename Tree_traits::size_type;
    using pointer         = typename Tree_traits::pointer;
    using const_pointer   = typename Tree_traits::const_pointer;
    using reference       = typename Tree_traits::reference;
    using const_reference = typename Tree_traits::const_reference;

    using Node    = typename Tree_traits::Node;
    using Nodeptr = typename Tree_traits::Nodeptr;

    using iterator = Mybase::iterator;
    using const_iterator = Mybase::const_iterator;


protected:
    /* If there is an element with the specified key,
       it promotes it to the root of the tree; otherwise,
       it promotes the last element that was accessed while
       searching for the desired element. */
    void lookup (const key_type &Key) {
        const typename Mybase::Tree_Find_Result Loc =
            Mybase::Find_Lower_Bound(Key);

        // Loc.Bound: exactly the element with the given key
        // Loc.Parent: the last element that was accessed
        Nodeptr Pnode = (!Loc.Bound->Ishead && (Tree_traits::Extract_Key(Loc.Bound) == Key)) ?
            Loc.Bound : Loc.Parent;

        while (Pnode != Mybase::Myhead->Parent)
            splay(Pnode);

        return;
    }

    // Zig | Zag | Zig-Zig | Zag-Zag | Zig-Zag | Zag-Zig
    void splay (Nodeptr Pnode) {
        if (Pnode->Ishead)
            return;

        Nodeptr Parent = Pnode->Parent;
        typename Mybase::Tree_Child_Tag Child_tag;
        if (Parent->Right == Pnode)
            Child_tag = Mybase::Tree_Child_Tag::Right;
        else
            Child_tag = Mybase::Tree_Child_Tag::Left;


        // (1) Zig | Zag
        if (Parent == Mybase::Myhead->Parent) {

            if (Child_tag == Mybase::Tree_Child_Tag::Left)
                Pnode = Mybase::Rrotate(Parent);
            else
                Pnode = Mybase::Lrotate(Parent);

        // (2) Zig-Zig
        } else if (Child_tag == Mybase::Tree_Child_Tag::Left &&
                Parent->Parent->Left == Parent) {

            Parent = Mybase::Rrotate(Parent->Parent);
            Pnode = Mybase::Rrotate(Parent);
            // if (Parent->Parent == Mybase::Myhead->Parent) {
            //     Mybase::Rrotate(Mybase::Myhead->Parent);
            //     Mybase::Rrotate(Mybase::Myhead->Parent);
            // } else {
            //     Parent = Mybase::Rrotate(Parent->Parent);
            //     Pnode = Mybase::Rrotate(Parent);
            //     splay(Loc);
            // }

        // (3) Zag-Zag
        } else if (Child_tag == Mybase::Tree_Child_Tag::Right &&
                Parent->Parent->Right == Parent) {

                Parent = Mybase::Lrotate(Parent->Parent);
                Pnode = Mybase::Lrotate(Parent);

        // (4) Zag-Zig
        } else if (Child_tag == Mybase::Tree_Child_Tag::Right &&
                Parent->Parent->Left == Parent) {

            Pnode = Mybase::Lrotate(Parent);
            Pnode = Mybase::Rrotate(Pnode->Parent);

        // (5) Zig-Zag
        } else if (Child_tag == Mybase::Tree_Child_Tag::Left &&
                Parent->Parent->Right == Parent) {

            Pnode = Mybase::Rrotate(Parent);
            Pnode = Mybase::Lrotate(Pnode->Parent);

        }

        return;
    }

    Nodeptr Find (const key_type &Key) {
        lookup(Key);

        Nodeptr root = Mybase::Myhead->Parent;
        return (Tree_traits::Extract_Key(root) == Key) ?
            root : Mybase::Myhead;
    }

public:
    const_iterator find (const key_type &Key) {
        return const_iterator(Find(Key));
    }

    // Can't overload??
    // iterator find (const key_type &Key) {
    //     return iterator(this->Find(Key));
    // }

    std::pair<iterator, bool> insert (const value_type &Val) {
        const auto &[Where, Success] = Mybase::Emplace(Val);
        lookup(Val.first);
        return { iterator(Where), Success };
    }

    std::pair<iterator, bool> erase (const key_type &Key) {
        lookup(Key);
        const auto &[Nextnode, Success] = Mybase::Erase(Key);
        return { iterator(Nextnode), Success };
    }
};


class Program {
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
                uint8_t _ver) :
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
        uint8_t ver;
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


template <typename Kty, typename Vty>
void println (const std::pair<Kty, Vty> &pair) {
    std::cout << "[KEY]: " << pair.first << " " << "[VALUE]: " << pair.second;
    putchar('\n');
    return;
}


bool is_equivalent (int a, int b) {
    return a == b;
}


bool is_equivalent_progs (const Program &a, const Program &b) {
    return a == b;
}


template <typename Kty, typename Vty>
void print (const tree<Kty, Vty> &map, char sep = ' ', char end = '\n') {
    for (auto it = map.begin(); it != map.end(); ++it)
        std::cout << "[KEY]: " << it->first << " "
            << "[VALUE]: " << it->second << sep;
    putchar(end);
}


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


int main() {
    // ************************
    // TREE 2.3
    // ************************
    tree<int, int> mymap {
        {60, 6}, {70, 7}, {80, 8}, {90, 9}, {100, 0},
        {10, 1}, {20, 2}, {30, 3}, {40, 4}, {50, 5}
    };
    mymap.display();
    putchar('\n');

    // Can't insert element with the key which already exists
    mymap.insert({100,10});
    mymap.display();
    putchar('\n');

    print(mymap, '\n');
    putchar('\n');
    putchar('\n');

    // ************************
    // MULTITREE 2.3
    // ************************
    multitree<int, int> multimap {
        {10, 1}, {10, 0}, {20, 2}, {30, 3}, {40, 4},
        {50, 5}, {60, 6}, {70, 7}, {60, 8}, {20, 9}
    };
    multimap.display();
    putchar('\n');

    puts("Find all elements in multimap by key: 20");
    auto pq = multimap[20];
    print(pq);

    printf("\n\n");



    // ************************
    // OTHER TESTS
    // ************************
    printf("Print using iterators ASCENDING order (in-order traversal)\n");
    for (auto it = mymap.begin(); it != mymap.end(); ++it)
        println(*it);
    printf("\n");

    printf("Print using iterators DESCENDING order (in-order traversal)\n");
    for (auto it = --mymap.end(); it != mymap.end(); --it)
        println(*it);
    printf("\n");

    printf("Find by key: 30\n");
    auto it_key_30 = mymap.find(30);
    if (it_key_30 != mymap.end())
        println(*it_key_30);
    else
        puts("Tree doesn't contain value with key: 30");
    printf("\n");

    printf("Find by value: 5\n");
    auto it_value_5 = mymap[5];
    if (it_value_5 != mymap.end())
        println(*it_value_5);
    else
        puts("Tree doesn't contain value: 5");
    printf("\n");

    printf("Find by value: 6\n");
    auto it_value_6 = mymap.find_by_mapped(6, is_equivalent);
    if (it_value_6 != mymap.end())
        println(*it_value_6);
    else
        puts("Tree doesn't contain value: 5");
    printf("\n");


    puts("Erase value: 30 (root) from tree");
    puts("Before:");
    mymap.display();
    mymap.erase(30);
    puts("After:");
    mymap.display();
    printf("\n");

    puts("Insert element {80, 8} in tree");
    puts("Before:");
    mymap.display();
    mymap.insert({80,8});
    puts("After:");
    mymap.display();
    putchar('\n');
    putchar('\n');
    putchar('\n');




    // ************************
    // USE CLASS Program
    // ************************
    Program prog_array[12] = {
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

    char const *keyA = "A", *keyB = "B", *keyC = "C", *keyD = "D", *keyE = "E",
         *keyF = "F", *keyG = "G", *keyH = "H", *keyI = "I", *keyJ = "J",
         *keyK = "K", *keyL = "L";

    tree<char const*, Program> prog_map = {
        { keyA, prog_array[0] },
        { keyB, prog_array[1] },
        { keyC, prog_array[2] },
        { keyD, prog_array[3] },
        { keyE, prog_array[4] },
        { keyF, prog_array[5] },
        { keyG, prog_array[6] },
        { keyH, prog_array[7] },
        { keyI, prog_array[8] },
        { keyJ, prog_array[9] },
        { keyK, prog_array[10] },
        { keyL, prog_array[11] },
    };
    prog_map.display();
    putchar('\n');

    puts("-- PRINT MAP USING ITERATORS --");
    print(prog_map, '\n');
    putchar('\n');

    printf("Find by value: prog_array[10]\n");
    auto it_value_prog_10 = prog_map.find_by_mapped(prog_array[10], is_equivalent_progs);
    if (it_value_prog_10 != prog_map.end())
        println(*it_value_prog_10);
    else
        puts("Tree doesn't contain value: prog_array[10]");
    printf("\n");

    // ************************
    // SPLAY TREE 2.5
    // ************************
    splay_tree<int, int> tsplay {
        {6, 60}, {7, 70}, {3, 30}, {4, 40}, {5, 50}
    };
    tsplay.display();
    putchar('\n');

    tsplay.find(7);
    tsplay.display();
    putchar('\n');

    tsplay.insert({10, 100});
    tsplay.display();
    putchar('\n');

    tsplay.erase(10);
    tsplay.display();
    putchar('\n');



    return 0;
}
