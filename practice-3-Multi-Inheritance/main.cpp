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

struct B1 : public A1 {
    public:
        B1 (int m) : A1(m), b1(m)
        {
            std::cout << std::format("B1 ctor: B1({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from B1, B1::b1 = {};\n", b1);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from B1: A1::a1 = {}, B1::b1 = {};\n", A1::a1, b1);
        }

    protected:
        int b1;
};

struct C1 : public B1 {
    public:
        C1 (int m) : B1(m), c1(m)
        {
            std::cout << std::format("С1 ctor: С1({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from C1, C1::c1 = {};\n", c1);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from C1: A1::a1 = {}, B1::b1 = {}, C1::c1 = {};\n", A1::a1, B1::b1, c1);
        }

    protected:
        int c1;
};

struct C2 : public B1 {
    public:
        C2 (int m) : B1(m), c2(m)
        {
            std::cout << std::format("C2 ctor: C2({})\n", m);
        }

        virtual void printn (std::ostream& os = std::cout)
        {
            os << std::format("'printn' from C2, C2::c2 = {};\n", c2);
        }

        virtual void show (std::ostream& os = std::cout)
        {
            os << std::format("Available variables from C2: A1::a1 = {}, B1::b1 = {}, C2::c2 = {};\n", A1::a1, B1::b1, c2);
        }

    protected:
        int c2;
};

void endl (std::ostream& os = std::cout)
{
    os << '\n';
}


int main (void)
{
    A1 a1(1); a1.printn(); endl();
    B1 b1(2); b1.printn(); endl();
    C1 c1(3); c1.printn(); endl();
    C2 c2(4); c2.printn(); endl();

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
