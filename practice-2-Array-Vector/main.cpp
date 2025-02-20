#include <iostream>
#include <format>

#define DEBUG_INFO_LEVEL 0 // 0 - turn off any debug info
                           // 1 - function type
                           // 2 - reallocations
constexpr unsigned int CAPACITY_SHADOW = 10;

class Vector {
    public:
        using value_type      = double;
        using reference       = double&;
        using const_reference = const double&;
        using pointer         = double*;
        using const_pointer   = const double*;
        using size_type       = size_t;

    public:
        // Vector with 'count' capacity (no default init-tion)
        explicit Vector (size_type count = 100)
            : Mycapacity(count) // : Mycapacity(count + CAPACITY_SHADOW)
            , Mysize(0)
        {
#if DEBUG_INFO_LEVEL == 1
            std::cout <<
                std::format("[INFO]: Vector::Vector(size_type count = {})\n", count);
#endif
            Construct_n(count);
            // TODO Vector v = { 32, 4023, 2042, 0.248, };
        }

        // Vector of 'count' values 'val'
        explicit Vector (size_type count, const value_type &val)
            : Mycapacity(count)
            , Mysize(0)
        {
#if DEBUG_INFO_LEVEL == 1
            std::cout <<
                std::format("[INFO]: Vector::Vector(size_type count = {})\n", count);
#endif
            Construct_n(count, val);
        }

    protected:
        static void Xlength_error() {
            std::cout << "\033[" << "[ERROR]: vector too long" << "\033[m" << '\n';
            std::exit(EXIT_FAILURE);
            // throw std::length_error(where);
        }

        static void Xrange_error() {
            std::cout << "\033[" << "[ERROR]: invalid vector subscript" << "\033[m" << '\n';
            std::exit(EXIT_FAILURE);
            // throw std::out_of_range(where);
        }

        // allocate non-zero memory for 'Mycont'
        void Buy_nonzero (const size_type new_capacity)
        {
            if (new_capacity <= max_size() && new_capacity > 0) {
                Mycont = new value_type[new_capacity];
                return ;
            } // most likely case ^^^

            Xlength_error(); // std::cout << "Buy more memmory lol\n";
        }

        // construct 'count' elements by 'val'
        void Construct_n (const size_type count, const value_type &val)
        {
            Buy_nonzero(count);
            if (double *iter = Mycont; iter) {
                for (size_t i = 0; i < count; ++i)
                    *iter++ = val;
            }
        }

        // construct 'count' elements with default init. = only alloc memory
        void Construct_n (const size_type count)
        {
            Buy_nonzero(count);
        }

        // destroy class invariant: free Mycont, zeros Mycapacity, Mysize
        void Tidy()
        {
            if (Mycont)
                delete[] Mycont;

            Mycont = nullptr;
            Mysize = 0;
            Mycapacity = 0;
        }

        // assign elements from counted range first + [0, count)
        void Assign_counted_range (pointer first, const size_type count)
        {
            if (count > Mycapacity) {
                Tidy();
                Construct_n(count);
                Mycapacity = count;
            }

            if (pointer iter = Mycont; iter != nullptr) {
                for (size_t i = 0; i < count; ++i)
                    *iter++ = *first++;
            }

            Mysize = count;
        }

