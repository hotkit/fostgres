add_library(fostgres-request-logger
        logger.cpp
    )
target_link_libraries(fostgres-request-logger fostgres)
set_target_properties(fostgres-request-logger
    PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fostgres-request-logger
    LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES
        001-initial.blue.sql
    DESTINATION share/fostgres/extras/rquest-logging)

if(TARGET stress OR TARGET pgtest)
    add_custom_command(OUTPUT fostgres-request-logger-test
            COMMAND fostgres-test fostgres-request-logger-test
                "-o" fostgres-request-logger-test
                $<TARGET_SONAME_FILE:fostgres>
                $<TARGET_SONAME_FILE:fostgres-request-logger>
                ${CMAKE_CURRENT_SOURCE_DIR}/request-logger.fg
                ${CMAKE_CURRENT_SOURCE_DIR}/001-initial.blue.sql
                MAIN_DEPENDENCY request-logger.fg
            DEPENDS
                001-initial.blue.sql
                fostgres
                fostgres-request-logger
                fostgres-test
                request-logger.fg
        )
    add_custom_target(fostgres-request-logger-test-target
            DEPENDS
                fostgres-request-logger-test
        )
    if(TARGET stress)
        add_dependencies(stress fostgres-request-logger-test-target)
    endif()
    if(TARGET pgtest)
        add_dependencies(pgtest fostgres-request-logger-test-target)
    endif()
endif()
