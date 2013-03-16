#include <ReadModuleExt.h>

//#include <stdio.h>
//#include <unistd.h>
#include <string>
#include <iostream>
#include <unistd.h>

using namespace rur;
using namespace std;

static int lifetime = 1000;

/***********************************************************************************************************************
 * Implementation
 **********************************************************************************************************************/

ReadModuleExt::ReadModuleExt() {
}

ReadModuleExt::~ReadModuleExt() {
	std::cout << "Deallocate object and close socket to name server" << endl;
}

/**
 * This example is from the web, but doesn't seem to work.
 */
bool ReadModuleExt::Tick() {
	std::cout << '[' << getpid() << "] Tick " << lifetime << endl;
	ReadModule::Tick();

	int *value = readInput(false);
	if (value != NULL) std::cout << "Got value " << *value << std::endl;

	sleep(1);
	return true;
}

bool ReadModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
	return true;
}
