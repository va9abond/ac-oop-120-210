#include <iostream>
#include <format>
// #include <initializer_list> // TODO

struct Circle {
    public:
        Circle (double x = 0, double y = 0, double rad = 0)
            : m_x(x)
            , m_y(y)
            , m_rad(rad)
        {
            std::cout <<
                std::format("CIRCLE CTOR: x={}, y={}, rad={}\n", x, y, rad);
        }

        Circle (const Circle& other) // = default
            : m_x(other.m_x)
            , m_y(other.m_y)
            , m_rad(other.m_rad)
        {
            std::cout << "Circle (const Circle& other)\n";
        }

        Circle& operator= (const Circle& other) // = default
        {
            std::cout << "Circle& operator= (const Circle& other)\n";
            m_x = other.m_x;
            m_y = other.m_y;
            m_rad = other.m_rad;

            return *this;
        }

        Circle& operator+= (double x)
        {
            m_rad += x;
            return *this;
        }

        Circle& operator-= (double x)
        {
            m_rad -= x;
            return *this;
        }

        Circle operator- () const
        {
            return Circle { -m_x, -m_y, -m_rad };
        }

        // void print (int precision, char end, ostream)
        // {
        //     
        // }

        // void printn (int precision)
        // {
        //
        //
        // }

        // friend Circle operator+ (Circle, double); // TODO

    private:
        double m_x;
        double m_y;
        double m_rad;
};

inline Circle operator+ (const Circle& lhs, double rhs)
{
    Circle result {lhs};
    result += rhs;

    return result;
}

inline Circle operator- (const Circle& lhs, double rhs)
{
    Circle result {lhs};
    result -= rhs;

    return result;
}


int main (void)
{
    using std::cout;
    using std::format;

    cout << format("Hello, World. {}", "Rustem\n");

    double f = 23.358495894385;
    printf("%f\n", f);
    cout << format("f = {:.4}\n", f);
    cout << format("f = {:.6}\n", f);
    cout << format("f = {:#.14}\n", f);

    // Circle c1

    // Circle c1 = Circle();
    // c1 + 20;
    //
    // Circle c2 = Circle(c1);
    // Circle c3 { 9, 24, 7 };
    // Circle c4 { Circle(23,13,31) };


    return 0;
}
