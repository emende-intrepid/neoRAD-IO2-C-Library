#include <pybind11/pybind11.h>

#include <neoRAD-IO2.h>

namespace py = pybind11;

PYBIND11_MODULE(neoradio2, m) {
    m.doc() = R"pbdoc(
        neoRADIO2 Python Library
        -----------------------

        .. currentmodule:: neoradio2

        .. autosummary::
           :toctree: _generate

            FindDevices
            CloseDevice
            ConnectDevice
            ProcessIncommingData
            SetDeviceSettings
            SetOnline
            GetReportRate
            
    )pbdoc";
    
    py::class_<neoRADIO2_DeviceInfo>(m, "DeviceInfo")
        .def_readwrite("ft260Device", &neoRADIO2_DeviceInfo::ft260Device)
        //.def_readwrite("ChainList", &neoRADIO2_DeviceInfo::ChainList)
        .def_readwrite("State", &neoRADIO2_DeviceInfo::State)
        .def_readwrite("maxID", &neoRADIO2_DeviceInfo::maxID)
        .def_readwrite("Timeus", &neoRADIO2_DeviceInfo::Timeus)
        .def_readwrite("online", &neoRADIO2_DeviceInfo::online)
        //.def_readwrite("rxbuffer", &neoRADIO2_DeviceInfo::rxbuffer)
        //.def_readwrite("txbuffer", &neoRADIO2_DeviceInfo::txbuffer)
        .def_readwrite("rxfifo", &neoRADIO2_DeviceInfo::rxfifo)
        .def_readwrite("txfifo", &neoRADIO2_DeviceInfo::txfifo)
        //.def_readwrite("rxDataBuffer", &neoRADIO2_DeviceInfo::rxDataBuffer)
        //.def_readwrite("rxHeaderBuffer", &neoRADIO2_DeviceInfo::rxHeaderBuffer)
        .def_readwrite("rxDataCount", &neoRADIO2_DeviceInfo::rxDataCount)
        .def_readwrite("rxHeaderCount", &neoRADIO2_DeviceInfo::rxHeaderCount);
    
    m.def("FindDevices", &neoRADIO2FindDevices, R"pbdoc(
        Find neoRADIO2 Devices.
        
        Returns a tuple of DeviceInfo objects
    )pbdoc");
    
    m.def("CloseDevice", &neoRADIO2CloseDevice, R"pbdoc(
        TODO
        
        TODO
    )pbdoc");
    
    m.def("ConnectDevice", &neoRADIO2ConnectDevice, R"pbdoc(
        TODO
        
        TODO
    )pbdoc");
    
    m.def("ProcessIncommingData", &neoRADIO2ProcessIncommingData, R"pbdoc(
        TODO
        
        TODO
    )pbdoc");
    
    m.def("SetDeviceSettings", &neoRADIO2SetDeviceSettings, R"pbdoc(
        TODO
        
        TODO
    )pbdoc");
    
    m.def("SetOnline", &neoRADIO2SetOnline, R"pbdoc(
        TODO
        
        TODO
    )pbdoc");
    
    m.def("GetReportRate", &neoRADIO2GetReportRate, R"pbdoc(
        TODO
        
        TODO
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
