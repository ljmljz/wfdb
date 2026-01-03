# WFDB XML Tools - CMake Configuration

# Find expat library
find_package(EXPAT QUIET)

# If expat is not found, skip XML tools compilation
if(NOT EXPAT_FOUND)
    message(STATUS "EXPAT library not found - skipping XML tools compilation")
    return()
endif()

# List of XML applications
set(XML_APPS
    annxml
    heaxml
    xmlann
    xmlhea
)

# Build each application
foreach(app ${XML_APPS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${app}.c)
        add_executable(${app} ${app}.c)
        
        # Link with WFDB library
        target_link_libraries(${app} wfdb)
        
        # Include expat headers
        target_include_directories(${app} PRIVATE ${EXPAT_INCLUDE_DIRS})
        
        # Link with expat library
        target_link_libraries(${app} ${EXPAT_LIBRARIES})
        
        # Set output name with extension
        if(WIN32)
            set_target_properties(${app} PROPERTIES
                SUFFIX ".exe"
            )
        endif()
        
        # Installation
        install(TARGETS ${app}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endforeach()

# Install XML schema files
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/wfdb.dtd)
    install(FILES wfdb.dtd
        DESTINATION ${CMAKE_INSTALL_DATADIR}/wfdb
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/wfdb.xsl)
    install(FILES wfdb.xsl
        DESTINATION ${CMAKE_INSTALL_DATADIR}/wfdb
    )
endif()