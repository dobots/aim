#include <WriteModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 1000;

#define SHOW_YOU_ALSO_CAN_CONNECT_DIRECTLY 0

/***********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************/

WriteModuleExt::WriteModuleExt() {
}

WriteModuleExt::~WriteModuleExt() {
	std::cout << "Deallocate object and close socket to name server" << endl;
}

/**
 * The write module
 */
void WriteModuleExt::Tick() {
	std::cout << '[' << getpid() << "] Tick " << lifetime << endl;
	WriteModule::Tick();
#if SHOW_YOU_ALSO_CAN_CONNECT_DIRECTLY == 1
	if (lifetime == 1000) {
		Connect("/writemodule0/output", "/readmodule0/input");
	}
#endif
	if (!(lifetime % 7)) { // every 7 ticks send a value
		int value = 666;
		if (writeOutput(value)) {
			std::cout << "Send value " << value << std::endl;
		}
	}
	sleep(1);
}

bool WriteModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
	return true;
}
