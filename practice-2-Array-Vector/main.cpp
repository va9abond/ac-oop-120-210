#include <iostream>
#include <format>

// dynamic array is vector
class Vector {
    public:
        using value_type      = double;
        using reference       = double&;
        using const_reference = const double&;
        using size_type       = size_t;

    public:
        Vector (size_t size = 100)
        {
            std::cout << "[INFO]: ctor\n";
            Mycont = new double[size];
            Mycapacity = size;
            Mysize = 0;
        }

        // C-array to Vector
        Vector (double *other, size_t size)
        {
            std::cout << "[INFO]: ctor\n";
            Mycont = new double[size];
            Mycapacity = size;
            Mysize = size;

            for (size_t i = 0; i < size; ++i) {
                *Mycont++ = *other++; // XXX it doesn't work
            }
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

        size_t max_size()
        {
            return std::numeric_limits<size_t>::max();
        }

        size_t capacity()
        {
            return Mycapacity;
        }

        size_t size()
        {
            return Mysize;
        }

        // bounds checking
        // reference at (size_type pos)
        // const_reference at (size_type pos) const
        double at (size_t pos)
        {
            if (pos < Mysize)
                return Mycont[pos];

            // else
            // throw(exception) std::out_or_range if pos >= size()
        }

        // no bounds checking is performed
        // reference operator[] (size_type pos)
        // const_reference operator[] (size_type pos) const

        void set_at (size_t pos, double val)
        {
            if (pos < Mysize)
                Mycont[pos] = val;
            // else
            // throw(exception) std::out_or_range if pos >= size()
        }

        void push (double val)
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

        void pop()
        {
            if (Mysize >= 0)
                --Mysize;
            // exception
        }

        Vector& operator= (const Vector& other)
        {
            if (Mycapacity != other.Mycapacity) {
                // reallocate
            }
            Mycapacity = other.Mycapacity;

            size_t other_size  = other.Mysize; // cached
            double *other_cont = other.Mycont; // cached
            for (size_t i = 0; i < other_size; ++i) {
                Mycont[i] = other_cont[i];
            }
            Mysize = other_size;

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
        size_t Mycapacity;
        size_t Mysize;
        double *Mycont;
};

// https://en.cppreference.com/w/cpp/container/array/get
// get<pos>(array)


class Vector_child : public Vector {
    public:
        Vector_child (size_t size = 100) : Vector(size)
        {
            std::cout << "[INFO]: Vector_child ctor\n";
        }

        ~Vector_child()
        {
            std::cout << "[INFO]: Vector_child dtor\n";
        }

        void remove (size_t pos = -1)
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

        void insert (size_t pos, const double& val)
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

