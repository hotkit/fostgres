add_subdirectory(fost-csj)
add_subdirectory(fostgres)
add_subdirectory(fostgres-core)
add_subdirectory(fostgres-fg)
add_subdirectory(fostgres-test)

install(DIRECTORY include/fost DESTINATION include)
install(DIRECTORY include/fostgres DESTINATION include)
