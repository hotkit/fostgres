add_library(fostgres-core
        dbconnect.cpp
        fostgres.cpp
        fsigma.cpp
        iteration.cpp
    )
target_include_directories(fostgres-core PUBLIC ../include)
target_link_libraries(fostgres-core fost-postgres fost-urlhandler)
set_target_properties(fostgres-core PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fostgres-core LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY ../include/fostgres DESTINATION include)
