add_library(fostgres-schema-loader
        loader.cpp
    )
target_link_libraries(fostgres-schema-loader
    fost-postgres f5-json-schema)
set_target_properties(fostgres-schema-loader
    PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fostgres-schema-loader
    LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(FILES
        schema-validation-001.sql
        schema-validation.json
    DESTINATION share/fostgres/extras/schema-validation)


if(TARGET stress OR TARGET pgtest)
    add_custom_command(OUTPUT fostgres-schema-validation-test
            COMMAND fostgres-test fostgres-schema-validation-test
                "-o" fostgres-schema-validation-test
                $<TARGET_SONAME_FILE:fostgres>
                $<TARGET_SONAME_FILE:fostgres-schema-loader>
                ${CMAKE_CURRENT_SOURCE_DIR}/schema-validation.fg
                ${CMAKE_CURRENT_SOURCE_DIR}/schema-validation.json
                ${CMAKE_CURRENT_SOURCE_DIR}/schema-validation-001.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/test-table.sql
                ${CMAKE_CURRENT_SOURCE_DIR}/test-view.json
            MAIN_DEPENDENCY schema-validation.fg
            DEPENDS
                fostgres
                fostgres-schema-loader
                fostgres-test
                schema-validation.fg
                schema-validation.json
                schema-validation-001.sql
                test-table.sql
                test-view.json
        )
    add_custom_target(fostgres-schema-validation-test-target
            DEPENDS
                fostgres-schema-validation-test
        )
    if(TARGET stress)
        add_dependencies(stress fostgres-schema-validation-test-target)
    endif()
    if(TARGET pgtest)
        add_dependencies(pgtest fostgres-schema-validation-test-target)
    endif()
endif()
