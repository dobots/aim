#!/bin/bash

LOG_FILE=make.log

# Sets "CORES" and "AVAILABLE_CORES"
set_cores() {
	CORES=1
	AVAILABLE_CORES=`nproc`
	echo "We will compile on ${CORES} of a total of ${AVAILABLE_CORES} cores"
	echo "Note that Unity/Ubuntu grinds to a halt if this reaches the total number of cores available)"
}

set_verbosity() {
	MAKE_VERBOSITY="VERBOSE=1" 
	MAKE_VERBOSITY=
}

set_timing() {
	MAKE_TIMING=time
}

# Create log file and write everything to it
# Prerequisite for check_errors and check_warnings
set_logging() {
	rm -f ${LOG_FILE}
	PIPE_TO_FILE="3>&1 1>&2 2>&3 | tee ${LOG_FILE}"
}

# A log file is a prerequisite, assumes we are still in the build directory
# so create the script file in the parent directory ..
check_errors() {
	echo "Check for errors"
	CHECK_FOR_ERROR="egrep '(error|instantiated from|relocation truncated|multiple definition)'"
	CHECK_FOR_ERROR_CMD="cat ${LOG_FILE} | ${CHECK_FOR_ERROR} | head -n 1"
	ERRORS=`eval ${CHECK_FOR_ERROR_CMD}`
	ERROR_SCRIPT=../check_error.sh
	rm -f ${ERROR_SCRIPT}
	if [ -n "$ERRORS" ]; then
		ERROR_FILE=`eval ${CHECK_FOR_ERROR_CMD} | cut -d':' -f1`
		ERROR_LINE=`eval ${CHECK_FOR_ERROR_CMD} | cut -d':' -f2`
		echo "#!/bin/bash" >> ${ERROR_SCRIPT}
		echo "# -- automatically generated --" >> ${ERROR_SCRIPT}
		echo "gedit ${ERROR_FILE} +${ERROR_LINE}" >> ${ERROR_SCRIPT}
		echo "echo \"Open ${ERROR_FILE} at line ${ERROR_LINE}\"" >> ${ERROR_SCRIPT}
		echo "echo \"because of error: \"" >> ${ERROR_SCRIPT}
		ESCAPE_ERRORS=`echo ${ERRORS} | sed 's|["'\''\`]||g'`
		echo "echo \"   ${ESCAPE_ERRORS} \"" >> ${ERROR_SCRIPT}
		chmod a+x ${ERROR_SCRIPT}
		echo "There are errors! Run ./check_error.sh"
	else
		echo "There are no errors found"
	fi
}

check_warnings() {
	echo "Check for warnings"
	WARNINGS=`cat ${LOG_FILE} | grep "warning"`
	WARNING_SCRIPT=../check_warning.sh
	rm -f ${WARNING_SCRIPT}
	if [ -n "$WARNINGS" ]; then
		WARNING_FILE=`cat ${LOG_FILE} | grep "warning" | cut -d':' -f1`
		WARNING_LINE=`cat ${LOG_FILE} | grep "warning" | cut -d':' -f2`
		echo "gedit ${WARNING_FILE} +${WARNING_LINE}" >> ${WARNING_SCRIPT}
		chmod a+x ${WARNING_SCRIPT}
		echo "There are warnings! Run ./check_warning.sh"
	else
		echo "There are no warnings found"
	fi
}

make_and_check() {
	MAKE_COMMAND="${MAKE_VERBOSITY} ${MAKE_TIMING} make -j${CORES} ${PIPE_TO_FILE}"
	echo "We will perform the following command: "
	echo "  ${MAKE_COMMAND}"
	eval ${MAKE_COMMAND}

	check_errors
	check_warnings
}

build_native() {
	cd ..
	BUILD_DIR=build
	mkdir -p $BUILD_DIR
	cd $BUILD_DIR

	set_cores
	set_verbosity
	set_timing
	set_logging

	cmake -C ../cmake/blackfin.initial.cmake  -DCMAKE_BUILD_TYPE="Debug" \
		..

	make_and_check

	echo "The result of our compilation efforts:"
	cd ..
	file ${BUILD_DIR}/bin/*
}

build_bfin() {
	cd ..
	BUILD_DIR=build_bfin
	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR}

	set_cores
	set_verbosity
	set_timing
	set_logging

	echo "We use the toolchain and disable the tests because of some awkward error"
	echo "Note that CMAKE_TOOLCHAIN_FILE will only be used on the first run"
	echo "it is not allowed to change an existing build tree"
	echo "see: http://www.cmake.org/pipermail/cmake/2011-February/042554.html"
	cmake -C ../cmake/blackfin.initial.cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/blackfin.toolchain.cmake -DCMAKE_BUILD_TYPE="Debug" \
		..

	make_and_check

	echo "The result of our compilation efforts:"
	cd ..
	file ${BUILD_DIR}/bin/*

	ls -latr ${BUILD_DIR}/bin/*
	
	bfin-linux-uclibc-strip ${BUILD_DIR}/bin/*

	ls -latr ${BUILD_DIR}/bin/*

}

help() {
	echo "Build the source by providing an additional argument:"
	echo " native          to build it for the host platform in \"build\""
	echo " bfin            to build it for the blackfin architecture in \"build_bfin\""
}

case "${1}" in
	bfin)
		build_bfin
		;;
	native)
		build_native
		;;
	*)
		help
		;;
esac