    public:
        // C-array to Vector
        Vector (double *other, size_type size)
            : Mycapacity(size)
            , Mysize(0)
        {
#if DEBUG_INFO_LEVEL == 1
            std::cout <<
                std::format("[INFO]: Vector::Vector({}, {})\n", other, size);
#endif
            Construct_n(size);
            Assign_counted_range(other, size);
        }


// ========================
// BEGIN RULE OF FIVE (RoF)
// ========================

// [RoF] I. Copy constructor
        Vector (const Vector& other)
        {
            Construct_n(other.Mycapacity);
            Mycapacity = other.Mycapacity;

            Assign_counted_range(other.Mycont, other.Mysize);
        }

// [RoF] II. Move constructor
        Vector (Vector&& other)
        {
            Mycapacity = other.Mycapacity;
            Mysize = other.Mysize;
            Mycont = other.Mycont;

            other.Mycont = nullptr;
            other.Tidy();
        }

// [RoF] III. Copy assignment operator
        Vector& operator= (const Vector& rhs)
        {
            if (this != &rhs) {
                if (Mycapacity != rhs.Mycapacity) {
                    Tidy();
                    Construct_n(rhs.Mycapacity);
                    Mycapacity = rhs.Mycapacity;
                }

                Assign_counted_range(rhs.Mycont, rhs.Mysize);
            }

            return *this;
        }

// [RoF] IV. Move assignment operator
        Vector& operator= (Vector&& other)
        {
            Tidy();

            Mycapacity = other.Mycapacity;
            Mysize = other.Mysize;
            Mycont = other.Mycont;

            other.Mycont = nullptr;
            other.Tidy();

            return *this;
        }

// [RoF] V. Destructor
        ~Vector()
        {
#if DEBUG_INFO_LEVEL == 1
            std::cout <<
                std::format("[INFO]: Vector::~Vector()\n");
#endif
            Tidy();
        }

// ======================
// END RULE OF FIVE (RoF)
// ======================


        constexpr size_type size() const noexcept
        {
            return Mysize;
        }

        constexpr size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max();
        }

        constexpr size_type capacity() const noexcept
        {
            return Mycapacity; // std::distance(begin(), end())
            // return static_cast<size_type>(_My_data._Mylast - _My_data._Myfirst);
        }

        // access specified element with bounds checking
        reference at (size_type pos)
        {
            if (pos < Mysize) // most likely case
                return Mycont[pos];

            Xrange_error();
            throw ;
        }

        // const_reference at (size_type pos) const { }

        // access specified element (no bounds checking is performed)
        reference operator[] (size_type pos)
        {
            return *(Mycont+pos);
        }

        const_reference operator[] (size_type pos) const
        {
            return *(Mycont+pos);
        }

        // adds an element to the end
        void push_back (value_type val)
        {
            if (Mysize < Mycapacity) { // most likely case
                Mycont[Mysize++] = val;
                return ;
            }

            // size_type CAPACITY_STEP = max_size() / 50000000...; // magic variable
            size_type CAPACITY_STEP = 50; // magic variable
            size_type New_capacity = 0;
            size_type diff = max_size() - Mycapacity;
            if (diff > CAPACITY_STEP) {
                New_capacity = Mycapacity + CAPACITY_STEP;
            } else if (diff > 0) {
                New_capacity = Mycapacity + 1;
            }

            // Mycapacity == max_size() then New_capacity == 0 and
            // Construct_n(New_capacity) will cause length error
            // ^^^ to prevent type overflow
            // size_type New_capacity = (Mycapacity+1) * (Mycapacity != max_size());
            // bad: frequent memory reallocations
            // good: the ability to use the entire range of type 'size_type'

            double *Old_mycont = Mycont;
            Construct_n(New_capacity);
            Mycapacity = New_capacity;

            {
                double *myiter = Mycont;
                double *iter   = Old_mycont;
                for (size_t i = 0; i < Mysize; ++i)
                    *myiter++ = *iter++;
            }
            Mycont[Mysize++] = val;

            delete[] Old_mycont;
        }

        // removes the last element
        void pop_back() noexcept
        {
            if (Mysize > 0)
                --Mysize;
            // else do nothing
        }

        void printn()
        {
            std::cout <<
                std::format("(size: {}, capacity: {}) ", Mysize, Mycapacity);

            double *iter = Mycont;

            std::cout << "{ ";
            for (size_t i = 0; i < Mysize; ++i)
                std::cout << *iter++ << " ";
            std::cout << "};\n";
        }


    protected:
        size_type Mycapacity; // main allocated memory as number of elements
        size_type Mysize;     // number of elements
        double    *Mycont;    // pointer to the beggining of main memory
};

