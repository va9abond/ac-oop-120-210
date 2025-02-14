#include <iostream>
#include <format>

struct Circle {
    public:
        Circle (double x = 0, double y = 0, double rad = 0) // = default
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

        friend Circle operator+ (double, const Circle&);
        friend Circle operator- (double, const Circle&);
        friend Circle operator~ (const Circle&);
        friend void print (const Circle&, char end, std::ostream&);


    private:
        double m_x;
        double m_y;
        double m_rad;
};

void print (
    const Circle& obj, char end = '\n', std::ostream& os = std::cout
) {
    os << std::format("({},{},{})", obj.m_x, obj.m_y, obj.m_rad) << end;
}

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

inline Circle operator+ (double lhs, const Circle& rhs)
{
    return rhs + lhs;
}

inline Circle operator- (double lhs, const Circle& rhs)
{
    return rhs + lhs;
}

inline Circle operator~ (const Circle& rhs)
{
    return Circle { rhs.m_x, -rhs.m_y, rhs.m_rad };
}


int main (void)
{
    using std::cout;
    using std::format;

    cout << "MAIN.CPP: STDOUT START vvv\n";

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

    cout << "MAIN.CPP: STDOUT END ^^^\n";
    return 0;
}
