install(DIRECTORY films
    DESTINATION share/fostgres/examples)
install(DIRECTORY tutorial
    DESTINATION share/fostgres/examples)
install(DIRECTORY users
    DESTINATION share/fostgres/examples)
install(DIRECTORY datum
    DESTINATION share/fostgres/examples)

if(TARGET stress OR TARGET pgtest)
    add_custom_command(OUTPUT example-comment
            COMMAND fostgres-test fostgres-example-comment -o example-comment
                ${CMAKE_CURRENT_SOURCE_DIR}/comment.fg
            MAIN_DEPENDENCY comment.fg
            DEPENDS
                fostgres-test
                comment.fg
        )

    add_custom_command(OUTPUT example-datum
            COMMAND fostgres-test fostgres-example-datum -o example-datum
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/datum/datum.fg
                ${CMAKE_CURRENT_SOURCE_DIR}/datum/datum.tables.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/datum/view.datum.json
            MAIN_DEPENDENCY datum/datum.fg
            DEPENDS
                fostgres
                fostgres-test
                datum/datum.fg
                datum/datum.tables.sql
                datum/view.datum.json
        )

    add_custom_command(OUTPUT example-empty
            COMMAND fostgres-test fostgres-example-empty -o example-empty
                ${CMAKE_CURRENT_SOURCE_DIR}/empty.fg
            MAIN_DEPENDENCY empty.fg
            DEPENDS
                fostgres-test
                empty.fg
        )

    add_custom_command(OUTPUT example-films
            COMMAND fostgres-test fostgres-example-films -o example-films
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/films/films.fg
                ${CMAKE_CURRENT_SOURCE_DIR}/films/films.tables.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/films/views.json
            MAIN_DEPENDENCY films/films.fg
            DEPENDS
                fostgres
                fostgres-test
                films/films.fg
                films/films.tables.sql
                films/views.json
        )

    add_custom_command(OUTPUT example-users
            COMMAND fostgres-test fostgres-example-users -o example-users
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

    add_custom_command(OUTPUT example-user-avatar
            COMMAND fostgres-test fostgres-example-user-avatar -o example-user-avatar
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

    add_custom_command(OUTPUT example-pg-error
            COMMAND fostgres-test fostgres-example-pg-error -o example-pg-error
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/pg-error.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/pg-error-view.json
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/pg-error.fg
            MAIN_DEPENDENCY errors/pg-error.fg
            DEPENDS
                fostgres-test
                errors/pg-error.sql
                errors/pg-error-view.json
                errors/pg-error.fg
        )

    add_custom_command(OUTPUT example-pg-retry
            COMMAND fostgres-test fostgres-example-pg-retry -o example-pg-retry
                ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
                $<TARGET_SONAME_FILE:fostgres>
                ${CMAKE_CURRENT_SOURCE_DIR}/errors/retry.fg
            MAIN_DEPENDENCY errors/retry.fg
            DEPENDS
                fostgres-test
                errors/retry.fg
        )

    ## Because of the way cmake works we need this stuff at the end to
    ## actually make the above commands run when things change.
    add_custom_target(fg-examples DEPENDS
            example-comment
            example-datum
            example-empty
            example-films
            example-pg-error
            example-pg-retry
            example-users
            example-user-avatar
        )
    if(TARGET stress)
        add_dependencies(stress fg-examples)
    endif()
    if(TARGET pgtest)
        add_dependencies(pgtest fg-examples)
    endif()
endif()
