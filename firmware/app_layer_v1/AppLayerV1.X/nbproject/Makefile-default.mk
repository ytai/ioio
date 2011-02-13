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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf
endif
# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}
# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/pwm.o ${OBJECTDIR}/_ext/1472/features.o ${OBJECTDIR}/_ext/1472/byte_queue.o ${OBJECTDIR}/_ext/1472/digital.o ${OBJECTDIR}/_ext/1472/pins.o ${OBJECTDIR}/_ext/1472/protocol.o ${OBJECTDIR}/_ext/1472/uart.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/adc.o ${OBJECTDIR}/_ext/131612190/uart2.o ${OBJECTDIR}/_ext/1472/logging.o


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/pwm.o: ../pwm.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/pwm.o.d -o ${OBJECTDIR}/_ext/1472/pwm.o ../pwm.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/pwm.o.d > ${OBJECTDIR}/_ext/1472/pwm.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pwm.o.tmp ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/pwm.o.d > ${OBJECTDIR}/_ext/1472/pwm.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pwm.o.tmp ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.tmp}
endif
${OBJECTDIR}/_ext/1472/features.o: ../features.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/features.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/features.o.d -o ${OBJECTDIR}/_ext/1472/features.o ../features.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/features.o.d > ${OBJECTDIR}/_ext/1472/features.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/features.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/features.o.tmp ${OBJECTDIR}/_ext/1472/features.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/features.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/features.o.d > ${OBJECTDIR}/_ext/1472/features.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/features.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/features.o.tmp ${OBJECTDIR}/_ext/1472/features.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/features.o.tmp}
endif
${OBJECTDIR}/_ext/1472/byte_queue.o: ../byte_queue.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/byte_queue.o.d -o ${OBJECTDIR}/_ext/1472/byte_queue.o ../byte_queue.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/byte_queue.o.d > ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/byte_queue.o.d > ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp}
endif
${OBJECTDIR}/_ext/1472/digital.o: ../digital.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/digital.o.d -o ${OBJECTDIR}/_ext/1472/digital.o ../digital.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/digital.o.d > ${OBJECTDIR}/_ext/1472/digital.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/digital.o.tmp ${OBJECTDIR}/_ext/1472/digital.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/digital.o.d > ${OBJECTDIR}/_ext/1472/digital.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/digital.o.tmp ${OBJECTDIR}/_ext/1472/digital.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.tmp}
endif
${OBJECTDIR}/_ext/1472/pins.o: ../pins.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/pins.o.d -o ${OBJECTDIR}/_ext/1472/pins.o ../pins.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/pins.o.d > ${OBJECTDIR}/_ext/1472/pins.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pins.o.tmp ${OBJECTDIR}/_ext/1472/pins.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/pins.o.d > ${OBJECTDIR}/_ext/1472/pins.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pins.o.tmp ${OBJECTDIR}/_ext/1472/pins.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.tmp}
endif
${OBJECTDIR}/_ext/1472/protocol.o: ../protocol.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/protocol.o.d -o ${OBJECTDIR}/_ext/1472/protocol.o ../protocol.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/protocol.o.d > ${OBJECTDIR}/_ext/1472/protocol.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/protocol.o.tmp ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/protocol.o.d > ${OBJECTDIR}/_ext/1472/protocol.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/protocol.o.tmp ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.tmp}
endif
${OBJECTDIR}/_ext/1472/uart.o: ../uart.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/uart.o.d -o ${OBJECTDIR}/_ext/1472/uart.o ../uart.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/uart.o.d > ${OBJECTDIR}/_ext/1472/uart.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/uart.o.tmp ${OBJECTDIR}/_ext/1472/uart.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/uart.o.d > ${OBJECTDIR}/_ext/1472/uart.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/uart.o.tmp ${OBJECTDIR}/_ext/1472/uart.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.tmp}
endif
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c 
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
${OBJECTDIR}/_ext/1472/adc.o: ../adc.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/adc.o.d -o ${OBJECTDIR}/_ext/1472/adc.o ../adc.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adc.o.d > ${OBJECTDIR}/_ext/1472/adc.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adc.o.tmp ${OBJECTDIR}/_ext/1472/adc.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adc.o.d > ${OBJECTDIR}/_ext/1472/adc.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adc.o.tmp ${OBJECTDIR}/_ext/1472/adc.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.tmp}
endif
${OBJECTDIR}/_ext/131612190/uart2.o: ../../../../mal/Common/uart2.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/131612190 
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/131612190/uart2.o.d -o ${OBJECTDIR}/_ext/131612190/uart2.o ../../../../mal/Common/uart2.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/131612190/uart2.o.d > ${OBJECTDIR}/_ext/131612190/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/131612190/uart2.o.d > ${OBJECTDIR}/_ext/131612190/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp}
endif
${OBJECTDIR}/_ext/1472/logging.o: ../logging.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${MP_CC} -g -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/logging.o.d -o ${OBJECTDIR}/_ext/1472/logging.o ../logging.c 
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
${OBJECTDIR}/_ext/1472/pwm.o: ../pwm.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/pwm.o.d -o ${OBJECTDIR}/_ext/1472/pwm.o ../pwm.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/pwm.o.d > ${OBJECTDIR}/_ext/1472/pwm.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pwm.o.tmp ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/pwm.o.d > ${OBJECTDIR}/_ext/1472/pwm.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pwm.o.tmp ${OBJECTDIR}/_ext/1472/pwm.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pwm.o.tmp}
endif
${OBJECTDIR}/_ext/1472/features.o: ../features.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/features.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/features.o.d -o ${OBJECTDIR}/_ext/1472/features.o ../features.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/features.o.d > ${OBJECTDIR}/_ext/1472/features.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/features.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/features.o.tmp ${OBJECTDIR}/_ext/1472/features.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/features.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/features.o.d > ${OBJECTDIR}/_ext/1472/features.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/features.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/features.o.tmp ${OBJECTDIR}/_ext/1472/features.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/features.o.tmp}
endif
${OBJECTDIR}/_ext/1472/byte_queue.o: ../byte_queue.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/byte_queue.o.d -o ${OBJECTDIR}/_ext/1472/byte_queue.o ../byte_queue.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/byte_queue.o.d > ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/byte_queue.o.d > ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp ${OBJECTDIR}/_ext/1472/byte_queue.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/byte_queue.o.tmp}
endif
${OBJECTDIR}/_ext/1472/digital.o: ../digital.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/digital.o.d -o ${OBJECTDIR}/_ext/1472/digital.o ../digital.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/digital.o.d > ${OBJECTDIR}/_ext/1472/digital.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/digital.o.tmp ${OBJECTDIR}/_ext/1472/digital.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/digital.o.d > ${OBJECTDIR}/_ext/1472/digital.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/digital.o.tmp ${OBJECTDIR}/_ext/1472/digital.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/digital.o.tmp}
endif
${OBJECTDIR}/_ext/1472/pins.o: ../pins.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/pins.o.d -o ${OBJECTDIR}/_ext/1472/pins.o ../pins.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/pins.o.d > ${OBJECTDIR}/_ext/1472/pins.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pins.o.tmp ${OBJECTDIR}/_ext/1472/pins.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/pins.o.d > ${OBJECTDIR}/_ext/1472/pins.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/pins.o.tmp ${OBJECTDIR}/_ext/1472/pins.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/pins.o.tmp}
endif
${OBJECTDIR}/_ext/1472/protocol.o: ../protocol.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/protocol.o.d -o ${OBJECTDIR}/_ext/1472/protocol.o ../protocol.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/protocol.o.d > ${OBJECTDIR}/_ext/1472/protocol.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/protocol.o.tmp ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/protocol.o.d > ${OBJECTDIR}/_ext/1472/protocol.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/protocol.o.tmp ${OBJECTDIR}/_ext/1472/protocol.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/protocol.o.tmp}
endif
${OBJECTDIR}/_ext/1472/uart.o: ../uart.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/uart.o.d -o ${OBJECTDIR}/_ext/1472/uart.o ../uart.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/uart.o.d > ${OBJECTDIR}/_ext/1472/uart.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/uart.o.tmp ${OBJECTDIR}/_ext/1472/uart.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/uart.o.d > ${OBJECTDIR}/_ext/1472/uart.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/uart.o.tmp ${OBJECTDIR}/_ext/1472/uart.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/uart.o.tmp}
endif
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c 
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
${OBJECTDIR}/_ext/1472/adc.o: ../adc.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/adc.o.d -o ${OBJECTDIR}/_ext/1472/adc.o ../adc.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/adc.o.d > ${OBJECTDIR}/_ext/1472/adc.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adc.o.tmp ${OBJECTDIR}/_ext/1472/adc.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/1472/adc.o.d > ${OBJECTDIR}/_ext/1472/adc.o.tmp
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.d 
	${CP} ${OBJECTDIR}/_ext/1472/adc.o.tmp ${OBJECTDIR}/_ext/1472/adc.o.d 
	${RM} ${OBJECTDIR}/_ext/1472/adc.o.tmp}
