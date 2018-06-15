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
    
    // Fifo.h =================================================================
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
    
    // neoRADIO2_frames.h =====================================================
    
    py::enum_<neoRADIO2_deviceTypes>(m, "deviceTypes")
        .value("DEVTYPE_TC", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_TC)
        .value("DEVTYPE_DIO", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_DIO)
        .value("DEVTYPE_PWRRLY", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_PWRRLY)
        .value("DEVTYPE_AIN", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_AIN)
        .value("DEVTYPE_CANHUB", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_CANHUB)
        .value("DEVTYPE_HOST", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_HOST);
    
    py::class_<neoRADIO2frame_identifyResponse>(m, "identifyResponse")
        .def(py::init([]() { return new neoRADIO2frame_identifyResponse{0}; }))
        .def_readonly("serialNumber", &neoRADIO2frame_identifyResponse::serialNumber)
        .def_readonly("manufacture_year", &neoRADIO2frame_identifyResponse::manufacture_year)
        .def_readonly("manufacture_month", &neoRADIO2frame_identifyResponse::manufacture_month)
        .def_readonly("manufacture_day", &neoRADIO2frame_identifyResponse::manufacture_day)
        .def_readonly("deviceType", &neoRADIO2frame_identifyResponse::deviceType)
        .def_readonly("deviceID", &neoRADIO2frame_identifyResponse::deviceID)
        .def_readonly("firmwareVersion_major", &neoRADIO2frame_identifyResponse::firmwareVersion_major)
        .def_readonly("firmwareVersion_minor", &neoRADIO2frame_identifyResponse::firmwareVersion_minor)
        .def_readonly("hardware_revMinor", &neoRADIO2frame_identifyResponse::hardware_revMinor)
        .def_readonly("hardware_revMajor", &neoRADIO2frame_identifyResponse::hardware_revMajor)
        .def_readonly("current_state", &neoRADIO2frame_identifyResponse::current_state)
        .def_readonly("checksum", &neoRADIO2frame_identifyResponse::checksum);
    
    py::class_<neoRADIO2frame_id>(m, "frame_id")
        .def(py::init([]() { return new neoRADIO2frame_id{0}; }));
        //.def_readonly("chip", [](const neoRADIO2frame_id& fi) { return fi.chip; }) // TODO
        //.def_readonly("device", [](const neoRADIO2frame_id& fi) { return fi.device; }) // TODO
        //.def_readonly("byte", &neoRADIO2frame_id::byte); // TODO
        
    py::class_<neoRADIO2_destination>(m, "destination")
        .def(py::init([]() { return new neoRADIO2_destination{0}; }))
        .def_readonly("device", &neoRADIO2_destination::device)
        .def_readonly("chip", &neoRADIO2_destination::chip);
    
    py::class_<neoRADIO2frame_header>(m, "frame_header")
        .def(py::init([]() { return new neoRADIO2frame_header{0}; }))
        .def_readonly("headerAA", &neoRADIO2frame_header::headerAA)
        .def_readonly("command", &neoRADIO2frame_header::command)
        .def_readonly("dest", &neoRADIO2frame_header::dest)
        .def_readonly("len", &neoRADIO2frame_header::len)
        .def_readonly("checksum", &neoRADIO2frame_header::checksum);

    py::class_<neoRADIO2frame_DeviceReportHeader>(m, "frame_DeviceReportHeader")
        .def(py::init([]() { return new neoRADIO2frame_DeviceReportHeader{0}; }))
        .def_readonly("header55", &neoRADIO2frame_DeviceReportHeader::header55)
        .def_readonly("id", &neoRADIO2frame_DeviceReportHeader::id)
        .def_readonly("len", &neoRADIO2frame_DeviceReportHeader::len);
        
    py::class_<neoRADIO2_ReceiveData>(m, "ReceiveData")
        .def(py::init([]() { return new neoRADIO2_ReceiveData{0}; }))
        .def_readonly("header", &neoRADIO2_ReceiveData::header)
        .def_readonly("data", &neoRADIO2_ReceiveData::data);
        
    py::class_<neoRADIO2frame_identify>(m, "frame_identify")
        .def(py::init([]() { return new neoRADIO2frame_identify{0}; }))
        .def_readonly("device_type", &neoRADIO2frame_identify::device_type)
        .def_readonly("id", &neoRADIO2frame_identify::id);
    
    py::enum_<_neoRADIO2frame_commands>(m, "frame_commands")
        .value("COMMAND_IDENTIFY", _neoRADIO2frame_commands::NEORADIO2_COMMAND_IDENTIFY)
        .value("COMMAND_WRITEDATA", _neoRADIO2frame_commands::NEORADIO2_COMMAND_WRITEDATA)
        .value("COMMAND_READDATA", _neoRADIO2frame_commands::NEORADIO2_COMMAND_READDATA)
        .value("COMMAND_SETSETTINGS", _neoRADIO2frame_commands::NEORADIO2_COMMAND_SETSETTINGS)
        .value("COMMAND_READSETTINGS", _neoRADIO2frame_commands::NEORADIO2_COMMAND_READSETTINGS)
        .value("COMMAND_JUMPTOAPP", _neoRADIO2frame_commands::NEORADIO2_COMMAND_JUMPTOAPP)
        .value("COMMAND_ENTERBOOT", _neoRADIO2frame_commands::NEORADIO2_COMMAND_ENTERBOOT);
        
    py::enum_<neoRADIO2frame_BLommands>(m, "frame_BLommands")
        .value("BLCOMMAND_ERASE", neoRADIO2frame_BLommands::NEORADIO2_BLCOMMAND_ERASE)
        .value("BLCOMMAND_WRITE", neoRADIO2frame_BLommands::NEORADIO2_BLCOMMAND_WRITE)
        .value("BLCOMMAND_FLASH", neoRADIO2frame_BLommands::NEORADIO2_BLCOMMAND_FLASH)
        .value("BLCOMMAND_VERIFY", neoRADIO2frame_BLommands::NEORADIO2_BLCOMMAND_VERIFY)
        .value("BLCOMMAND_JUMP_APP", neoRADIO2frame_BLommands::NEORADIO2_BLCOMMAND_JUMP_APP);

    py::class_<neoRADIO2AOUTframe_data>(m, "AOUTframe_data")
        .def(py::init([]() { return new neoRADIO2AOUTframe_data{0}; }))
        .def_readonly("DAC11", &neoRADIO2AOUTframe_data::DAC11)
        .def_readonly("DAC12", &neoRADIO2AOUTframe_data::DAC12)
        .def_readonly("DAC23", &neoRADIO2AOUTframe_data::DAC23);
    
    py::class_<neoRADIO2TCframe_deviceSettings>(m, "TCframe_deviceSettings")
        .def(py::init([]() { return new neoRADIO2TCframe_deviceSettings{0}; }))
        .def_readonly("reportRate", &neoRADIO2TCframe_deviceSettings::reportRate)
        .def_readonly("enables", &neoRADIO2TCframe_deviceSettings::enables);
        
    py::class_<neoRADIO2AOUTframe_deviceSettings>(m, "AOUTframe_deviceSettings")
        .def(py::init([]() { return new neoRADIO2AOUTframe_deviceSettings{0}; }))
        .def_readonly("reportRate", &neoRADIO2AOUTframe_deviceSettings::reportRate)
        .def_readonly("DefaultValue1", &neoRADIO2AOUTframe_deviceSettings::DefaultValue1)
        .def_readonly("DefaultValue2", &neoRADIO2AOUTframe_deviceSettings::DefaultValue2)
        .def_readonly("DefaultValue3", &neoRADIO2AOUTframe_deviceSettings::DefaultValue3)
        .def_readonly("enables", &neoRADIO2AOUTframe_deviceSettings::enables);

    py::class_<neoRADIO2AINframe_deviceSettings>(m, "AINframe_deviceSettings")
        .def(py::init([]() { return new neoRADIO2AINframe_deviceSettings{0}; }))
        .def_readonly("reportRate", &neoRADIO2AINframe_deviceSettings::reportRate)
        .def_readonly("InputVoltageRange", &neoRADIO2AINframe_deviceSettings::InputVoltageRange)
        .def_readonly("enables", &neoRADIO2AINframe_deviceSettings::enables);
        
    py::class_<neoRADIO2PWRRLYframe_deviceSettings>(m, "PWRRLYframe_deviceSettings")
        .def(py::init([]() { return new neoRADIO2PWRRLYframe_deviceSettings{0}; }))
        .def_readonly("reportRate", &neoRADIO2PWRRLYframe_deviceSettings::reportRate)
        .def_readonly("InitialRelayState", &neoRADIO2PWRRLYframe_deviceSettings::InitialRelayState);
        
    py::class_<neoRADIO2frame_deviceSettings>(m, "frame_deviceSettings")
        .def(py::init([]() { return new neoRADIO2frame_deviceSettings{0}; }))
        .def_readonly("device", &neoRADIO2frame_deviceSettings::device);

    py::enum_<neoRADIO2TC_commands>(m, "TC_commands")
        .value("TC_READDATA", neoRADIO2TC_commands::NEORADIO2TC_READDATA)
        .value("TC_READ_CALDATA", neoRADIO2TC_commands::NEORADIO2TC_READ_CALDATA)
        .value("TC_SET_CALDATA", neoRADIO2TC_commands::NEORADIO2TC_SET_CALDATA);
    
    py::enum_<neoRADIO2AOUT_commands>(m, "AOUT_commands")
        .value("AOUT_READ_DAC", neoRADIO2AOUT_commands::NEORADIO2AOUT_READ_DAC)
        .value("AOUT_SET_DAC", neoRADIO2AOUT_commands::NEORADIO2AOUT_SET_DAC)
        .value("AOUT_DISABLE", neoRADIO2AOUT_commands::NEORADIO2AOUT_DISABLE)
        .value("AOUT_READ_CALDATA", neoRADIO2AOUT_commands::NEORADIO2AOUT_READ_CALDATA)
        .value("AOUT_SET_CALDATA", neoRADIO2AOUT_commands::NEORADIO2AOUT_SET_CALDATA);
    
    py::enum_<neoRADIO2PWRRLY_commands>(m, "PWRRLY_commands")
        .value("PWRRLY_SET_RLY", neoRADIO2PWRRLY_commands::NEORADIO2PWRRLY_SET_RLY)
        .value("PWRRLY_READ_RLY", neoRADIO2PWRRLY_commands::NEORADIO2PWRRLY_READ_RLY)
        .value("PWRRLY_DISABLE", neoRADIO2PWRRLY_commands::NEORADIO2PWRRLY_DISABLE);
    
    py::enum_<neoRADIO2states>(m, "states")
        .value("STATE_RUNNING", neoRADIO2states::NEORADIO2STATE_RUNNING)
        .value("STATE_INBOOTLOADER", neoRADIO2states::NEORADIO2STATE_INBOOTLOADER);
    

    // neoRAD-IO2.h ===========================================================

    py::class_<neoRADIO2_DeviceInfo>(m, "DeviceInfo")
        .def(py::init([]() { return new neoRADIO2_DeviceInfo{0}; }))
        .def_readwrite("usbDevice", &neoRADIO2_DeviceInfo::usbDevice)
        //.def_readwrite("ChainList", &neoRADIO2_DeviceInfo::ChainList) // TODO
        .def_readonly("State", &neoRADIO2_DeviceInfo::State)
        .def_readonly("maxID", &neoRADIO2_DeviceInfo::maxID)
        .def_readonly("Timeus", &neoRADIO2_DeviceInfo::Timeus)
        .def_readonly("online", &neoRADIO2_DeviceInfo::online)
        .def_readonly("rxbuffer", &neoRADIO2_DeviceInfo::rxbuffer)
        .def_readonly("txbuffer", &neoRADIO2_DeviceInfo::txbuffer)
        .def_readonly("rxfifo", &neoRADIO2_DeviceInfo::rxfifo)
        .def_readonly("txfifo", &neoRADIO2_DeviceInfo::txfifo)
        .def_readonly("rxDataBuffer", &neoRADIO2_DeviceInfo::rxDataBuffer)
        .def_readonly("rxHeaderBuffer", &neoRADIO2_DeviceInfo::rxHeaderBuffer)
        .def_readonly("rxDataCount", &neoRADIO2_DeviceInfo::rxDataCount)
        .def_readonly("rxHeaderCount", &neoRADIO2_DeviceInfo::rxHeaderCount);

    py::class_<neoRADIO2_ChipInfo>(m, "ChipInfo")
        .def(py::init([]() { return new neoRADIO2_ChipInfo{0}; }))
        .def_readonly("serialNumber", &neoRADIO2_ChipInfo::serialNumber)
        .def_readonly("manufacture_year", &neoRADIO2_ChipInfo::manufacture_year)
        .def_readonly("manufacture_day", &neoRADIO2_ChipInfo::manufacture_day)
        .def_readonly("manufacture_month", &neoRADIO2_ChipInfo::manufacture_month)
        .def_readonly("deviceID", &neoRADIO2_ChipInfo::deviceID)
        .def_readonly("deviceType", &neoRADIO2_ChipInfo::deviceType)
        .def_readonly("firmwareVersion_major", &neoRADIO2_ChipInfo::firmwareVersion_major)
        .def_readonly("firmwareVersion_minor", &neoRADIO2_ChipInfo::firmwareVersion_minor)
        .def_readonly("hardwareRev_major", &neoRADIO2_ChipInfo::hardwareRev_major)
        .def_readonly("hardwareRev_minor", &neoRADIO2_ChipInfo::hardwareRev_minor)
        .def_readonly("status", &neoRADIO2_ChipInfo::status)
        .def_readonly("settingsRead", &neoRADIO2_ChipInfo::settingsRead)
        .def_readonly("lastReadTimeus", &neoRADIO2_ChipInfo::lastReadTimeus);
        //.def_readonly("settings", &neoRADIO2_ChipInfo::settings); // TODO

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

    py::enum_<neoRADIO2_RunStates>(m, "RunStates")
        .value("Disconnected", neoRADIO2_RunStates::neoRADIO2state_Disconnected)
        .value("ConnectedWaitIdentHeader", neoRADIO2_RunStates::neoRADIO2state_ConnectedWaitIdentHeader)
        .value("ConnectedWaitIdentResponse", neoRADIO2_RunStates::neoRADIO2state_ConnectedWaitIdentResponse)
        .value("ConnectedWaitSettings", neoRADIO2_RunStates::neoRADIO2state_ConnectedWaitSettings)
        .value("Connected", neoRADIO2_RunStates::neoRADIO2state_Connected)
        .value("Disconnected", neoRADIO2_RunStates::neoRADIO2state_Disconnected);

    // Functions ==============================================================
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
