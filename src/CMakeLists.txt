file(GLOB_RECURSE SOURCES *.cpp *.cc *.h *.hpp *.c)

foreach(f ${SOURCES})
    # Get the path of the file relative to ${DIRECTORY},
    # then alter it (not compulsory)
    file(RELATIVE_PATH SRCGR ${CMAKE_CURRENT_SOURCE_DIR} ${f})
    set(SRCGR "Sources/${SRCGR}")

    # Extract the folder, ie remove the filename part
    string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

    # Source_group expects \\ (double antislash), not / (slash)
    string(REPLACE / \\ SRCGR ${SRCGR})
    source_group("${SRCGR}" FILES ${f})
endforeach()

string(REPLACE pytes.cpp "" SOURCESwithoutMain "${SOURCES}")

add_library(pytes 
   ${SOURCESwithoutMain}
)

add_executable (
   pytes_daemon
   pytes.cpp
)

target_link_libraries(pytes PRIVATE 
   cpprestsdk::cpprest
   CppLinuxSerial::CppLinuxSerial
   OpenSSL::SSL
)


target_link_libraries(pytes_daemon PRIVATE 
   pytes
)
