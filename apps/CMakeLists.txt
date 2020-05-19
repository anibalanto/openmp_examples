add_executable(ompe ompe.cpp)
#target_compile_features(app PRIVATE cxx_std_17)

target_link_libraries(ompe PRIVATE OpenMP::OpenMP_CXX)
