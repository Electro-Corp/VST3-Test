cmake_minimum_required(VERSION 3.14.0)

option(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES "Enable VST 3 Plug-in Examples" OFF)
option(SMTG_ENABLE_VST3_HOSTING_EXAMPLES "Enable VST 3 Hosting Examples" OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12 CACHE STRING "")

set(vst3sdk_SOURCE_DIR "../../vst3sdk")
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(ECNext
    # This is your plug-in version number. Change it here only.
    # Version number symbols usable in C++ can be found in
    # source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
    VERSION 1.0.0.0 
    DESCRIPTION "ECNext VST 3 Plug-in"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(ECNext
    source/version.h
    source/ECNex_cids.h
    source/ECNex_processor.h
    source/ECNex_processor.cpp
    source/ECNex_controller.h
    source/ECNex_controller.cpp
    source/ECNex_entry.cpp
)

#- VSTGUI Wanted ----
if(SMTG_ENABLE_VSTGUI_SUPPORT)
    target_sources(ECNext
        PRIVATE
            resource/ECNex_Main.uidesc
    )
    target_link_libraries(ECNext
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(ECNext
        RESOURCES
            "resource/ECNex_Main.uidesc"
    )
endif(SMTG_ENABLE_VSTGUI_SUPPORT)
# -------------------

smtg_target_add_plugin_snapshots (ECNext
    RESOURCES
        resource/8D05239369E4512FA503B05BFF8F50D3_snapshot.png
        resource/8D05239369E4512FA503B05BFF8F50D3_snapshot_2.0x.png
)

target_link_libraries(ECNext
    PRIVATE
        sdk
)

smtg_target_configure_version_file(ECNext)


if(SMTG_MAC)
    smtg_target_set_bundle(ECNext
        BUNDLE_IDENTIFIER com.electrocorp.vst3.ecnex
        COMPANY_NAME "Electro-Corp"
    )
    smtg_target_set_debug_executable(ECNext
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(ECNext PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ECNext)

        smtg_target_set_debug_executable(ECNext
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
