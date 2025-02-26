#include <iostream>
#include <format>


struct A1 {
    public:
        A1 (int m) : a1(m)
        {
            std::cout << std::format("A1 ctor: A1({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from A1, A1::a1 = {};\n", a1);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from A1: A1::a1 = {};\n", a1);
        }

    protected:
        int a1;
};

struct A2 {
    public:
        A2 (int m) : a2(m)
        {
            std::cout << std::format("A2 ctor: A2({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from A2, A2::a2 = {};\n", a2);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from A2: A2::a2 = {};\n", a2);
        }

    protected:
        int a2;
};

struct A3 {
    public:
        A3 (int m) : a3(m)
        {
            std::cout << std::format("A3 ctor: A3({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from A3, A3::a3 = {};\n", a3);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from A3: A3::a3 = {};\n", a3);
        }

    protected:
        int a3;
};

struct B1 : public A1, virtual public A3 {
    public:
        B1 (int m) : A3(m), A1(m), b1(m)
        {
            std::cout << std::format("B1 ctor: B1({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from B1, B1::b1 = {};\n", b1);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from B1: A1::a1 = {}, A3::a3 = {}, B1::b1 = {};\n", A1::a1, A3::a3, b1);
        }

    protected:
        int b1;
};

struct B2 : public A2, virtual public A3 {
    public:
        B2 (int m) : A3(m), A2(m), b2(m)
        {
            std::cout << std::format("B2 ctor: B2({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from B2, B2::b2 = {};\n", b2);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from B2: A2::a2 = {}, A3::a3 = {}, B2::b2 = {};\n", A2::a2, A3::a3, b2);
        }

    protected:
        int b2;
};

struct C1 : public B1, public B2 {
    public:
        C1 (int m) : A3(-1), B1(m), B2(m), c1(m)
        {
            std::cout << std::format("С1 ctor: С1({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from C1, C1::c1 = {};\n", c1);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << "Available variables from C1:\n"
                << std::format("\tB1::A1::a1 = {},\n", B1::A1::a1)
                << std::format("\tB2::A2::a2 = {},\n", B2::A2::a2)
                << std::format("\tB1::A3::a3 = B2::A3::a3 {}, {},\n", B1::A3::a3, B2::A3::a3)
                << std::format("\tB1::b1 = {},\n", B1::b1)
                << std::format("\tB2::b2 = {},\n", B2::b2)
                << std::format("\tC1::c1 = {};\n", c1);
        }

    protected:
        int c1;
};

struct C2 : public B1, public B2 {
    public:
        C2 (int m) : A3(-2), B1(m), B2(m), c2(m)
        {
            std::cout << std::format("С2 ctor: С2({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from C2, C2::c2 = {};\n", c2);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << "Available variables from C2:\n"
                << std::format("\tB1::A1::a1 = {},\n", B1::A1::a1)
                << std::format("\tB2::A2::a2 = {},\n", B2::A2::a2)
                << std::format("\tB1::A3::a3 = B2::A3::a3 {}, {},\n", B1::A3::a3, B2::A3::a3)
                << std::format("\tB1::b1 = {},\n", B1::b1)
                << std::format("\tB2::b2 = {},\n", B2::b2)
                << std::format("\tC2::c2 = {};\n", c2);
        }

    protected:
        int c2;
};

struct C3 : public B1, public B2 {
    public:
        C3 (int m) : A3(-3), B1(m), B2(m), c3(m)
        {
            std::cout << std::format("С3 ctor: С3({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from C3, C3::c3 = {};\n", c3);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << "Available variables from C3:\n"
                << std::format("\tB1::A1::a1 = {},\n", B1::A1::a1)
                << std::format("\tB2::A2::a2 = {},\n", B2::A2::a2)
                << std::format("\tB1::A3::a3 = B2::A3::a3 {}, {},\n", B1::A3::a3, B2::A3::a3)
                << std::format("\tB1::b1 = {},\n", B1::b1)
                << std::format("\tB2::b2 = {},\n", B2::b2)
                << std::format("\tC3::c3 = {};\n", c3);
        }

    protected:
        int c3;
};

void endl (std::ostream& os = std::cout)
{
    os << '\n';
}


int main (void)
{
    A1 a1(1); a1.printn(); endl();
    A2 a2(2); a2.printn(); endl();
    A3 a3(3); a3.printn(); endl();

    B1 b1(2); b1.printn(); endl();
    B2 b2(2); b2.printn(); endl();

    C1 c1(3); c1.printn(); endl();
    C2 c2(4); c2.printn(); endl();
    C3 c3(4); c3.printn(); endl();

    A1 *base_ptr = &a1;
    base_ptr->printn();
    base_ptr->show();
    endl();

    base_ptr = &b1;
    base_ptr->printn();
    base_ptr->show();
    endl();

    base_ptr = &c1;
    base_ptr->printn();
    base_ptr->show();
    endl();

    base_ptr = &c2;
    base_ptr->printn();
    base_ptr->show();
    endl();

    return 0;
}
