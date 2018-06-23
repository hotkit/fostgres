install(DIRECTORY films
    DESTINATION share/fostgres/examples)
install(DIRECTORY tutorial
    DESTINATION share/fostgres/examples)
install(DIRECTORY users
    DESTINATION share/fostgres/examples)

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

add_custom_command(OUTPUT test-film-t1
        COMMAND fostgres-test fostgres-test-film-t1 -o test-film-t1
            ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
            $<TARGET_SONAME_FILE:fostgres>
            ${CMAKE_CURRENT_SOURCE_DIR}/films/films.tables.sql
            ${CMAKE_CURRENT_SOURCE_DIR}/films/view.film-slug.json
            ${CMAKE_CURRENT_SOURCE_DIR}/films/film.t1.fg
        MAIN_DEPENDENCY films/film.t1.fg
        DEPENDS
            fostgres-test
            films/films.tables.sql
            films/view.film-slug.json
            films/film.t1.fg
    )

add_custom_command(OUTPUT test-films
        COMMAND fostgres-test fostgres-test-films -o test-films
            ${CMAKE_CURRENT_SOURCE_DIR}/../Configuration/log-show-all.json
            $<TARGET_SONAME_FILE:fostgres>
            ${CMAKE_CURRENT_SOURCE_DIR}/films/films.tables.sql
            ${CMAKE_CURRENT_SOURCE_DIR}/films/view.film-slug.json
            ${CMAKE_CURRENT_SOURCE_DIR}/films/films.fg
        MAIN_DEPENDENCY films/films.fg
        DEPENDS
            fostgres-test
            films/films.tables.sql
            films/view.film-slug.json
            films/films.fg
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
            fostgres-test
            users/users.tables.sql
            users/view.users.json
            users/user.avatar.fg
    )

## Because of the way cmake works we need this stuff at the end to
## actually make the above commands run when things change.
add_custom_target(fg-tests DEPENDS
        test-comment
        test-empty
        test-films
        test-film-t1
        test-users
        test-user-avatar
    )
add_dependencies(stress fg-tests)
