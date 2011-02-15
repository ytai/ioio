#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
MKDIR=mkdir -p
RM=rm -f 
CP=cp 
# Macros
CND_CONF=default

ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf
endif
# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}
# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/455219722/usb_host.o ${OBJECTDIR}/_ext/1472/flash.o ${OBJECTDIR}/_ext/1472/adb.o ${OBJECTDIR}/_ext/1472/adb_file.o ${OBJECTDIR}/_ext/1537399865/uart2.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/ioio_file.o ${OBJECTDIR}/_ext/1472/usb_config.o ${OBJECTDIR}/_ext/1472/usb_host_android.o ${OBJECTDIR}/_ext/1472/adb_packet.o ${OBJECTDIR}/_ext/1472/bootloader.o ${OBJECTDIR}/_ext/1472/logging.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

OS_ORIGINAL="Linux"
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC=/opt/microchip/mplabc30/v3.24/bin/pic30-gcc
MP_AS=/opt/microchip/mplabc30/v3.24/bin/pic30-as
MP_LD=/opt/microchip/mplabc30/v3.24/bin/pic30-ld
MP_AR=/opt/microchip/mplabc30/v3.24/bin/pic30-ar
MP_CC_DIR=/opt/microchip/mplabc30/v3.24/bin
MP_AS_DIR=/opt/microchip/mplabc30/v3.24/bin
MP_LD_DIR=/opt/microchip/mplabc30/v3.24/bin
MP_AR_DIR=/opt/microchip/mplabc30/v3.24/bin
.build-conf: ${BUILD_SUBPROJECTS}
ifneq ($(OS_CURRENT),$(OS_ORIGINAL))
	@echo "***** WARNING: This make file contains OS dependent code. The OS this makefile is being run is different from the OS it was created in."
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/455219722/usb_host.o: ../microchip/usb/usb_host.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/455219722 
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/455219722/usb_host.o.d -o ${OBJECTDIR}/_ext/455219722/usb_host.o ../microchip/usb/usb_host.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/455219722/usb_host.o.d > ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${CP} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/455219722/usb_host.o.d > ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${CP} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp}
endif
${OBJECTDIR}/_ext/1472/flash.o: ../flash.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/flash.o.d -o ${OBJECTDIR}/_ext/1472/flash.o ../flash.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/flash.o.d > ${OBJECTDIR}/_ext/1472/flash.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/flash.o.tmp ${OBJECTDIR}/_ext/1472/flash.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/flash.o.d > ${OBJECTDIR}/_ext/1472/flash.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/flash.o.tmp ${OBJECTDIR}/_ext/1472/flash.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.tmp}
endif
${OBJECTDIR}/_ext/1472/adb.o: ../adb.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/adb.o.d -o ${OBJECTDIR}/_ext/1472/adb.o ../adb.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adb.o.d > ${OBJECTDIR}/_ext/1472/adb.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb.o.tmp ${OBJECTDIR}/_ext/1472/adb.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adb.o.d > ${OBJECTDIR}/_ext/1472/adb.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb.o.tmp ${OBJECTDIR}/_ext/1472/adb.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.tmp}
endif
${OBJECTDIR}/_ext/1472/adb_file.o: ../adb_file.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/adb_file.o.d -o ${OBJECTDIR}/_ext/1472/adb_file.o ../adb_file.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adb_file.o.d > ${OBJECTDIR}/_ext/1472/adb_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adb_file.o.d > ${OBJECTDIR}/_ext/1472/adb_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp}
endif
${OBJECTDIR}/_ext/1537399865/uart2.o: ../microchip/common/uart2.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1537399865 
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1537399865/uart2.o.d -o ${OBJECTDIR}/_ext/1537399865/uart2.o ../microchip/common/uart2.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1537399865/uart2.o.d > ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1537399865/uart2.o.d > ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp}
endif
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/main.o.d > ${OBJECTDIR}/_ext/1472/main.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/main.o.tmp ${OBJECTDIR}/_ext/1472/main.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/main.o.d > ${OBJECTDIR}/_ext/1472/main.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/main.o.tmp ${OBJECTDIR}/_ext/1472/main.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.tmp}
endif
${OBJECTDIR}/_ext/1472/ioio_file.o: ../ioio_file.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/ioio_file.o.d -o ${OBJECTDIR}/_ext/1472/ioio_file.o ../ioio_file.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/ioio_file.o.d > ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/ioio_file.o.d > ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp}
endif
${OBJECTDIR}/_ext/1472/usb_config.o: ../usb_config.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/usb_config.o.d -o ${OBJECTDIR}/_ext/1472/usb_config.o ../usb_config.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/usb_config.o.d > ${OBJECTDIR}/_ext/1472/usb_config.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/usb_config.o.d > ${OBJECTDIR}/_ext/1472/usb_config.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp}
endif
${OBJECTDIR}/_ext/1472/usb_host_android.o: ../usb_host_android.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/usb_host_android.o.d -o ${OBJECTDIR}/_ext/1472/usb_host_android.o ../usb_host_android.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/usb_host_android.o.d > ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/usb_host_android.o.d > ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp}
endif
${OBJECTDIR}/_ext/1472/adb_packet.o: ../adb_packet.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/adb_packet.o.d -o ${OBJECTDIR}/_ext/1472/adb_packet.o ../adb_packet.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adb_packet.o.d > ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adb_packet.o.d > ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp}
endif
${OBJECTDIR}/_ext/1472/bootloader.o: ../bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/bootloader.o.d -o ${OBJECTDIR}/_ext/1472/bootloader.o ../bootloader.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/bootloader.o.d > ${OBJECTDIR}/_ext/1472/bootloader.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/bootloader.o.d > ${OBJECTDIR}/_ext/1472/bootloader.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp}
endif
${OBJECTDIR}/_ext/1472/logging.o: ../logging.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/logging.o.d -o ${OBJECTDIR}/_ext/1472/logging.o ../logging.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/logging.o.d > ${OBJECTDIR}/_ext/1472/logging.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/logging.o.tmp ${OBJECTDIR}/_ext/1472/logging.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/logging.o.d > ${OBJECTDIR}/_ext/1472/logging.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/logging.o.tmp ${OBJECTDIR}/_ext/1472/logging.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.tmp}
endif
else
${OBJECTDIR}/_ext/455219722/usb_host.o: ../microchip/usb/usb_host.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/455219722 
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/455219722/usb_host.o.d -o ${OBJECTDIR}/_ext/455219722/usb_host.o ../microchip/usb/usb_host.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/455219722/usb_host.o.d > ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${CP} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/455219722/usb_host.o.d > ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${CP} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp ${OBJECTDIR}/_ext/455219722/usb_host.o.d 
	${RM} ${OBJECTDIR}/_ext/455219722/usb_host.o.tmp}
