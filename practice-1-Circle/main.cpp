#include <iostream>
#include <iomanip>
#include <string>
#include <format>

#define DEBUG_INFO_FUNCTION_TYPE 0
// constexpr int DEBUG_INDENT = (int)std::size("[INFO]:"); // EXPERIMENTAL


struct Circle {
    public:
        Circle (double x = 0, double y = 0, double rad = 0) // = default
            : m_x(x)
            , m_y(y)
            , m_rad(rad)
        {
#if DEBUG_INFO_FUNCTION_TYPE
            std::cout <<
                std::format("[INFO]: Circle::Circle(x={}, y={}, rad={})\n", x, y, rad);
#endif
        }

        Circle (const Circle& other) // = default
            : m_x(other.m_x)
            , m_y(other.m_y)
            , m_rad(other.m_rad)
        {
#if DEBUG_INFO_FUNCTION_TYPE
            std::cout << "[INFO]: Circle::Circle (const Circle& other)\n";
#endif
        }

        Circle& operator= (const Circle& other) // = default
        {
#if DEBUG_INFO_FUNCTION_TYPE
            std::cout << "[INFO]: Circle::Circle& operator= (const Circle& other)\n";
#endif
            m_x = other.m_x;
            m_y = other.m_y;
            m_rad = other.m_rad;

            return *this;
        }

        Circle& operator+= (double x)
        {
#if DEBUG_INFO_FUNCTION_TYPE
            std::cout << "[INFO]: Circle::Circle& operator+= (double x)\n";
#endif
            m_rad += x;
            return *this;
        }

        Circle& operator-= (double x)
        {
#if DEBUG_INFO_FUNCTION_TYPE
            std::cout << "[INFO]: Circle::Circle& operator-= (double x)\n";
#endif
            m_rad -= x;
            return *this;
        }

        Circle operator- () const
        {
#if DEBUG_INFO_FUNCTION_TYPE
            std::cout << "[INFO]: Circle::Circle operator- ()\n";
#endif
            return Circle { -m_x, -m_y, -m_rad };
        }

        void print (std::ostream& os = std::cout) const
        {
#if DEBUG_INFO_FUNCTION_TYPE
            std::cout << "[INFO]: void Circle::print (std::ostream)\n";
            // std::cout << std::setw(DEBUG_INDENT) << ""; // EXPERIMENTAL
#endif
            os << std::format("({},{},{})", m_x, m_y, m_rad);
        }

        friend Circle operator+ (double, const Circle&);
        friend Circle operator- (double, const Circle&);
        friend Circle operator~ (const Circle&);


    private:
        double m_x;
        double m_y;
        double m_rad;
};

// XXX presision if Backend is a float type
// XXX variadic args?
void print (const Circle& obj, std::ostream& os = std::cout, char end = '\n')
{
#if DEBUG_INFO_FUNCTION_TYPE
    std::cout << "[INFO]: void print (const Circle&, std::ostream, char)\n";
#endif
    obj.print(os);
    os << end;
}

inline Circle operator+ (const Circle& lhs, double rhs)
{
#if DEBUG_INFO_FUNCTION_TYPE
    std::cout << "[INFO]: Circle operator+ (const Circle&, double)\n";
#endif
    Circle result {lhs};
    result += rhs;

    return result;
}

inline Circle operator- (const Circle& lhs, double rhs)
{
#if DEBUG_INFO_FUNCTION_TYPE
    std::cout << "[INFO]: Circle operator- (const Circle&, double)\n";
#endif
    Circle result {lhs};
    result -= rhs;

    return result;
}

inline Circle operator+ (double lhs, const Circle& rhs)
{
#if DEBUG_INFO_FUNCTION_TYPE
    std::cout << "[INFO]: friend Circle operator+ (const Circle&, double)\n";
#endif
    return rhs + lhs;
}

inline Circle operator- (double lhs, const Circle& rhs)
{
#if DEBUG_INFO_FUNCTION_TYPE
    std::cout << "[INFO]: friend Circle operator+ (const Circle&, double)\n";
#endif
    // XXX should I follow that (rhs-lsh) = (rhs + (-lhs))?
    return rhs - lhs;
}

inline Circle operator~ (const Circle& rhs)
{
#if DEBUG_INFO_FUNCTION_TYPE
    std::cout << "[INFO]: friend Circle operator~ (const Circle&)\n";
#endif
    return Circle { rhs.m_x, -rhs.m_y, rhs.m_rad };
}


int main (void)
{
    using std::cout;
    using std::format;

    Circle c1 (1, 2, 3);
    print(c1);

    // Circle& operator+= (double)
    c1 += 8;
    print(c1);

    // Circle& operator-= (double)
    c1 -= 4;
    print(c1);

    Circle c2 {4, 5, 6};
    print(c2);

    // Circle operator- () const
    print(-c2);

    // Circle operator+ (const Circle&, double)
    print(-c2 + 26);

    // friend Circle operator+ (double, const Circle&)
    print(26 + -c2);

    // friend Circle operator~ (const Circle&);
    print(~c2);


    // Circle c1 = Circle();
    // c1 + 20;
    //
    // Circle c2 = Circle(c1);
    // Circle c3 { 9, 24, 7 };
    // Circle c4 { Circle(23,13,31) };

    return 0;
}