// https://en.cppreference.com/w/cpp/container/array/get
// get<pos>(array)


class Vector_child : public Vector {
    public:
        using Mybase          = Vector;
        using value_type      = Mybase::value_type;
        using reference       = Mybase::reference;
        using const_reference = Mybase::const_reference;
        using size_type       = Mybase::size_type;
        using pointer         = Mybase::pointer;
        using const_pointer   = Mybase::const_pointer;

    public:
        explicit Vector_child (size_type count = 100) : Vector(count)
        {
#if DEBUG_INFO_LEVEL == 1
            std::cout <<
                std::format("[INFO]: Vector_child::Vector_child(size_type count = {})\n", count);
#endif
        }

        ~Vector_child()
        {
#if DEBUG_INFO_LEVEL == 1
            std::cout <<
                std::format("[INFO]: Vector_child::~Vector_child()\n");
#endif
        }

        // canonically insert/erase apply as arguments and return iterator to
        // position. Iterator is just a wrapped pointer, and because I
        // didn't implement iterators for container, I will use raw pointers.
        // pos - index, where - pointer (iterator)
        pointer erase (const_pointer where)
        {
            // TODO check is where in [Mycont, Mycont+Mysize) range

            if (Mysize > 0) { // most likely case
                // VER.1 - on pointers
                pointer last_1  = Mycont+Mysize - 1;
                for (pointer iter = const_cast<pointer>(where);
                        iter != last_1; ++iter)
                {
                    *iter = *(iter+1);
                }
                --Mysize;
                return const_cast<pointer>(where);

                // VER.2 - on indexes (safer)
                // size_type mysize_1 = Mysize-1;
                // for (size_type i = pos; i < mysize_1; ++i) {
                //     Mycont[i] = Mycont[i+1];
                // }
                // --Mysize;
                // return Mycont[pos];
            }

            return Mycont;
        }

    private:
        // search left-to-right // XXX what if not found
        size_type find_lr (const_reference val, size_type first, size_type last)
        {
            while (first < last) {
                if (Mycont[first] == val)
                    break;
                ++first;
            }

            return first;
        }

        // XXX what if not found
        size_type find_rl (const_reference val, size_type first, size_type last)
        {
            while (last >= first) {
                if (Mycont[last] == val)
                    break;
                --last;
            }

            return last;
        }

    public:
        // XXX what if not found
        size_type find (const_reference& val,
                size_type first, size_type last, bool lr_direction = true)
        {
            // check if [first, last) in [Mycont, Mycont+Mysize) and first < last
            if (Mycont+first < Mycont || Mycont+last > Mycont+Mysize || first > last) {
                Xrange_error();
            } // rare case

            return (lr_direction) ? find_lr(val, first, last) : find_rl(val, first, last);
        }

        // insert val at where
        pointer insert (const_pointer where, const_reference val)
        {
            if (Mysize < Mycapacity) { // most likely case
                // VER.1 - raw pointers (fuuny, but dangerous)
                pointer iter = Mycont+Mysize;
                while (iter != where) {
                    *iter = *(iter-1);
                    --iter;
                }
                *const_cast<pointer>(where) = val; // *iter = val;
                ++Mysize;
                return iter;

                // VER.2 - indexes (safer)
                // for (size_type i = Mysize; i > pos; --i)
                //     Mycont[i] = Mycont[i-1];
                // Mycont[pos] = val;
                // ++Mysize;
            }

            size_type CAPACITY_STEP = 50; // magic variable
            size_type New_capacity = 0;
            size_type diff = max_size() - Mycapacity;
            if (diff > CAPACITY_STEP) {
                New_capacity = Mycapacity + CAPACITY_STEP;
            } else if (diff > 0) {
                New_capacity = Mycapacity + 1;
            }

            pointer Old_mycont = Vector::Mycont;
            pointer New_where = nullptr;
            Vector::Construct_n(New_capacity);
            Mycapacity = New_capacity;

            {
                pointer myiter = Mycont;
                pointer iter   = Old_mycont;

                while (iter < where) // (iter < where) is safer than (iter != where)
                    *myiter++ = *iter++;

                New_where = myiter;
                *myiter++ = val;

                // iter == where
                pointer end = Old_mycont+Mysize;
                while (iter < end) // (iter < end) is safer than (iter != end)
                    *myiter++ = *iter++;
            }

            ++Mysize;
            delete[] Old_mycont;

            return New_where;
        }

