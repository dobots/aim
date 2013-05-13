option(COMPILE_TESTS "Compile tests" TRUE)

if (COMPILE_TESTS)
	# use Google test
	find_package(GTest REQUIRED)
	
	# define the list of test units
	set(test_targets TestRandom TestHough)

	set(PROJECT_TESTLIB ${PROJECT_NAME}Test)
	message(STATUS "Use project test shared library: ${PROJECT_TESTLIB}")

	# iterate through a family of test units
	foreach(test_family ${test_targets})  

		set(PROJECT_TEST_NAME "${test_family}")
		set(PROJECT_TEST_FILE "${test_family}.cpp")
	
		include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})
		message(STATUS "Project test name: ${PROJECT_TEST_NAME}")
		add_executable(${PROJECT_TEST_NAME} ${PROJECT_TEST_NAME})
		target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME_STR} ${GTEST_BOTH_LIBRARIES} pthread ${PROJECT_TESTLIB})

		add_test(${PROJECT_TEST_NAME} ${PROJECT_TEST_NAME})

  endforeach()
    
else (COMPILE_TESTS)
	message(STATUS "Tests compiled, run them with \"make test\"")
endif (COMPILE_TESTS)  