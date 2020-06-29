add_library(fostgres-fg
        contains.cpp
        fg.cpp
        fg.builtin.cpp
        fg.builtin.checks.cpp
        fg.builtin.expect.cpp
        fg.builtin.http.cpp
        fg.builtin.sql.cpp
        fg.parser.cpp
        fg.testserver.cpp
        mime.cpp
    )
target_link_libraries(fostgres-fg fost-csj fost-test fostgres-core)
set_target_properties(fostgres-fg PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fostgres-fg LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

