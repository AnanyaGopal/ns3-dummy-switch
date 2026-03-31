build_lib(
    LIBNAME dummy-switch
    SOURCE_FILES
        model/dummy-switch-net-device.cc
        helper/dummy-switch-helper.cc
    HEADER_FILES
        model/dummy-switch-net-device.h
        helper/dummy-switch-helper.h
    LIBRARIES_TO_LINK
        ${libnetwork}
        ${libtraffic-control}
        ${libcore}
)