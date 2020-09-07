// Copyright 2019 Max Planck Gesellschaft and New York University
// Authors : Vincent Berenz, Maximilien Naveau

#include "real_time_tools/frequency_manager.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(real_time_tools_cpp_bindings, m)
{
    m.doc() = "real-time wrappers of basic OS tools, like timer and threads.";

    pybind11::class_<real_time_tools::FrequencyManager>(m, "FrequencyManager")
        .def(pybind11::init<double>())
        .def("wait", &real_time_tools::FrequencyManager::wait);
}
