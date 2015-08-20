add_executable (TestFirst resizer_test.c framer_test.c TestFirst.c)
target_link_libraries (TestFirst resizer)

add_custom_target (unittests ALL COMMAND TestFirst)
