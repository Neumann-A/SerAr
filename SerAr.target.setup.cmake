
#SerAr_WITH_ConfigFile
#SerAr_WITH_ProgramOptions
#SerAr_WITH_MATLAB
#SerAr_WITH_HDF5
#SerAr_WITH_Cereal

if(SerAr_WITH_ConfigFile)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:ConfigFile>)
endif()

if(SerAr_WITH_ProgramOptions)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:ProgramOptionsWrapper>)
endif()

if(SerAr_WITH_MATLAB)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:MATLAB>)
endif()

if(SerAr_WITH_HDF5)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:HDF5>)
endif()

if(SerAr_WITH_Cereal)
    target_link_libraries(AllArchives INTERFACE $<TARGET_NAME:CerealWrappers)
endif()
