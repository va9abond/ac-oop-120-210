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

        String (const_pointer str)
            : Myreserved(0)
            , Mycapacity(0)
            , Mysize(0)
            , Mystring(nullptr)
        {
            if (pointer iter = const_cast<pointer>(str); iter != nullptr) {
                size_type size = 0;
                while (*iter++ != '\0') ++size;

                Construct_n(size);

                Mysize = size;

                pointer myiter = Mystring;
                iter = const_cast<pointer>(str);
                while ( (*myiter++ = *iter++) != '\0' ) ;

                *(Mystring+Mysize) = NULL_S;
            }
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

        String& operator= (const_pointer str)
        {
            size_type str_size = size(str);
            if (Mycapacity < str_size) {
                Tidy();
                Construct_n(str_size);
            } // most likely case ^^^

            Mysize = str_size;
            pointer iter = const_cast<pointer>(str);
            pointer myiter = Mystring;
            while ( (*myiter++ = *iter++) != '\0' ) ;

            return *this;
        }

        ~String()
        {
            Tidy();
        }

    protected:
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

        static size_type size (const_pointer str)
        {
            size_type size = 0;
            pointer iter = const_cast<pointer>(str);
            while (*iter++ != '\0') ++size;

            return size;
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
        String_child& operator= (const_pointer str)
        {
            size_type str_size = size(str);
            if (Mycapacity < str_size) {
                Tidy();
                Construct_n(str_size);
            } // most likely case ^^^

            Mysize = str_size;
            pointer iter = const_cast<pointer>(str);
            pointer myiter = Mystring;
            while ( (*myiter++ = *iter++) != '\0' ) ;

            return *this;
        }

        void reverse()
        {
            pointer left  = Mystring;
            pointer right = Mystring+Mysize-1;
            while (left < right) {
                swap(left, right);
                ++left; --right;
            }
        }

        void trim()
        {
            size_type left = 0;
            size_type right = Mysize-1;
            while ( isspace(Mystring[left]) ) ++left;
            while ( isspace(Mystring[right]) ) --right;

            Mysize = right - left + 1;
            for (size_type i = 0; left <= right; ++i, ++left)
                Mystring[i] = Mystring[left];

            Mystring[Mysize] = String::NULL_S;
        }

        void remove_trailing_blanks()
        {
            size_type right = Mysize-1;
            while ( isspace(Mystring[right]) ) --right;
            // now Mystring[right] point to the last non-space symbol

            Mysize = right+1;
            Mystring[Mysize] = NULL_S;
        }

        static bool isspace(const value_type ch)
        {
            unsigned char c = static_cast<unsigned char>(ch);
            return c == ' '
                || c == '\f'
                || c == '\n'
                || c == '\r'
                || c == '\t'
                || c == '\v';
        }

        size_type find_last (const char* target)
        {
            size_type size = String::size(target);

            for (int i = Mysize-1; i >= 0; --i) {
                int k = size-1;
                int j = i;
                while (j >= 0 && k >= 0 && Mystring[j] == target[k]) {
                    --j; --k;
                }

                if (k == -1) return j+1;
            }

            return Mysize;
        }

        size_type find_first (const char* target)
        {
            pointer begin = const_cast<pointer>(target);

            for (size_type i = 0; i < Mysize; ++i) {
                pointer iter = begin;
                size_type j = i;

                while (j < Mysize && Mystring[j] == *iter) {
                    ++j; ++iter;
                }

                if (*iter == '\0') return i;
            }

            return Mysize;
        }

        String_child& insert (size_type pos, const char* str)
        {
            if (pos > Mysize)
                Xrange_error();

            size_type new_size = size(str) + Mysize;
            String_child buffer(new_size);
            pointer iter = buffer.Unwrapped();


            for (size_type i = 0; i < pos; ++i)
                *iter++ = Mystring[i];

            pointer it = const_cast<pointer>(str);
            while ( (*iter = *it) != '\0' ) {
                ++iter; ++it;
            }

            for (; Mystring[pos] != NULL_S; ++pos, ++iter)
                *iter = Mystring[pos];

            *iter = NULL_S;


            pointer temp = buffer.Unwrapped();
            buffer.Mystring = Mystring;
            Mystring = temp;
            Mysize = new_size;
            Mycapacity = buffer.Mycapacity;
            Myreserved = buffer.Myreserved;

            buffer.Tidy();

            return *this;
        }

    private:
        void swap (pointer a, pointer b)
        {
            auto temp = *a;
            *a = *b;
            *b = temp;
        }
};

bool compare (const String_child& lhs, const String_child& rhs)
{
    auto iter1 = lhs.Unwrapped();
    auto iter2 = rhs.Unwrapped();

    for (; *iter1 == *iter2; ++iter1, ++iter2)
        if (*iter1 == '\0') return true;

    return false;
}


void printn (const std::string_view& str, std::ostream& os = std::cout)
{
    os << str << '\n';
}

void newline()
{
    printn("");
}

void test_name (const std::string_view& test_name)
{
    using std::cout; using std::format;
    static int test_no = 0;
    cout << format("\033[;1;33m[TEST {}]\033[;0m\n", ++test_no);
    cout << "\033[;1;32m" << test_name << "\033[;0m" << '\n';
}


int main (void)
{
    using std::cout; using std::format;
    cout << "\033[;1;31m=== TESTING BEGIN ===\033[;0m\n";
    newline();
    newline();


    constexpr char STRING_LITERAL[] = "Lorem ipsum dolor sit amet";

    test_name("Ctor: String(const char *)");
    String s (STRING_LITERAL);
    s.printn();
    newline();

    test_name("Ctor: String(size_t 20)");
    String s1;
    s1.printn();
    newline();

    test_name("String& operator= (const char *)");
    s1 = STRING_LITERAL;
    s1.printn();
    newline();

    test_name("Ctor: String (const String& other)");
    String s2(s1);
    s2.printn();
    newline();

    test_name("String& operator= (const String& other)");
    String s3; s3 = s2;
    s3.printn();
    newline();

    test_name("default Ctor: String_child (const String& other)");
    String_child sc (s);
    newline();

    test_name("Ctor: String_child(const char *)");
    String scc (STRING_LITERAL);
    scc.printn();
    newline();

    test_name("Ctor: String_child(size_t 250)");
    String scc1(250);
    scc1.printn();
    newline();

    test_name("String_child& operator= (const char *)");
    scc1 = STRING_LITERAL;
    scc1.printn();
    newline();

    test_name("Ctor: String_child (const String_child& other)");
    String scc2(scc1);
    scc2.printn();
    newline();

    test_name("String& operator= (const String_child& other)");
    String scc3; scc3 = scc2;
    scc3.printn();
    newline();

    test_name("reverse string: String_child::reverse()");
    sc.reverse();
    sc.printn();
    newline();

    test_name("reverse string again: String_child::reverse()");
    sc.reverse();
    sc.printn();
    newline();

    test_name("remove whitespaces before and after: String_child::trim()");
    sc = "   one two  three          ";
    sc.printn(); sc.trim(); sc.printn();
    newline();

    test_name("remove whitespaces before and after: String_child::trim()");
    sc = "   one two  three          -";
    sc.printn(); sc.trim(); sc.printn();
    newline();

    test_name("remove whitespaces before and after: String_child::trim()");
    sc = "-   one two  three          ";
    sc.printn(); sc.trim(); sc.printn();
    newline();

    test_name("remove trailing blanks: String_child::remove_trailing_blanks()");
    sc = "-   one two three    ";
    sc.printn(); sc.remove_trailing_blanks(); sc.printn();
    newline();

    test_name("remove trailing blanks: String_child::remove_trailing_blanks()");
    sc = "one two three   ";
    sc.printn(); sc.remove_trailing_blanks(); sc.printn();
    newline();

    String_child sc1(STRING_LITERAL);
    String_child sc2(STRING_LITERAL);

    test_name("compare strings sc1 and sc2");
    sc1.printn();
    sc2.printn();
    std::cout << std::format("sc1 == sc2 is {}\n", compare(sc1, sc2));
    newline();

    test_name("compare strings sc1 and sc2");
    sc2 = "rustem sirazetdinov";
    sc1.printn();
    sc2.printn();
    std::cout << std::format("sc1 == sc2 is {}\n", compare(sc1, sc2));
    newline();

    test_name("String_child::find_first()");
    String_child source (STRING_LITERAL);
    constexpr char target1[] = " dol";
    std::cout << "find_first occurance of \"" << target1 << "\" in ";
    source.printn();
    std::cout << "pos: " << source.find_first(target1) << '\n';
    newline();

    constexpr char target2[] = "xxx";
    std::cout << "find_first occurance of \"" << target2 << "\" in ";
    source.printn();
    std::cout << "pos: " << source.find_first(target2) << '\n';
    newline();

    test_name("String_child::find_first()");
    constexpr char target3[] = " am";
    std::cout << "find_first occurance of \"" << target3 << "\" in ";
    source.printn();
    std::cout << "pos: " << source.find_last(target3) << '\n';
    newline();

    std::cout << "find_first occurance of \"" << target2 << "\" in ";
    source.printn();
    std::cout << "pos: " << source.find_last(target2) << '\n';
    newline();

    test_name("insert: String_child::insert(size_t pos, const char*)");
    sc = STRING_LITERAL; sc.printn();
    sc.insert(4, "XXXXX"); sc.printn();
    newline();



    newline();
    cout << "\033[;1;31m=== TESTING END ===\033[;0m\n";

    return 0;
}
