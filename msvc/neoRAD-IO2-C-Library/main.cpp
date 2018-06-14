#include <iostream>
#include <string>
#include <sstream>

#include "neoRAD-IO2.h"


std::string get_friendly_name(neoRADIO2_DeviceInfo& info)
{
	std::stringstream ss;
	ss << "neoRADIO2 " << info.ft260Device.SerialNumber;
	return ss.str();
}


int main(int argc, char* argv[])
{
	std::cout << "neoRAD-IO2 Example Application\n";

	neoRADIO2_DeviceInfo* device_infos = new neoRADIO2_DeviceInfo[255]{0};
	int result = 0;
    // int neoRADIO2FindDevices(neoRADIO2_DeviceInfo * devInfo);
    if ((result = neoRADIO2FindDevices(device_infos)) != 0)
    {
        std::cerr << "neoRADIO2FindDevices() failed with error code: " << result << "\n";
		delete[] device_infos;
        return 1;
    }
	// Lets read the first 10, we have no way to know how many we have.
	std::cout << "Displaying first 10 potential devices:\n";
	for (int i=0; i < 10; ++i)
	{
		std::cout << i << ".)\t" << get_friendly_name(device_infos[i]) << "\n";
	}
	delete[] device_infos;
    return 0;
}