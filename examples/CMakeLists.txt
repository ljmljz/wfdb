# WFDB Examples - CMake Configuration

# List of example programs
set(EXAMPLE_PROGRAMS
    example1
    example2
    example3
    example4
    example5
    example6
    example7
    example8
    example9
    example10
)

# Build each example
foreach(example ${EXAMPLE_PROGRAMS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${example}.c)
        add_executable(${example} ${example}.c)
        
        # Link with WFDB library
        target_link_libraries(${example} wfdb)
        
        # Set output name with extension
        if(WIN32)
            set_target_properties(${example} PROPERTIES
                SUFFIX ".exe"
            )
        endif()
        
        # Examples are not installed by default
        # Uncomment to install examples
        # install(TARGETS ${example}
        #     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
        # )
    endif()
endforeach()

# Install example data files if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/README)
    install(FILES README
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples
        OPTIONAL
    )
endif()

# Install any additional example files
file(GLOB EXAMPLE_FILES "*.c" "*.h" "README*" "*.txt")
if(EXAMPLE_FILES)
    install(FILES ${EXAMPLE_FILES}
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples
        OPTIONAL
    )
endif()