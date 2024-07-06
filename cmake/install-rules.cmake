if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/request_dispatcher-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package request_dispatcher)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT request_dispatcher_Development
)

install(
    TARGETS request_dispatcher_request_dispatcher
    EXPORT request_dispatcherTargets
    RUNTIME #
    COMPONENT request_dispatcher_Runtime
    LIBRARY #
    COMPONENT request_dispatcher_Runtime
    NAMELINK_COMPONENT request_dispatcher_Development
    ARCHIVE #
    COMPONENT request_dispatcher_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    request_dispatcher_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE request_dispatcher_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(request_dispatcher_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${request_dispatcher_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT request_dispatcher_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${request_dispatcher_INSTALL_CMAKEDIR}"
    COMPONENT request_dispatcher_Development
)

install(
    EXPORT request_dispatcherTargets
    NAMESPACE request_dispatcher::
    DESTINATION "${request_dispatcher_INSTALL_CMAKEDIR}"
    COMPONENT request_dispatcher_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
