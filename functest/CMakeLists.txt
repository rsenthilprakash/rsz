add_executable (test color_convert_utils.c png_utils.c test.c)
target_link_libraries (test resizer ${PNG_LIBS})

add_custom_target (functests ALL COMMAND test ${IMAGES_DIR})
