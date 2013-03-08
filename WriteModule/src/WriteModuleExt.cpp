#include <WriteModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 10;

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
bool WriteModuleExt::Tick() {
	std::cout << '[' << getpid() << "] Tick " << lifetime << endl;
	WriteModule::Tick();
	if (lifetime == 9) {
		Connect("/writemodule0/output", "/readmodule0/input");
	}
	if (lifetime == 5) {
		int value = 666;
		std::cout << "Send value " << value << std::endl;
		writeOutput(value);
	}
	sleep(1);
	return true;
}

bool WriteModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
	return true;
}
