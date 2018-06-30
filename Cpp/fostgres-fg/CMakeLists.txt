add_library(fostgres-fg
        contains.cpp
        fg.cpp
        fg.builtin.cpp
        fg.builtin.checks.cpp
        fg.builtin.http.cpp
        fg.builtin.sql.cpp
        fg.call.cpp
        fg.frame.cpp
        fg.parser.cpp
        fg.testserver.cpp
        mime.cpp
    )
target_link_libraries(fostgres-fg fost-test fostgres)
install(TARGETS fostgres-fg LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

