set(SRCS
    app.cc
    app.h
	main_frame.h
	main_frame.cc
	text_scroll_window.cc
	text_scroll_window.h
	text_file.cc
	text_file.h
	text_line.cc
	text_line.h
	line_number_panel.cc
	line_number_panel.h
	text_panel.cc
	text_panel.h
	config.h
	config.cc
	edit_command.h
	edit_command.cc
	text_listener.h
	selection_region.cc
	selection_region.h
    )

set(TARGET_NAME editor)
#link_directories(${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(
 ${PROJET_SOURCE_DIR}/src/tinyxml
 ${PROJET_SOURCE_DIR}/src/editor
 )

add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${SRCS})
if(WIN32)
    target_link_libraries(${TARGET_NAME} gdiplus tinyxml)
endif()
target_link_libraries(${TARGET_NAME} ${wxWidgets_LIBRARIES})
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_NAME editor)

# Unit test.
if(JIL_ENABLE_TEST)
    set(UT_SRCS
        )
    set(UT_TARGET_NAME app_unittest)
    add_executable(${UT_TARGET_NAME} ${UT_SRCS})
    target_link_libraries(${UT_TARGET_NAME} ${wxWidgets_LIBRARIES} ${GTEST_BOTH_LIBRARIES})
endif()
