# WFDB Library - CMake Configuration

# Generate header files from .h0 templates
# Generate wfdb.h from wfdb.h0
set(NETFILES_CONFIG "0")
set(NETFILES_LIBCURL_CONFIG "0")
if(CURL_FOUND AND ENABLE_NETFILES)
    set(NETFILES_CONFIG "1")
    set(NETFILES_LIBCURL_CONFIG "1")
endif()

add_custom_command(
    OUTPUT wfdb.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wfdb
    COMMAND ${CMAKE_COMMAND} -E echo "s/WFDB_NETFILES 1/WFDB_NETFILES ${NETFILES_CONFIG}/" > ${CMAKE_CURRENT_BINARY_DIR}/sed_script.txt
    COMMAND ${CMAKE_COMMAND} -E echo "s/WFDBMAJOR/${PROJECT_VERSION_MAJOR}/" >> ${CMAKE_CURRENT_BINARY_DIR}/sed_script.txt
    COMMAND ${CMAKE_COMMAND} -E echo "s/WFDBMINOR/${PROJECT_VERSION_MINOR}/" >> ${CMAKE_CURRENT_BINARY_DIR}/sed_script.txt
    COMMAND ${CMAKE_COMMAND} -E echo "s/WFDBRELEASE/${PROJECT_VERSION_PATCH}/" >> ${CMAKE_CURRENT_BINARY_DIR}/sed_script.txt
    COMMAND ${CMAKE_COMMAND} -E echo "s/WFDB_NETFILES_LIBCURL 1/WFDB_NETFILES_LIBCURL ${NETFILES_LIBCURL_CONFIG}/" >> ${CMAKE_CURRENT_BINARY_DIR}/sed_script.txt
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/sed_script.txt ${CMAKE_CURRENT_SOURCE_DIR}/wfdb.h0 > ${CMAKE_CURRENT_BINARY_DIR}/wfdb/wfdb.h
    COMMAND sed -f ${CMAKE_CURRENT_BINARY_DIR}/sed_script.txt ${CMAKE_CURRENT_SOURCE_DIR}/wfdb.h0 > ${CMAKE_CURRENT_BINARY_DIR}/wfdb.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ecgcodes.h ${CMAKE_CURRENT_BINARY_DIR}/wfdb/ecgcodes.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ecgmap.h ${CMAKE_CURRENT_BINARY_DIR}/wfdb/ecgmap.h
    DEPENDS wfdb.h0
    COMMENT "Generating wfdb.h from wfdb.h0"
)

# Generate wfdblib.h from wfdblib.h0
add_custom_command(
    OUTPUT wfdblib.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wfdb
    COMMAND sed "s+DBDIR+${CMAKE_INSTALL_PREFIX}/database+" ${CMAKE_CURRENT_SOURCE_DIR}/wfdblib.h0 > ${CMAKE_CURRENT_BINARY_DIR}/wfdb/wfdblib.h
    COMMAND sed "s+DBDIR+${CMAKE_INSTALL_PREFIX}/database+" ${CMAKE_CURRENT_SOURCE_DIR}/wfdblib.h0 > ${CMAKE_CURRENT_BINARY_DIR}/wfdblib.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ecgcodes.h ${CMAKE_CURRENT_BINARY_DIR}/wfdb/ecgcodes.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ecgmap.h ${CMAKE_CURRENT_BINARY_DIR}/wfdb/ecgmap.h
    DEPENDS wfdblib.h0
    COMMENT "Generating wfdblib.h from wfdblib.h0"
)

# Source files for WFDB library
set(WFDB_SOURCES
    annot.c
    calib.c
    signal.c
    wfdbio.c
    wfdbinit.c
)

# Header files
set(WFDB_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/wfdb.h
    ${CMAKE_CURRENT_BINARY_DIR}/wfdblib.h
    ecgcodes.h
    ecgmap.h
)

# Create WFDB library
if(BUILD_SHARED_LIBS)
    add_library(wfdb SHARED ${WFDB_SOURCES} ${WFDB_HEADERS})
else()
    add_library(wfdb STATIC ${WFDB_SOURCES} ${WFDB_HEADERS})
endif()

# Add dependencies to ensure headers are generated before compilation
add_custom_target(generate_headers ALL DEPENDS wfdb.h wfdblib.h)
add_dependencies(wfdb generate_headers)

# Set library properties
set_target_properties(wfdb PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 10
    OUTPUT_NAME wfdb
)

# Add include directories for the library itself
target_include_directories(wfdb PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/wfdb>
)

# Link libraries
if(CURL_FOUND AND ENABLE_NETFILES)
    target_link_libraries(wfdb ${CURL_LIBRARIES})
endif()

if(FLAC_FOUND AND ENABLE_FLAC)
    target_link_libraries(wfdb ${FLAC_LIBRARIES})
endif()

# Windows specific
if(WIN32)
    set_target_properties(wfdb PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

# Installation
install(TARGETS wfdb
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/wfdb/wfdb.h
    ${CMAKE_CURRENT_BINARY_DIR}/wfdb/wfdblib.h
    ecgcodes.h
    ecgmap.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/wfdb
    COMPONENT headers
)

# pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/wfdb.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/wfdb.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/wfdb.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)