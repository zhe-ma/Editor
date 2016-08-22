set(SRCS
	tinystr.h
	tinystr.cpp
	tinyxml.h
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp
    )

add_library(tinyxml ${SRCS})
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

