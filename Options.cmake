include(FeatureSummary)

OPTION(SerAr_WITH_HDF5 "Enable HDF5 Component" ON)
add_feature_info(HDF5 SerAr_WITH_HDF5 "Enable HDF5 archives")

OPTION(SerAr_WITH_MATLAB "Enable MATLAB Archives" ON)
add_feature_info(MATLAB SerAr_WITH_MATLAB "Enable MATLAB archives")

OPTION(SerAr_WITH_ConfigFile "Enable ConfigFile" ON)
add_feature_info(ConfigFile SerAr_WITH_ConfigFile "Enable ConfigFile archives")

OPTION(SerAr_WITH_Cereal "Enable CerealWrappers" OFF)
add_feature_info(Cereal SerAr_WITH_CerealWrappers "Include and enable CerealWrappers (requires Cereal)")

OPTION(SerAr_WITH_ProgramOptions "Enable Boost Program_Options_Wrapper" ON)
add_feature_info(ProgramOptions SerAr_WITH_ProgramOptions "Include and enable Wrappers for boost program_options (requires boost COMPONENTS program_options)")

