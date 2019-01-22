if(TARGET stress OR TARGET pgtest)
    add_custom_command(OUTPUT test-pg-error
            COMMAND fostgres-test fostgres-test-pg-error -o test-pg-error
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/test-pg-error.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/test-pg-error-view.json
                ${CMAKE_CURRENT_SOURCE_DIR}/test-pg-error.fg
            MAIN_DEPENDENCY test-pg-error.fg
            DEPENDS
                fostgres-test
                test-pg-error.sql
                test-pg-error-view.json
                test-pg-error.fg
        )

    ## Because of the way cmake works we need this stuff at the end to
    ## actually make the above commands run when things change.
    add_custom_target(fostgres-tests DEPENDS
            test-pg-error
    )
    if(TARGET stress)
        add_dependencies(stress fostgres-tests)
    endif()
    if(TARGET pgtest)
        add_dependencies(pgtest fostgres-tests)
    endif()
endif()
