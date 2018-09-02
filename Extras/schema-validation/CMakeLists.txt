add_custom_command(OUTPUT fostgres-schema-validation-test
        COMMAND fostgres-test fostgres-schema-validation-test
            "-o" fostgres-schema-validation-test
            ${CMAKE_CURRENT_SOURCE_DIR}/schema-validation.fg
        MAIN_DEPENDENCY schema-validation.fg
        DEPENDS
            fostgres-test
            schema-validation.fg
    )
add_custom_target(fostgres-schema-validation-test-target
        DEPENDS
            fostgres-schema-validation-test
    )
add_dependencies(stress fostgres-schema-validation-test-target)

