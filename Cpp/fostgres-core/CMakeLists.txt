add_library(fostgres-core
        dbconnect.cpp
        fostgres.cpp
        iteration.cpp
    )
target_include_directories(fostgres-core PUBLIC ../include)
target_link_libraries(fostgres-core fost-postgres fost-pqxx fost-urlhandler)
install(TARGETS fostgres-core LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
