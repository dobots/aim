option(COMPILE_TESTS "Compile tests" TRUE)

if (COMPILE_TESTS)

	find_package(GTest REQUIRED)

	SET(test_targets TestRandom)


  foreach(test_family ${test_targets})  

	set(PROJECT_TEST_NAME "${test_family}")
	set(PROJECT_TEST_FILE "${test_family}.cpp")
	
	include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

	add_executable(${PROJECT_TEST_NAME} ${PROJECT_TEST_NAME})
	target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME_STR} ${GTEST_BOTH_LIBRARIES} pthread)

	add_test(${PROJECT_TEST_NAME} ${PROJECT_TEST_NAME})

#    file(GLOB harness_code ${CMAKE_SOURCE_DIR}/test/*.cpp ${CMAKE_SOURCE_DIR}/test/*.h)
#    include_directories(inc)
#    add_test("${test_family}")

  endforeach()  
else (COMPILE_TESTS)

  message(STATUS "Tests compiled, run them with \"make test\"")

endif (COMPILE_TESTS)  