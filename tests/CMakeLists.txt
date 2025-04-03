# Test configuration
enable_testing()

# Add test executables
add_executable(test_uie_predictor test_uie_predictor.cpp)

# Link dependencies
target_link_libraries(test_uie_predictor
    PRIVATE
    ${PROJECT_NAME}
    ${FASTDEPLOY_LIBS}
)

# Add include directories
target_include_directories(test_uie_predictor
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${FASTDEPLOY_INCS}
)

# Add tests
add_test(
    NAME test_uie_predictor
    COMMAND test_uie_predictor
)