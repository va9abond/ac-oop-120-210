#include <iostream>


#define CST_BEGIN namespace cst {
#define CST_END }


CST_BEGIN
    struct exception : public std::exception {
    public:
        exception (const char* init);
        exception (const exception& other);
        ~exception();
        virtual void print();


    protected:
        const char *msg;
    };

CST_END


class Matrix {
public:
    using value_type = double;
    using size_type  = size_t;

public:
    Matrix (size_type, size_type);
    Matrix (const Matrix& other);
    Matrix dummy_matrix_ctor (int, int);
    ~Matrix();

    double& operator() (size_type, size_type);
    void print();

    friend std::ostream& operator << (std::ostream&, Matrix&);
    friend std::istream& operator << (std::istream&, Matrix&);


protected:
    value_type **ptr;
    size_type rows;
    size_type cols;
};

std::ostream& operator << (std::ostream&, Matrix&);
std::istream& operator << (std::istream&, Matrix&);

std::ostream& ostream_formater (std::ostream&);


int main (void)
{
    // test 1
    try {
        Matrix E = dummy_matrix_ctor(-2, 0);
    } catch (cst::exception&) {

    }

    // test 2
    Matrix A(2, 3); std::cin >> A;

    // test 3
    if (std::ofstream file("out.txt"); file.is_open()) {
        file << A;
        file.close();
    }

    // test 4
    if (std::ifstream file_input("out.txt"); file_input.is_open()) {
        file_input >> A;
        file_input.close();
    }

    // test 5
    std::cout << A;


    return 0;
}