endif
${OBJECTDIR}/_ext/1472/flash.o: ../flash.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/flash.o.d -o ${OBJECTDIR}/_ext/1472/flash.o ../flash.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/flash.o.d > ${OBJECTDIR}/_ext/1472/flash.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/flash.o.tmp ${OBJECTDIR}/_ext/1472/flash.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/flash.o.d > ${OBJECTDIR}/_ext/1472/flash.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/flash.o.tmp ${OBJECTDIR}/_ext/1472/flash.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/flash.o.tmp}
endif
${OBJECTDIR}/_ext/1472/adb.o: ../adb.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/adb.o.d -o ${OBJECTDIR}/_ext/1472/adb.o ../adb.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adb.o.d > ${OBJECTDIR}/_ext/1472/adb.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb.o.tmp ${OBJECTDIR}/_ext/1472/adb.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adb.o.d > ${OBJECTDIR}/_ext/1472/adb.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb.o.tmp ${OBJECTDIR}/_ext/1472/adb.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb.o.tmp}
endif
${OBJECTDIR}/_ext/1472/adb_file.o: ../adb_file.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/adb_file.o.d -o ${OBJECTDIR}/_ext/1472/adb_file.o ../adb_file.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adb_file.o.d > ${OBJECTDIR}/_ext/1472/adb_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adb_file.o.d > ${OBJECTDIR}/_ext/1472/adb_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp ${OBJECTDIR}/_ext/1472/adb_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_file.o.tmp}
endif
${OBJECTDIR}/_ext/1537399865/uart2.o: ../microchip/common/uart2.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1537399865 
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1537399865/uart2.o.d -o ${OBJECTDIR}/_ext/1537399865/uart2.o ../microchip/common/uart2.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1537399865/uart2.o.d > ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1537399865/uart2.o.d > ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp ${OBJECTDIR}/_ext/1537399865/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/1537399865/uart2.o.tmp}
endif
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/main.o.d > ${OBJECTDIR}/_ext/1472/main.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/main.o.tmp ${OBJECTDIR}/_ext/1472/main.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/main.o.d > ${OBJECTDIR}/_ext/1472/main.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/main.o.tmp ${OBJECTDIR}/_ext/1472/main.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.tmp}
endif
${OBJECTDIR}/_ext/1472/ioio_file.o: ../ioio_file.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/ioio_file.o.d -o ${OBJECTDIR}/_ext/1472/ioio_file.o ../ioio_file.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/ioio_file.o.d > ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/ioio_file.o.d > ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp ${OBJECTDIR}/_ext/1472/ioio_file.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/ioio_file.o.tmp}
endif
${OBJECTDIR}/_ext/1472/usb_config.o: ../usb_config.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/usb_config.o.d -o ${OBJECTDIR}/_ext/1472/usb_config.o ../usb_config.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/usb_config.o.d > ${OBJECTDIR}/_ext/1472/usb_config.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/usb_config.o.d > ${OBJECTDIR}/_ext/1472/usb_config.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp ${OBJECTDIR}/_ext/1472/usb_config.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_config.o.tmp}
endif
${OBJECTDIR}/_ext/1472/usb_host_android.o: ../usb_host_android.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/usb_host_android.o.d -o ${OBJECTDIR}/_ext/1472/usb_host_android.o ../usb_host_android.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/usb_host_android.o.d > ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/usb_host_android.o.d > ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp ${OBJECTDIR}/_ext/1472/usb_host_android.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/usb_host_android.o.tmp}
endif
${OBJECTDIR}/_ext/1472/adb_packet.o: ../adb_packet.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/adb_packet.o.d -o ${OBJECTDIR}/_ext/1472/adb_packet.o ../adb_packet.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adb_packet.o.d > ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adb_packet.o.d > ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp ${OBJECTDIR}/_ext/1472/adb_packet.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adb_packet.o.tmp}
endif
${OBJECTDIR}/_ext/1472/bootloader.o: ../bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/bootloader.o.d -o ${OBJECTDIR}/_ext/1472/bootloader.o ../bootloader.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/bootloader.o.d > ${OBJECTDIR}/_ext/1472/bootloader.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/bootloader.o.d > ${OBJECTDIR}/_ext/1472/bootloader.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp ${OBJECTDIR}/_ext/1472/bootloader.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/bootloader.o.tmp}
endif
${OBJECTDIR}/_ext/1472/logging.o: ../logging.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -I".." -I"../.." -I"../microchip/include" -I"../../blapi" -I"../microchip/USB" -I"../microchip/include/USB" -mlarge-data -mlarge-scalar -mconst-in-data -Os -MMD -MF ${OBJECTDIR}/_ext/1472/logging.o.d -o ${OBJECTDIR}/_ext/1472/logging.o ../logging.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/logging.o.d > ${OBJECTDIR}/_ext/1472/logging.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/logging.o.tmp ${OBJECTDIR}/_ext/1472/logging.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/logging.o.d > ${OBJECTDIR}/_ext/1472/logging.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/logging.o.tmp ${OBJECTDIR}/_ext/1472/logging.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.tmp}
endif
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC}  -omf=elf  -mcpu=24FJ128DA106  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf ${OBJECTFILES}      -Wl,--defsym=__MPLAB_BUILD=1,--script=../boot_p24FJ128DA106.gld,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--heap=512,-L"..",-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem
else
dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC}  -omf=elf  -mcpu=24FJ128DA106  -o dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf ${OBJECTFILES}      -Wl,--defsym=__MPLAB_BUILD=1,--script=../boot_p24FJ128DA106.gld,--heap=512,-L"..",-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem
	${MP_CC_DIR}/pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Bootloader.X.${IMAGE_TYPE}.elf -omf=elf
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist
# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
