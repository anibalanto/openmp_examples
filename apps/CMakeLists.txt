add_executable(ompe ompe.cpp)
#target_compile_features(app PRIVATE cxx_std_17)

target_link_libraries(ompe PRIVATE OpenMP::OpenMP_CXX)

install(TARGETS ompe
        CONFIGURATIONS Debug
        RUNTIME DESTINATION Debug/bin)
install(TARGETS ompe
        CONFIGURATIONS Release
        RUNTIME DESTINATION Release/bin)
