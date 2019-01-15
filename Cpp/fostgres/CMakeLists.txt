add_library(fostgres
        configuration.cpp
        datum.cpp
        file.cpp
        fostgres-control-error.cpp
        fostgres-sql.cpp
        matcher.cpp
        response.cpp
        response.csj.cpp
        response.json-csv.cpp
        response.object.cpp
        sql.cpp
        updater.cpp
    )
target_link_libraries(fostgres f5-json-schema fostgres-core fost-csj)
set_target_properties(fostgres PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fostgres LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

if(TARGET check)
    add_library(fostgres-smoke STATIC EXCLUDE_FROM_ALL
            matcher.tests.cpp
            sql.tests.cpp
        )
    target_link_libraries(fostgres-smoke fostgres)
    smoke_test(fostgres-smoke)
endif()
