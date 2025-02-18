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

    private:
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

        Vector (const Vector& other)
        {
            Construct_n(other.Mycapacity);
            Mycapacity = other.Mycapacity;

            Assign_counted_range(other.Mycont, other.Mysize);
        }

        ~Vector()
        {
#if DEBUG_INFO_LEVEL == 1
            std::cout <<
                std::format("[INFO]: Vector::~Vector()\n");
#endif
            Tidy();
        }

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

        // const_reference operator[] (size_type pos) const { }

        // adds an element to the end
        void push_back (double val)
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

    public:
        Vector_child (size_t size = 100) : Vector(size)
        {
            std::cout << "[INFO]: Vector_child ctor\n";
        }

        ~Vector_child()
        {
            std::cout << "[INFO]: Vector_child dtor\n";
        }

        // XXX canonically insert/erase return iterators to pos, but iterators
        //     are wrapped pointer, should i return raw pointers?
        size_t erase (size_t pos = -1)
        {
            if (Mysize >= 0) { // most likely case

                while (pos < 0)
                    pos += this->Mysize;
                    // pos += ::Mysize;

                double *mycont = Mycont+pos;
                size_t mysize_1 = Mysize-1; // cached // init inside for/while
                for (size_t i = pos; i < mysize_1; ++i) {
                    Mycont[i] = Mycont[i+1];
                }
                --Mysize;

            }

            // throw
        }

    private:
        // search left-to-right
        size_t find_lr (const double& val, size_t first, size_t last)
        { // [!] first <= last
            while (first != last && Mycont[first] != val)
                ++first;

            return first;
        }

        size_t find_rl (const double& val, size_t first, size_t last)
        { // [!] first >= last
            first -= 1 * (first == Mysize);

            while (first != last && Mycont[first] != val)
                --first;

            return first;
        }

    public:
        // XXX function-changer iter as a param
        // search from start: first = 0, last = Mysize
        // search from end:   first = Mysize, last = 0 (first > last)
        // search in range [begin, end): first = begin, last = end (begin <= end)
        // search in range (begin, end]: first = begin, last = end (begin >= end)
        // but search in range (begin, end) if first == Mysize
        size_t find (const double& val, size_t first = 0, size_t last = Mysize)
        {
            // TODO check if range is valid
            return (first < last) ? find_lr(val, first, last) : find_rl(val, first, last);
        }

        size_t insert (size_t pos, const double& val)
        {
            if (Mysize >= Mycapacity-1) { // rare case
                double *Cont_extended = new double[Mycapacity*2 + 1]; // +1 if Mycapacity == 0
                for (size_t i = 0; i < pos; ++i) {
                    Cont_extended[i] = Mycont[i];
                }

                // Cont_extended[pos] = val;

                for (size_t i = pos+1; i <= Mysize; ++i)
                    Cont_extended[i] = Mycont[i-1];

                delete[] Mycont;
                Mycont = Cont_extended;
                Mycapacity = Mycapacity*2 + 1;
            }

            for (size_t i = Mysize; i > pos; --i)
                Mycont[i] = Mycont[i-1];

            Mycont[pos] = val;
            ++Mysize;
        }

        // range = [first, last)
        Vector_child slice (size_t first, size_t last)
        {
            // assert(first > last);
            size_t size = last - first;
            Vector_child sub_vector(size) ;

            double *cont = sub_vector.Mycont;
            for (size_t i = 0; i < size; ++i)
                cont[i] = this->Mycont[i+first];
            Mysize = size;

            return sub_vector;
        }

        // Vector_child slice (size_t first, size_t size)
        // {
        //     // assert(size > 0);
        //
        // }

        Vector_child& operator+= (const double rhs)
        {
            this->push(rhs);
            return *this;
        }
};

Vector_child operator+ (const Vector_child& lhs, const double rhs)
{
    Vector_child v {lhs};
    v += rhs;
    return v;
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
// [ ] Rule of five
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
// [ ] Vector_child (size_type)
// [ ] ~Vector_child()
// [ ] size_type erase (size_type)
// [ ] size_type insert (size_type, const value_type&)
// [ ] size_type find_lr (const value_type&, size_type, size_type)
// [ ] size_type find_rl (const value_type&, size_type, size_type)
// [ ] size_type find (const value_type)
// [ ] Vector_child slice (size_type, size_type)
// [ ] Vector_chile& operator+= (const value_type&)
// [ ] Vector_child operator+ (const Vector_child&, const value_type&)

int main (void)
{
    Vector v(10);

    for (int i = 0; i < 10; ++i)
        v.push_back( (double)i );

    v.printn();

    v.push_back(21);
    v.push_back(55);

    v.printn();

    v.pop_back();
    v.pop_back();
    v.pop_back();
    v.pop_back();

    v.printn();

    Vector u(v);
    u.printn();

    Vector h{};
    h.printn();

    h = u;
    h.printn();

    std::cout << std::format("h.at(4) = {}\n", h.at(4));
    std::cout << std::format("h[4] = {}\n", h[4]);

    double g[] = {23, 23, 42, 8024, 20};
    Vector p(g, std::size(g));
    p.printn();


    return 0;
}
