
# if(SERAR_WITH_CONFIGFILE)
#     target_link_libraries(SerAr::AllArchives INTERFACE $<TARGET_NAME:SerAr::ConfigFile>)
#     target_compile_definitions(SerAr::AllArchives INTERFACE SERAR_HAS_CONFIGFILE)
# endif()

# if(SERAR_WITH_PROGRAMOPTIONS)
#     target_link_libraries(SerAr::AllArchives INTERFACE $<TARGET_NAME:SerAr::ProgramOptionsWrapper>)
#     target_compile_definitions(SerAr::AllArchives INTERFACE SERAR_HAS_PROGRAMOPTIONS)
# endif()

# if(SERAR_WITH_MATLAB)
#     target_link_libraries(SerAr::AllArchives INTERFACE $<TARGET_NAME:SerAr::MATLAB>)
#     target_compile_definitions(SerAr::AllArchives INTERFACE SERAR_HAS_MATLAB)
# endif()

# if(SERAR_WITH_HDF5)
#     target_link_libraries(SerAr::AllArchives INTERFACE $<TARGET_NAME:SerAr::HDF5>)
#     target_compile_definitions(SerAr::AllArchives INTERFACE SERAR_HAS_HDF5)
# endif()

# if(SERAR_WITH_CEREAL)
#     target_link_libraries(SerAr::AllArchives INTERFACE $<TARGET_NAME:SerAr::CerealWrappers>)
#     target_compile_definitions(SerAr::AllArchives INTERFACE SERAR_HAS_CEREAL)
# endif()

# if(SERAR_WITH_JSON)
#     target_link_libraries(SerAr::AllArchives INTERFACE $<TARGET_NAME:SerAr::JSON>)
#     target_compile_definitions(SerAr::AllArchives INTERFACE SERAR_HAS_JSON)
# endif()
