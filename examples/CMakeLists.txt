build_lib_example(
    NAME dummy-switch-example
    SOURCE_FILES dummy-switch-example.cc
    LIBRARIES_TO_LINK
        ${libdummy-switch}
        ${libcsma}
        ${libinternet}
        ${libapplications}
        ${libtraffic-control}
)