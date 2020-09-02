
if(SerAr_WITH_ConfigFile)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:ConfigFile>)
    target_compile_definitions(AllArchives INTERFACE SERAR_HAS_CONFIGFILE)
endif()

if(SerAr_WITH_ProgramOptions)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:ProgramOptionsWrapper>)
    target_compile_definitions(AllArchives INTERFACE SERAR_HAS_PROGRAMOPTIONS)
endif()

if(SerAr_WITH_MATLAB)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:MATLAB>)
    target_compile_definitions(AllArchives INTERFACE SERAR_HAS_MATLAB)
endif()

if(SerAr_WITH_HDF5)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:HDF5>)
    target_compile_definitions(AllArchives INTERFACE SERAR_HAS_HDF5)
endif()

if(SerAr_WITH_Cereal)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:CerealWrappers)
    target_compile_definitions(AllArchives INTERFACE SERAR_HAS_CEREAL)
endif()
