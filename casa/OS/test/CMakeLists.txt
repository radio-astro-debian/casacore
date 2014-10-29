set (tests
tCanonicalConversion
tConversion
tConversionPerf
tDataConversion
tDirectoryIterator
tEnvVar
tHostInfo
tIBMConversion
tLECanonicalConversion
tMemory
tMemoryTrace
tModcompConversion
tMutex
tPath
tPrecTimer
tTime
tTimer
tVAXConversion
)

foreach (test ${tests})
    add_executable (${test} ${test}.cc)
    target_link_libraries (${test} casa_casa)
    add_test (${test} ${CMAKE_SOURCE_DIR}/cmake/cmake_assay ./${test})
endforeach (test)
