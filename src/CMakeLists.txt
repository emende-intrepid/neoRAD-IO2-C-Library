set(HEADERS_PATH ${PROJECT_SOURCE_DIR}/inc)
set(FRAMEDISC_HEADERS_PATH ${HEADERS_PATH}/neorad-io2-framedescription)
set(HIDAPI_HEADER_DIR ${PROJECT_SOURCE_DIR}/libs/hidapi/hidapi)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

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
elseif(UNIX AND NOT APPLE)
	set(HIDAPI_DIR ${PROJECT_SOURCE_DIR}/libs/hidapi/linux)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	link_libraries("-ludev")
endif()


set(HEADER 	${HEADERS_PATH}/fifo.h
			${HEADERS_PATH}/ft260.h
			${HEADERS_PATH}/neoRAD-IO2.h
			${HEADERS_PATH}/neoRAD-IO2_PacketHandler.h
			${HEADERS_PATH}/neoRAD-IO2-TC.h
			${HEADERS_PATH}/neoRAD-IO2-AIN.h
			${HEADERS_PATH}/neoRAD-IO2-AOUT.h
			${HEADERS_PATH}/neoRAD-IO2-PWRRLY.h
			${FRAMEDISC_HEADERS_PATH}/radio2_frames.h
            ${HIDAPI_HEADER_DIR}/hidapi.h)

set(SRC ft260.c
		fifo.c
		neoRAD-IO2.c
		neoRAD-IO2_PacketHandler.c
        ${HIDAPI_DIR}/hid.c)

if(APPLE)
    set(SRC ${SRC} ${CoreFoundation} ${IOKit} ${ObjC})
endif()


add_library(neoRADIO2 ${SRC} ${HEADER})
target_include_directories(neoRADIO2 PUBLIC
									${CMAKE_CURRENT_BINARY_DIR}
									${HIDAPI_DIR}
									${HIDAPI_HEADER_DIR}
									${FRAMEDISC_HEADERS_PATH}
									${HEADERS_PATH})

