#include <iostream>
#include <format>

class String {
    public:
        using size_type = size_t;

    public:
        String (char *other)
        {

        }

        String (size_type count = 256)
        {

        }

        ~String() { }

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

        }

        char* get() const noexcept
        {
            return Mycont;
        }

        char& operator[] (size_type) const noexcept
        {

        }

        cosnt char& operator[] (size_type) const noexcept
        {

        }

        String& operator= (const String& rhs)
        {

        }

        String (const String& other)
        {

        }


    protected:
        size_type Mycapacity;
        size_type Mysize;
        char      *Mystring;
};

class String_child : public String {
    using Mybase = String;
    using size_type = Mybase::size_type;
};
