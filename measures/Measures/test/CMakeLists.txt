set (tests
dM1950_2000
tMeasMath
tMeasureHolder
tRecordTransformable
tStokes
tUVWMachine
)

foreach (test ${tests})
    add_executable (${test} ${test}.cc)
    target_link_libraries (${test} casa_measures)
    add_test (${test} ${CMAKE_SOURCE_DIR}/cmake/cmake_assay ./${test})
endforeach (test)

