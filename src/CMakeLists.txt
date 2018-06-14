set(HEADERS_PATH ${PROJECT_SOURCE_DIR}/inc)
set(HIDAPI_HEADER_DIR ${PROJECT_SOURCE_DIR}/libs/hidapi/hidapi)

if(WIN32)
    set(HIDAPI_DIR ${PROJECT_SOURCE_DIR}/libs/hidapi/windows)
    if(MSVC)
	
    else()
		link_libraries("-lhid -lsetupapi")
    endif()
elseif(APPLE)
    set(HIDAPI_DIR ${PROJECT_SOURCE_DIR}/libs/hidapi/mac)
    find_library(ObjC objc)
    find_library(CoreFoundation CoreFoundation)
    find_library(IOKit IOKit)
else()
    SET(HIDAPI_DIR ${PROJECT_SOURCE_DIR}/libs/hidapi/mac)
endif()


set(HEADER 	${HEADERS_PATH}/fifo.h
			${HEADERS_PATH}/ft260.h
			${HEADERS_PATH}/neoRAD-IO2.h
			${HEADERS_PATH}/neoRADIO2_frames.h
			${HEADERS_PATH}/neoRAD-IO2_PacketHandler.h
			${HEADERS_PATH}/neoRAD-IO2-TC.h
            ${HIDAPI_HEADER_DIR}/hidapi.h)

set(SRC ft260.c
		fifo.c
		neoRAD-IO2.c
		neoRAD-IO2_PacketHandler.c
		neoRAD-IO2-TC.c
        ${HIDAPI_DIR}/hid.c)

if(APPLE)
    set(SRC ${SRC} ${CoreFoundation} ${IOKit} ${ObjC})
endif()


add_library(neoRADIO2 ${SRC} ${HEADER})
target_include_directories(neoRADIO2 PUBLIC
									${CMAKE_CURRENT_BINARY_DIR}
									${HIDAPI_DIR}
									${HIDAPI_HEADER_DIR}
									${HEADERS_PATH})

