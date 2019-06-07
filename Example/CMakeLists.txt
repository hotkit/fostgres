install(DIRECTORY films
    DESTINATION share/fostgres/examples)
install(DIRECTORY tutorial
    DESTINATION share/fostgres/examples)
install(DIRECTORY users
    DESTINATION share/fostgres/examples)

if(TARGET stress OR TARGET pgtest)
    ## It's not error to have empty script files that do nothing
    add_custom_command(OUTPUT test-comment
            COMMAND fostgres-test fostgres-test-comment -o test-comment
                ${CMAKE_CURRENT_SOURCE_DIR}/comment.fg
            MAIN_DEPENDENCY comment.fg
            DEPENDS
                fostgres-test
                comment.fg
        )
    add_custom_command(OUTPUT test-empty
            COMMAND fostgres-test fostgres-test-empty -o test-empty
                ${CMAKE_CURRENT_SOURCE_DIR}/empty.fg
            MAIN_DEPENDENCY empty.fg
            DEPENDS
                fostgres-test
                empty.fg
        )

    add_custom_command(OUTPUT test-users
            COMMAND fostgres-test fostgres-test-users -o test-users
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/users/users.tables.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/users/view.users.json
                ${CMAKE_CURRENT_SOURCE_DIR}/users/user.add.fg
            MAIN_DEPENDENCY users/user.add.fg
            DEPENDS
                fostgres
                fostgres-test
                users/users.tables.sql
                users/view.users.json
                users/user.add.fg
        )

    add_custom_command(OUTPUT test-user-avatar
            COMMAND fostgres-test fostgres-test-user-avatar -o test-user-avatar
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/users/users.tables.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/users/view.users.json
                ${CMAKE_CURRENT_SOURCE_DIR}/users/user.avatar.fg
            MAIN_DEPENDENCY users/user.avatar.fg
            DEPENDS
                fostgres
                fostgres-test
                users/users.tables.sql
                users/view.users.json
                users/user.avatar.fg
        )

    add_custom_command(OUTPUT test-pg-error
            COMMAND fostgres-test fostgres-test-pg-error -o test-pg-error
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/test-pg-error.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/test-pg-error-view.json
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/test-pg-error.fg
            MAIN_DEPENDENCY errors/test-pg-error.fg
            DEPENDS
                fostgres-test
                errors/test-pg-error.sql
                errors/test-pg-error-view.json
                errors/test-pg-error.fg
        )

    add_custom_command(OUTPUT test-pg-retry
            COMMAND fostgres-test fostgres-test-pg-retry -o test-pg-retry
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/test-retry.fg
            MAIN_DEPENDENCY errors/test-retry.fg
            DEPENDS
                fostgres-test
                errors/test-retry.fg
        )

    ## Because of the way cmake works we need this stuff at the end to
    ## actually make the above commands run when things change.
    add_custom_target(fg-examples DEPENDS
            test-comment
            test-empty
            test-pg-error
            test-pg-retry
            test-users
            test-user-avatar
        )
    if(TARGET stress)
        add_dependencies(stress fg-examples)
    endif()
    if(TARGET pgtest)
        add_dependencies(pgtest fg-examples)
    endif()
endif()
