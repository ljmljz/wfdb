# WFDB Power Spectral Density Tools - CMake Configuration

# List of PSD applications
set(PSD_APPS
    coherence
    fft
    log10
    lomb
    memse
)

# Build each application
foreach(app ${PSD_APPS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${app}.c)
        add_executable(${app} ${app}.c)
        
        # Link with WFDB library and math library
        target_link_libraries(${app} wfdb)
        if(UNIX)
            target_link_libraries(${app} m)
        endif()
        
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

# Install additional files
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/hrfft)
    install(PROGRAMS hrfft
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/hrlomb)
    install(PROGRAMS hrlomb
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/hrmem)
    install(PROGRAMS hrmem
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/hrplot)
    install(PROGRAMS hrplot
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plot2d)
    install(PROGRAMS plot2d
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/plot3d)
    install(PROGRAMS plot3d
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()