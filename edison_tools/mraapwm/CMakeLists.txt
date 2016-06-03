cmake_minimum_required(VERSION 2.8)
PROJECT (HELLOMRAA)
#set sysroot path
SET (EDISON_SYSROOT "$ENV{HOME}/bin/edison-sdk-ww25/sysroots/core2-32-poky-linux")

#set libs & headers path
SET (SYS_INC ${EDISON_SYSROOT}/usr/include)
SET (SYS_LIB ${EDISON_SYSROOT}/usr/lib)

#set source code dir
#############################################################
######WARINING###############################################
#BIN NAME MUST BE THE SAME AS YOUR build.sh and .bb file!!!!#
#############################################################
SET (BIN_TARGET "mraapwm")
SET (SRC_LIST "src/main.c")

#set compile dir
ADD_EXECUTABLE(${BIN_TARGET} ${SRC_LIST})

#set include and libs
#mraa
INCLUDE_DIRECTORIES(${SYS_INC} ${SYS_INC}/mraa)
LINK_DIRECTORIES(${EDISON_SYSROOT}/usr/lib ${EDISON_SYSROOT}/lib)

#compile with libs
TARGET_LINK_LIBRARIES(${BIN_TARGET} mraa)
