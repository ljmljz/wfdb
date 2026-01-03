# WFDB Applications - CMake Configuration

# List of applications to build
set(WFDB_APPS
    ann2rr
    bxb
    calsig
    ecgeval
    epicmp
    fir
    gqfuse
    gqpost
    gqrs
    hrstats
    ihr
    mfilt
    mrgann
    mxm
    nguess
    nst
    plotstm
    pscgen
    pschart
    psfd
    rdann
    rdsamp
    rr2ann
    rxr
    sampfreq
    sigamp
    sigavg
    signame
    signum
    skewedit
    snip
    sortann
    sqrs
    sqrs125
    stepdet
    sumann
    sumstats
    tach
    time2sec
    wabp
    wfdb-config
    wfdbcat
    wfdbcollate
    wfdbdesc
    wfdbmap
    wfdbsignals
    wfdbtime
    wfdbwhich
    wqrs
    wrann
    wrsamp
    xform
)

# Build each application
foreach(app ${WFDB_APPS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${app}.c)
        add_executable(${app} ${app}.c)
        
        # Link with WFDB library
        target_link_libraries(${app} wfdb m)
        
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

# Special handling for applications with additional files
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/gqpost.c)
    # gqpost might need configuration files
    install(FILES gqrs.conf
        DESTINATION ${CMAKE_INSTALL_DATADIR}/wfdb
        OPTIONAL
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pschart.pro)
    # Install PostScript prolog files
    install(FILES 
        pschart.pro
        psfd.pro
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/ps
        OPTIONAL
    )
endif()

# Install shell scripts
set(SHELL_SCRIPTS
    cshsetwfdb
    map-record
    pnwlogin
    setwfdb
)

foreach(script ${SHELL_SCRIPTS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${script})
        install(PROGRAMS ${script}
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            OPTIONAL
        )
    endif()
endforeach()