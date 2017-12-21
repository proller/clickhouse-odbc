add_subdirectory(escaping)

file(GLOB clickhouse-odbc-source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
add_library(clickhouse-odbc SHARED ${clickhouse-odbc-source})
add_library(clickhouse-odbcw SHARED ${clickhouse-odbc-source})

target_compile_definitions(clickhouse-odbcw PUBLIC UNICODE)

set_target_properties(clickhouse-odbc
    PROPERTIES PREFIX ""
               OUTPUT_NAME "clickhouse-odbc"
<<<<<<< Updated upstream
)
=======
    )
    set_target_properties(clickhouse-odbc
        PROPERTIES PREFIX ""
               OUTPUT_NAME "clickhouse-odbcw"
    )

else()
    set_target_properties(clickhouse-odbc PROPERTIES OUTPUT_NAME "clickhouseodbc")
    set_target_properties(clickhouse-odbc PROPERTIES OUTPUT_NAME "clickhouseodbcw")
endif()
>>>>>>> Stashed changes

set (clickhouse-odbc-libraries
    clickhouse-odbc-escaping
    ${Poco_Net_LIBRARY}
    ${Poco_Foundation_LIBRARY}
    ${ODBC_LIBRARIES}
)
target_link_libraries(clickhouse-odbc ${clickhouse-odbc-libraries})
target_link_libraries(clickhouse-odbcw ${clickhouse-odbc-libraries})

<<<<<<< Updated upstream
=======
install(TARGETS clickhouse-odbc LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/odbc)
install(TARGETS clickhouse-odbcw LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/odbc)
install(FILES unixodbc.ini DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/clickhouse-odbc)
install(FILES odbc.ini DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples)

>>>>>>> Stashed changes
if(CLICKHOUSE_ODBC_TEST)
    add_subdirectory(ut)
endif()
