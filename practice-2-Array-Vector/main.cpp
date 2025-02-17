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
            , Mysize(count)
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
            , Mysize(count)
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
        }

        static void Xrange_error() {
            std::cout << "\033[" << "[ERROR]: invalid vector subscript" << "\033[m" << '\n';
        }

        // allocate non-zero memory for 'Mycont'
        void Buy_nonzero (const size_type new_capacity)
        {
            Mycont = nullptr;

            if (new_capacity <= max_size() && new_capacity > 0) {
                Mycont = new value_type[new_capacity];
                return ;
            } // most likely case ^^^

            // std::cout << "Buy more memmory lol\n";
            Xlength_error();
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

            Mysize = 0;
            Mycapacity = 0;
        }

        // assign elements from counted range first + [0, count)
        void Assign_counted_range (pointer first, const size_type count)
        {
            if (count > Mycapacity) {
                Tidy();
                Construct_n(count);
            }

            if (pointer iter = Mycont; iter != nullptr) {
                for (size_t i = 0; i < count; ++i)
                    *iter++ = *first++;
            }

            Mycapacity = count;
            Mysize = count;
        }

    public:
        // C-array to Vector
        Vector (double *other, size_type size)
            : Mycapacity(size)
            , Mysize(size)
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
            Mycont = new double[other.Mycapacity];
            Mycapacity = other.Mycapacity;

            size_t other_size  = other.Mysize; // cached
            double *other_cont = other.Mycont; // cached
            for (size_t i = 0; i < other_size; ++i) {
                Mycont[i] = other_cont[i];
            }
            Mysize = other_size;
        }

        ~Vector()
        {
            std::cout << "[INFO]: dtor\n";
            Mycapacity = 0;
            Mycont     = 0;
            if (Mycont)
                // XXX delete vs. delete[]
                delete[] Mycont;
            Mycont = nullptr;
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
            if (pos < Mysize)
                return Mycont[pos];

            // else
            // throw(exception) std::out_or_range if pos >= size()
        }

        // access specified element (no bounds checking is performed)
        // reference operator[] (size_type pos)
        // const_reference operator[] (size_type pos) const

        // adds an element to the end
        void push_back (double val)
        {
            if (Mysize < Mycapacity-1) { // most likely case
                Mycont[Mysize++] = val;
                return ;
            }

            double *Cont_extended = new double[Mycapacity*2 + 1]; // if Mycapacity == 0
            for (size_t i = 0; i < Mysize; ++i) {
                // does it works, what is a Mycont points to after loop
                // XXX this should not work!
                *Cont_extended++ = *Mycont++;
            }

            delete[] Mycont;
            Mycont = Cont_extended;
            Mycapacity = Mycapacity*2 + 1;

            Mycont[Mysize++] = val;
        }

        // removes the last element
        void pop_back()
        {
            if (Mysize >= 0)
                --Mysize;
            // exception
        }


        void print()
        {
            std::cout <<
                std::format("(size: {}, max_size: {}) ", Mysize, Mycapacity);

            double *mycont = Mycont;
            std::cout << "{ ";
            for (size_t i = 0; i < Mysize; ++i)
                std::cout << *mycont++ << " ";
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
// [ ] Vector (double*, size_type)
// [ ] Vector (const Vector&)
// [ ] Vector& operator= (const Vector&)
// [ ] Rule of five
// [ ] ~Vector()
// [x] size_type size()
// [x] size_type max_size()
// [x] size_type capacity()
// [ ] reference at (size_type)
// [ ] const_reference at (size_type) const
// [ ] reference operator[] (size_type pos)
// [ ] const_reference operator[] (size_type pos) const
// [ ] void push_back (const value_type&)
// [ ] void pop_back ()
// [ ] print()
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
