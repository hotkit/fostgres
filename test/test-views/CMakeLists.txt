add_library(test-views
        chain-request.cpp
    )
target_link_libraries(test-views fost-urlhandler)
set_target_properties(test-views PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS test-views LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
smoke_test(test-views)


