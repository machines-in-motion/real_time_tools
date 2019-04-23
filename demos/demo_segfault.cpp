/**
 * \file demo_finger.cpp
 * \brief a demo illustrating how to use the RealFinger class
 * \author Manuel Wuthrich
 * \date 2019
 *
 * This file uses the RealFinger class in a small demo.
 */

#include <iostream>
#include <tuple>
#include <cmath>
#include <math.h>
#include <Eigen/Eigen>
#include <limits>

#include "real_time_tools/realtime_thread_creation.hpp"


union myUnion {
    double dValue;
    uint64_t iValue;
};

double f_0(double t)
{
    double dummy1;
    double dummy2 = 1.0;

    // this strange way of assigning a double is just because i needed
    // to print the exact value of a double, which is best done as an integer
    myUnion the_number_which_causes_segfault;
    the_number_which_causes_segfault.iValue = 4619552210890228616;
    double x = the_number_which_causes_segfault.dValue;

    std::cout << "calling pow with " << x << std::endl;
    std::pow(x, 3.01);
    std::cout << "done calling pow "  << std::endl;

    return 0;
}

class DummyClass
{
public:
    DummyClass()
    { }

    double dummy3_;
    double dummy4_;
    double dummy5_;
    double dummy6_;

};

class SomeClass: public DummyClass
{
public:
    typedef Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::AutoAlign, 10, 1> Vector;

    SomeClass(){ }

    double f_1(double dummy) const
    {
        return f_0(0);
    }


    void f_3(const double& dummy1,
             const double& dummy2) const
    {
        double dummy3;

        f_2(0, 0, 0);
    }

    void f_2(const double& dummy7,
             const double& dummy8,
             const double& dummy9) const
    {
        char dummy1[96*3 + 8];

        double output = f_1(0);

        char dummy2[96];

        Vector dummy6; //96 byte
    }

private:
    double dummy1_;
    double dummy2_;
};

void f_4(const double& dummy1,
         const double& dummy2,
         const double& dummy3,
         const double& dummy4,
         const double& dummy5,
         const double& dummy6)
{
    SomeClass instance;
    instance.f_3(0., 0.);

    double dummy[4];
}

void f_5(const double& dummy1,
         const double& dummy2,
         const double& dummy3,
         const double& dummy4,
         const double& dummy5,
         const double& dummy6)
{
    double dummy;
    f_4(0, 0, 0, 0, 0, 0);
}



static void* thread_function(void*)
{
    double dummy0[22];

    Eigen::Vector3d dummy16  =
            0.2 * (Eigen::Vector3d::Ones()-Eigen::Vector3d::Ones()) -
            0.001 * Eigen::Vector3d::Ones();

    char dummy[10+12*8];

    size_t need_to_be_zero = 0;

    std::cout << "seg fault happens in here:" << std::endl;
    f_5(0, 0, 0, 0, 0, 0);
    std::cout << "we never get here" << std::endl;

    return 0;
}

int main(int argc, char **argv)
{
    real_time_tools::RealTimeThread thread;
    real_time_tools::create_realtime_thread(thread, &thread_function);
    real_time_tools::join_thread(thread);
    return 0;
}
