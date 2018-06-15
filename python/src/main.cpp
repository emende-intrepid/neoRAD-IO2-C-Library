#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <neoRAD-IO2.h>

#include <tuple>
#include <array>
#include <vector>
#include <algorithm>
#include <sstream>

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
    
    // Fifo.h
    py::class_<fifo_t>(m, "fifo_t")
        .def(py::init([]() { return new fifo_t{0}; }))
        .def_readonly("ptr", &fifo_t::ptr)
        .def_readonly("maxSz", &fifo_t::maxSz)
        .def_readonly("in", &fifo_t::in)
        .def_readonly("out", &fifo_t::out)
        .def_readonly("numItems", &fifo_t::numItems)
#ifdef TRACK_USAGE
        .def_readonly("numItems_max", &fifo_t::numItems_max)
#endif
        ;
    
    
    // neoRAD-IO2.h
    /*
    py::class_<neoRADIO2_DeviceInfo>(m, "neoRADIO2_DeviceInfo")
        .def(py::init([]() { return new neoRADIO2_DeviceInfo{0}; }))
        .def_readwrite("usbDevice", &neoRADIO2_DeviceInfo::usbDevice)
        //.def_readwrite("ChainList", &neoRADIO2_DeviceInfo::ChainList)
        .def_readwrite("State", &neoRADIO2_DeviceInfo::State)
        .def_readwrite("maxID", &neoRADIO2_DeviceInfo::maxID)
        .def_readwrite("Timeus", &neoRADIO2_DeviceInfo::Timeus)
        .def_readwrite("online", &neoRADIO2_DeviceInfo::online)
        .def_readwrite("rxbuffer", &neoRADIO2_DeviceInfo::rxbuffer)
        .def_readwrite("txbuffer", &neoRADIO2_DeviceInfo::txbuffer)
        .def_readwrite("rxfifo", &neoRADIO2_DeviceInfo::rxfifo)
        .def_readwrite("txfifo", &neoRADIO2_DeviceInfo::txfifo)
        //.def_readwrite("rxDataBuffer", &neoRADIO2_DeviceInfo::rxDataBuffer)
        //.def_readwrite("rxHeaderBuffer", &neoRADIO2_DeviceInfo::rxHeaderBuffer)
        .def_readwrite("rxDataCount", &neoRADIO2_DeviceInfo::rxDataCount)
        .def_readwrite("rxHeaderCount", &neoRADIO2_DeviceInfo::rxHeaderCount);
    */
    
    py::class_<neoRADIO2_USBDevice>(m, "USBDevice")
        .def(py::init([]() { return new neoRADIO2_USBDevice{0}; }))
        .def_readonly("ft260Device", &neoRADIO2_USBDevice::ft260Device)
        .def_readonly("serial", &neoRADIO2_USBDevice::serial)
        .def("__repr__", 
            [](const neoRADIO2_USBDevice& ud) 
            {
                std::wstringstream ss;
                ss << L"<neoradio2.USBDevice object " << ud.ft260Device.SerialNumber << L">";
                return ss.str();
            }
        );
        
    py::class_<ft260_device>(m, "ft260_device")
        .def(py::init([]() { return new ft260_device{0}; }))
        .def_readonly("SerialNumber", &ft260_device::SerialNumber);    

    m.def("FindDevices",
        []() {
            const unsigned int size = 8;
            neoRADIO2_USBDevice info[size];
            std::vector<neoRADIO2_USBDevice> info_vector;
            int found = 0;
            found = neoRADIO2FindDevices(info, size);
            const unsigned int found_size = found >= 0 ? found : 0;
            info_vector.reserve(found_size);
            std::copy(std::begin(info), std::begin(info)+found_size, std::back_inserter(info_vector));
            return std::make_tuple(found, info_vector);
        }, R"pbdoc(
        Find neoRADIO2 Devices.
        
        Returns a tuple (device_count, [list of USBDevice])
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