endif
${OBJECTDIR}/_ext/131612190/uart2.o: ../../../../mal/Common/uart2.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/131612190 
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/131612190/uart2.o.d -o ${OBJECTDIR}/_ext/131612190/uart2.o ../../../../mal/Common/uart2.c 
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	 sed -e 's/\"//g' -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/131612190/uart2.o.d > ${OBJECTDIR}/_ext/131612190/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp}
else 
	 sed -e 's/\"//g' ${OBJECTDIR}/_ext/131612190/uart2.o.d > ${OBJECTDIR}/_ext/131612190/uart2.o.tmp
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${CP} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp ${OBJECTDIR}/_ext/131612190/uart2.o.d 
	${RM} ${OBJECTDIR}/_ext/131612190/uart2.o.tmp}
endif
${OBJECTDIR}/_ext/1472/logging.o: ../logging.c  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} ${OBJECTDIR}/_ext/1472 
	${RM} ${OBJECTDIR}/_ext/1472/logging.o.d 
	${MP_CC}  -omf=elf -x c -c -mcpu=24FJ128DA106 -Wall -DENABLE_LOGGING -I".." -I"../.." -I"../../blapi" -I"/home/ytai/mal/Include" -O3 -MMD -MF ${OBJECTDIR}/_ext/1472/logging.o.d -o ${OBJECTDIR}/_ext/1472/logging.o ../logging.c 
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
dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC}  -omf=elf  -mcpu=24FJ128DA106  -D__DEBUG -D__MPLAB_DEBUGGER_UNKNOWN=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf ${OBJECTFILES}      -Wl,--defsym=__MPLAB_BUILD=1,--script=../../blapi/app_p24FJ128DA106.gld,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_UNKNOWN=1,--heap=0,-L"../../blapi",-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem
else
dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC}  -omf=elf  -mcpu=24FJ128DA106  -o dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf ${OBJECTFILES}      -Wl,--defsym=__MPLAB_BUILD=1,--script=../../blapi/app_p24FJ128DA106.gld,--heap=0,-L"../../blapi",-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem
	${MP_CC_DIR}/pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/AppLayerV1.X.${IMAGE_TYPE}.elf -omf=elf
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
