# WFDB Format Conversion Tools - CMake Configuration

# List of conversion applications
set(CONVERT_APPS
    a2m
    ad2m
    edf2mit
    m2a
    makeid
    md2a
    mit2edf
    mit2wav
    parsescp
    readid
    revise
    rdedfann
    wav2mit
)

# Build each application
foreach(app ${CONVERT_APPS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${app}.c)
        add_executable(${app} ${app}.c)
        
        # Link with WFDB library
        target_link_libraries(${app} wfdb)
        
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