PROJECT (HELLOMRAA)
#set sysroot path
SET (EDISON_SYSROOT "$ENV{HOME}/bin/edison-sdk-ww25/sysroots/core2-32-poky-linux")

#set libs & headers path
SET (MRAA_INC ${EDISON_SYSROOT}/usr/include)
SET (MRAA_LIB ${EDISON_SYSROOT}/usr/lib)

#set source code dir
#############################################################
######WARINING###############################################
#BIN NAME MUST BE THE SAME AS YOUR build.sh and .bb file!!!!#
#############################################################
SET (BIN_TARGET "check5611")
SET (SRC_LIST "src/main.cpp"
              "src/ArduinoWrapper.cpp"
              "src/edison_i2c/edison_i2c.cpp"
              "src/edison_i2c/i2c.c"
              "src/MS5611/MS561101BA.cpp")

MESSAGE(STATUS "This is sys_roots:" ${EDISON_SYSROOT})
MESSAGE(STATUS "This is mraa_include:" ${MRAA_INC})
MESSAGE(STATUS "This is mraa_libs:" ${MRAA_LIB})
MESSAGE(STATUS "This is src_list:" ${SRC_LIST})

#set compile dir
ADD_EXECUTABLE(${BIN_TARGET} ${SRC_LIST} )

#set include and libs
#mraa
INCLUDE_DIRECTORIES(${MRAA_INC} "src/MS5611" "src/edison_i2c" "src")
LINK_DIRECTORIES(${MRAA_LIB} )

#compile with libs
TARGET_LINK_LIBRARIES(${BIN_TARGET} libmraa.so)
