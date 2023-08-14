add_library(inja INTERFACE)

target_include_directories(inja INTERFACE .)

set_target_properties(inja PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:inja,INTERFACE_INCLUDE_DIRECTORIES>)
