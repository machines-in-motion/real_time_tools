// Copyright 2019 Max Planck Gesellschaft and New York University
// Authors : Vincent Berenz, Maximilien Naveau

#include "real_time_tools/frequency_manager.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace real_time_tools;

PYBIND11_MODULE(real_time_tools,m){

  pybind11::class_<FrequencyManager>(m,"FrequencyManager")
    .def(pybind11::init<double>())
    .def("wait",&FrequencyManager::wait);

}



  