        // range = [first, last)
        Vector_child slice (size_type first, size_type last)
        {
            // check if [first, last) in [Mycont, Mycont+Mysize) and first < last
            if (Mycont+first < Mycont || Mycont+last > Mycont+Mysize || first > last) {
                Xrange_error();
            } // rare case

            size_type size = last - first;
            Vector_child sub_vector(size) ;

            sub_vector.Assign_counted_range(Mycont+first, size);
            // pointer sub_iter = sub_vector.Mycont;
            // pointer myiter = Mycont+first;
            // pointer myend = Mycont+last;
            // while (myiter < myend)
            //     *sub_iter++ = *myiter++;
            // sub_vector.Mysize = size;

            return sub_vector;
        }

        // range [pos, pos+count)
        Vector_child slice_by_count (size_type pos, size_type count)
        {
            if (count > Mysize-pos)
                Xrange_error();

            Vector_child sub_vector(count);

            sub_vector.Assign_counted_range(Mycont+pos, count);
            // pointer sub_iter = sub_vector.Mycont;
            // pointer myiter = Mycont+pos;
            // for (size_t i = 0; i < count; ++i)
            //     *sub_iter++ = *myiter++;

            return sub_vector;
        }

        Vector_child& operator+= (const value_type rhs)
        {
            Vector::push_back(rhs);
            return *this;
        }
};

Vector_child operator+ (const Vector_child& lhs,
                        const Vector_child::value_type rhs)
{
    Vector_child v {lhs};
    v += rhs;
    return v;
}

Vector_child normalize (const Vector_child &other)
{
    Vector_child result {other};
    if (size_t size = other.size(); size > 0)
        for (size_t i = 1; i < size-1; ++i)
            result[i] = (other[i-1]+other[i+1])/2;

    return result;
}


// =======
// ROADMAP
// =======
// >>> Vector
// [x] Vector (size_type)
// [x] Vector (size_type, const value_type&)
// [ ] Vector (std::initilizer_list)
// [x] void Construct_n (size_type)
// [x] void Construct_n (size_type, const value_type&)
// [x] void Buy_nonzero (const size_type)
// [x] Vector (double*, size_type)
// [x] Vector& operator= (const Vector&)
// [x] Vector (const Vector&)
// [x] Rule of five
// [x] ~Vector()
// [x] size_type size()
// [x] size_type max_size()
// [x] size_type capacity()
// [x] reference at (size_type)
// [ ] const_reference at (size_type) const
// [x] reference operator[] (size_type pos)
// [ ] const_reference operator[] (size_type pos) const
// [x] void push_back (const value_type&)
// [x] void pop_back ()
// [x] print()
//
//
// >>> Vector_child
// [x] Vector_child (size_type)
// [x] ~Vector_child()
// [x] size_type erase (size_type)
// [x] size_type insert (size_type, const value_type&)
// [x] size_type find_lr (const value_type&, size_type, size_type)
// [x] size_type find_rl (const value_type&, size_type, size_type)
// [x] size_type find (const value_type)
// [x] Vector_child slice (size_type, size_type)
// [x] Vector_chile& operator+= (const value_type&)
// [x] Vector_child operator+ (const Vector_child&, const value_type&)


void printn (const std::string_view& str, std::ostream& os = std::cout)
{
    // use '\n' and not std::endl to not flushing std::ostream
    os << str << '\n';
}

