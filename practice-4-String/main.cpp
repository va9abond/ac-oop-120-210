#include <iostream>
#include <format>


// null-terminated array of chars
class String { // stl: typedef string string_base<char>
    public:
        using value_type      = char;
        using size_type       = size_t;
        using pointer         = char*;
        using const_pointer   = const char*;
        using reference       = char&;
        using const_reference = const char&;

        static constexpr char NULL_S = '\0'; // nulll-terminated symbol

    public:
        String (size_type count = 256)
            : Myreserved(0)
            , Mycapacity(0)
            , Mysize(0)
            , Mystring(nullptr)
        {
            Construct_n(count);
        }

        String (const String& other)
            : Myreserved(0)
            , Mycapacity(0)
            , Mysize(0)
            , Mystring(nullptr)
        {
            if (pointer iter = other.Mystring; iter != nullptr) {
                Construct_n(other.Mycapacity);

                Mysize = other.Mysize;
                pointer myiter = Mystring;
                while ( (*myiter++ = *iter++) != NULL_S ) ;
            } // most likely case ^^^
        }

        String& operator= (const String& rhs)
        {
            if (Mystring != rhs.Mystring) {
                if (Myreserved != rhs.Myreserved) {
                    Tidy();
                    Construct_n(rhs.Mycapacity);
                } // most likely case ^^^

                Mysize = rhs.Mysize;
                pointer iter   = rhs.Unwrapped();
                pointer myiter = Mystring;
                while ( (*myiter++ = *iter++) != NULL_S ) ;
            }

            return *this;
        }

        String (const_pointer other)
            : Myreserved(0)
            , Mycapacity(0)
            , Mysize(0)
            , Mystring(nullptr)
        {
            std::cout << "String (const_pointer other): " << other << '\n';
            if (pointer iter = const_cast<pointer>(other); iter != nullptr) {
                size_type size = 0;
                while (*iter++ != '\0') ++size;

                Construct_n(size);

                Mysize = size;

                pointer myiter = Mystring;
                iter = const_cast<pointer>(other);
                while ( (*myiter++ = *iter++) != '\0' ) ;

                *(Mystring+Mysize) = NULL_S;
            }
        }

        ~String()
        {
            Tidy();
        }

    private:
        static void Xlength_error() {
            std::cout << "\033[;31;1m" << "[ERROR]: vector too long" << "\033[;0m" << '\n';
            std::exit(EXIT_FAILURE);
            // throw std::length_error(where);
        }

        static void Xrange_error() {
            std::cout << "\033[;31;1m" << "[ERROR]: invalid vector subscript" << "\033[;0m" << '\n';
            std::exit(EXIT_FAILURE);
            // throw std::out_of_range(where);
        }

        void Construct_n (const size_type count)
        {
            Myreserved = count+1;
            Mycapacity = count;
            Allocate(Myreserved);

            *Mystring = NULL_S;
        }

        void Allocate (const size_type size)
        {
            if (size <= max_size()-1 && size > 0) {
                Mystring = new value_type[size];
                return ;
            } // most likely case ^^^

            Xlength_error(); // "lol, buy more memory"
        }

        void Tidy()
        {
            if (Mystring != nullptr)
                delete[] Mystring;

            Mystring = nullptr;
            Mysize = 0;
            Mycapacity = 0;
            Myreserved = 0;
        }

    public:
        pointer Unwrapped() const noexcept
        {
            return Mystring;
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
            return Mycapacity;
        }

        char* get() const noexcept // same as Unwrapped
        {
            return Mystring;
        }

        reference operator[] (size_type pos)
        {
            if (pos >= Mysize) // rare case
                Xrange_error();

            return *(Mystring+pos);
        }

        const_reference operator[] (size_type pos) const noexcept
        {
            if (pos >= Mysize) // rare case
                Xrange_error();

            return *(Mystring+pos);
        }

        void printn()
        {
            std::cout <<
                std::format("(size: {}, capacity: {}, reserved: {}) | ",
                        Mysize, Mycapacity, Myreserved);

            if (pointer myiter = Mystring; myiter != nullptr) {
                for (; *myiter != NULL_S; ++myiter)
                    std::cout << *myiter;
                std::cout << "\\0 |;\n";
            }
        }

    protected:
        size_type Myreserved; // current storage reserved for string (capacity) including NULL_S
        size_type Mycapacity; // current storage reserved for string (capacity) excluding NULL_S
        size_type Mysize;     // current length of string (size)
        pointer   Mystring;   // pointer to the beggining of reserved memory
};


class String_child : public String {
    public:
        using Mybase          = String;
        using size_type       = Mybase::size_type;
        using value_type      = Mybase::value_type;
        using pointer         = Mybase::pointer;
        using const_pointer   = Mybase::const_pointer;
        using reference       = Mybase::reference;
        using const_reference = Mybase::const_reference;

    public:
        void reverse()
        {
            pointer left  = Mystring;
            pointer right = Mystring+Mysize-1;
            while (left < right) {
                swap(left, right);
                ++left; --right;
            }
        }

    private:
        void swap (pointer a, pointer b)
        {
            auto temp = *a;
            *a = *b;
            *b = temp;
        }
};

int main (void)
{
    String s1 ("rustem sirazetdinov");
    s1.printn();

    // const char* str = "master";
    // for (char* ptr = const_cast<char*>(str); *ptr != '\0'; ++ptr) {
    //     std::cout << *ptr << '\n';
    // }

    String s (20);
    const char* str = "master";
    s = str;
    s.printn();

    String s2(s1);
    s2.printn();

    String s3; s3 = s2;
    s3.printn();

    String_child sc (s1);
    sc.reverse();
    sc.printn();

    sc.reverse();
    sc.printn();

    return 0;
}
