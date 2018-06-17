add_executable(fostgres-test fostgres-test.cpp)
target_link_libraries(fostgres-test fost-cli fostgres-fg)
install(TARGETS fostgres-test EXPORT fostgres-test RUNTIME DESTINATION bin)