int main (void)
{
    printn("=== TESTING BEGIN ===");
    printn("");


    printn("[TEST]: Vector::Vector(size_type): Vector v(10)");
    Vector v(10);
    printn("");

    printn("[TEST]: Vector::push_back: v.push_back(0), ..., v.push_back(9)");
    for (int i = 0; i < 10; ++i)
        v.push_back( (double)i );
    v.printn();
    printn("");

    printn("[TEST]: Vector::push_back: v.push_back(21)");
    v.push_back(21);
    v.printn();
    printn("");

    printn("[TEST]: Vector::push_back: v.push_back(55)");
    v.push_back(55);
    v.printn();
    printn("");

    printn("[TEST]: Vector::pop_back: 4*v.pop_back()");
    v.pop_back();
    v.pop_back();
    v.pop_back();
    v.pop_back();
    v.printn();
    printn("");

    printn("[TEST]: Vector::Vector(const Vector&): Vector u(v)");
    Vector u(v);
    u.printn();
    printn("");

    printn("[TEST]: Vector::Vector(size_type): Vector h{}");
    Vector h{};
    h.printn();
    printn("");

    printn("[TEST]: Vector::operator=(const Vector&): h = u");
    h = u;
    h.printn();
    printn("");

    std::cout << std::format("[TEST]: h.at(4) = {}\n", h.at(4));
    printn("");
    std::cout << std::format("[TEST]: h[4] = {}\n", h[4]);
    printn("");

    printn("[TEST]: Vector::Vector(double *, size_type): g[] = {23, 23, 42, 8024, 20} and Vector p(g, std::size(g))");
    double g[] = {23, 23, 42, 8024, 20};
    Vector p(g, std::size(g));
    p.printn();
    printn("");


    printn("[TEST]: Vector_child::Vector_child(size_type): Vector_child vc(10)");
    Vector_child vc(10);
    printn("");

    printn("[TEST]: vc.push_back(0), ..., vc.push_back(9)");
    for (int i = 0; i < 10; ++i)
        vc.push_back( (double)i );
    vc.printn();
    printn("");

    double *iter = &vc.at(4);
    printn("[TEST]: erase(&vc.at(4))");
    vc.erase(iter);
    vc.printn();
    printn("");

    iter = &vc.at(vc.size()-1);
    printn("[TEST]: erase(&vc.at(vc.size()-1))");
    vc.erase(iter);
    vc.printn();
    printn("");

    iter = &vc[4];
    printn("[TEST]: insert(28, &vc[4])");
    iter = vc.insert(iter, 28);
    vc.printn();
    printn("");

    printn("[TEST]: insert(29, &vc[4])");
    iter = vc.insert(iter, 29);
    vc.printn();
    printn("");

    printn("[TEST]: insert(30, &vc[4])");
    iter = vc.insert(iter, 30);
    vc.printn();
    printn("");

    printn("[TEST]: insert(31, &vc[4])");
    iter = vc.insert(iter, 31);
    vc.printn();
    printn("");

    printn("[TEST]: vc.slice(2, 8).printn()");
    vc.slice(2, 8).printn();
    printn("");

    printn("[TEST]: vc.slice_by_count(4, 8).printn()");
    vc.slice_by_count(4, 8).printn();
    printn("");

    printn("[TEST]: vc.operator+=( (double)55 )");
    vc += (double)55;
    vc.printn();
    printn("");

    printn("[TEST]: vc.operator+=( (double)33 )");
    vc += (double)33;
    vc.printn();
    printn("");

    printn("[TEST]: vc.operator+=( (double)101 )");
    vc += (double)101;
    vc.printn();
    printn("");

    std::cout << std::format("[TEST]: vc.find(101, 0, vc.size()) = {}\n", vc.find(101, 0, vc.size()));
    printn("");
    std::cout << std::format("[TEST]: vc.find(55, 0, vc.size()) = {}\n", vc.find(55, 0, vc.size()));
    printn("");

    printn("[TEST]: Normalize vector (task 2.2.3)");
    normalize(vc).printn();
    printn("");


    printn("");
    printn("=== TESTING END ===");
    return 0;
}
