set (tests
dRemoveAxes
dWorldMap
tCoordinate
tDirectionCoordinate
tGaussianConvert
tLinearCoordinate
tLinearXform
tProjection
tStokesCoordinate
tQualityCoordinate
tTabularCoordinate
)

foreach (test ${tests})
    add_executable (${test} ${test}.cc)
    target_link_libraries (${test} casa_coordinates)
    add_test (${test} ${CMAKE_SOURCE_DIR}/cmake/cmake_assay ./${test})
endforeach (test)
