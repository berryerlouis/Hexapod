#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-Release.mk)" "nbproject/Makefile-local-Release.mk"
include nbproject/Makefile-local-Release.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Release
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=Drv/DrvAdc.c Drv/DrvButton.c Drv/DrvEeprom.c Drv/DrvIo.c Drv/DrvLed.c Drv/DrvServo.c Drv/DrvTick.c Drv/DrvTimer.c Drv/DrvTwi.c Drv/DrvUart.c Srv/SrvComm.c Srv/SrvIhm.c main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/Drv/DrvAdc.o ${OBJECTDIR}/Drv/DrvButton.o ${OBJECTDIR}/Drv/DrvEeprom.o ${OBJECTDIR}/Drv/DrvIo.o ${OBJECTDIR}/Drv/DrvLed.o ${OBJECTDIR}/Drv/DrvServo.o ${OBJECTDIR}/Drv/DrvTick.o ${OBJECTDIR}/Drv/DrvTimer.o ${OBJECTDIR}/Drv/DrvTwi.o ${OBJECTDIR}/Drv/DrvUart.o ${OBJECTDIR}/Srv/SrvComm.o ${OBJECTDIR}/Srv/SrvIhm.o ${OBJECTDIR}/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/Drv/DrvAdc.o.d ${OBJECTDIR}/Drv/DrvButton.o.d ${OBJECTDIR}/Drv/DrvEeprom.o.d ${OBJECTDIR}/Drv/DrvIo.o.d ${OBJECTDIR}/Drv/DrvLed.o.d ${OBJECTDIR}/Drv/DrvServo.o.d ${OBJECTDIR}/Drv/DrvTick.o.d ${OBJECTDIR}/Drv/DrvTimer.o.d ${OBJECTDIR}/Drv/DrvTwi.o.d ${OBJECTDIR}/Drv/DrvUart.o.d ${OBJECTDIR}/Srv/SrvComm.o.d ${OBJECTDIR}/Srv/SrvIhm.o.d ${OBJECTDIR}/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/Drv/DrvAdc.o ${OBJECTDIR}/Drv/DrvButton.o ${OBJECTDIR}/Drv/DrvEeprom.o ${OBJECTDIR}/Drv/DrvIo.o ${OBJECTDIR}/Drv/DrvLed.o ${OBJECTDIR}/Drv/DrvServo.o ${OBJECTDIR}/Drv/DrvTick.o ${OBJECTDIR}/Drv/DrvTimer.o ${OBJECTDIR}/Drv/DrvTwi.o ${OBJECTDIR}/Drv/DrvUart.o ${OBJECTDIR}/Srv/SrvComm.o ${OBJECTDIR}/Srv/SrvIhm.o ${OBJECTDIR}/main.o

# Source Files
SOURCEFILES=Drv/DrvAdc.c Drv/DrvButton.c Drv/DrvEeprom.c Drv/DrvIo.c Drv/DrvLed.c Drv/DrvServo.c Drv/DrvTick.c Drv/DrvTimer.c Drv/DrvTwi.c Drv/DrvUart.c Srv/SrvComm.c Srv/SrvIhm.c main.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-Release.mk dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATmega1284P
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Drv/DrvAdc.o: Drv/DrvAdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvAdc.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvAdc.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvAdc.o.d" -MT "${OBJECTDIR}/Drv/DrvAdc.o.d" -MT ${OBJECTDIR}/Drv/DrvAdc.o  -o ${OBJECTDIR}/Drv/DrvAdc.o Drv/DrvAdc.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvButton.o: Drv/DrvButton.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvButton.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvButton.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvButton.o.d" -MT "${OBJECTDIR}/Drv/DrvButton.o.d" -MT ${OBJECTDIR}/Drv/DrvButton.o  -o ${OBJECTDIR}/Drv/DrvButton.o Drv/DrvButton.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvEeprom.o: Drv/DrvEeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvEeprom.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvEeprom.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvEeprom.o.d" -MT "${OBJECTDIR}/Drv/DrvEeprom.o.d" -MT ${OBJECTDIR}/Drv/DrvEeprom.o  -o ${OBJECTDIR}/Drv/DrvEeprom.o Drv/DrvEeprom.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvIo.o: Drv/DrvIo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvIo.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvIo.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvIo.o.d" -MT "${OBJECTDIR}/Drv/DrvIo.o.d" -MT ${OBJECTDIR}/Drv/DrvIo.o  -o ${OBJECTDIR}/Drv/DrvIo.o Drv/DrvIo.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvLed.o: Drv/DrvLed.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvLed.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvLed.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvLed.o.d" -MT "${OBJECTDIR}/Drv/DrvLed.o.d" -MT ${OBJECTDIR}/Drv/DrvLed.o  -o ${OBJECTDIR}/Drv/DrvLed.o Drv/DrvLed.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvServo.o: Drv/DrvServo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvServo.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvServo.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvServo.o.d" -MT "${OBJECTDIR}/Drv/DrvServo.o.d" -MT ${OBJECTDIR}/Drv/DrvServo.o  -o ${OBJECTDIR}/Drv/DrvServo.o Drv/DrvServo.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvTick.o: Drv/DrvTick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvTick.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvTick.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvTick.o.d" -MT "${OBJECTDIR}/Drv/DrvTick.o.d" -MT ${OBJECTDIR}/Drv/DrvTick.o  -o ${OBJECTDIR}/Drv/DrvTick.o Drv/DrvTick.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvTimer.o: Drv/DrvTimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvTimer.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvTimer.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvTimer.o.d" -MT "${OBJECTDIR}/Drv/DrvTimer.o.d" -MT ${OBJECTDIR}/Drv/DrvTimer.o  -o ${OBJECTDIR}/Drv/DrvTimer.o Drv/DrvTimer.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvTwi.o: Drv/DrvTwi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvTwi.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvTwi.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvTwi.o.d" -MT "${OBJECTDIR}/Drv/DrvTwi.o.d" -MT ${OBJECTDIR}/Drv/DrvTwi.o  -o ${OBJECTDIR}/Drv/DrvTwi.o Drv/DrvTwi.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvUart.o: Drv/DrvUart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvUart.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvUart.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvUart.o.d" -MT "${OBJECTDIR}/Drv/DrvUart.o.d" -MT ${OBJECTDIR}/Drv/DrvUart.o  -o ${OBJECTDIR}/Drv/DrvUart.o Drv/DrvUart.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Srv/SrvComm.o: Srv/SrvComm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Srv" 
	@${RM} ${OBJECTDIR}/Srv/SrvComm.o.d 
	@${RM} ${OBJECTDIR}/Srv/SrvComm.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Srv/SrvComm.o.d" -MT "${OBJECTDIR}/Srv/SrvComm.o.d" -MT ${OBJECTDIR}/Srv/SrvComm.o  -o ${OBJECTDIR}/Srv/SrvComm.o Srv/SrvComm.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Srv/SrvIhm.o: Srv/SrvIhm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Srv" 
	@${RM} ${OBJECTDIR}/Srv/SrvIhm.o.d 
	@${RM} ${OBJECTDIR}/Srv/SrvIhm.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Srv/SrvIhm.o.d" -MT "${OBJECTDIR}/Srv/SrvIhm.o.d" -MT ${OBJECTDIR}/Srv/SrvIhm.o  -o ${OBJECTDIR}/Srv/SrvIhm.o Srv/SrvIhm.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p" -g -DDEBUG  -gdwarf-2  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/Drv/DrvAdc.o: Drv/DrvAdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvAdc.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvAdc.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvAdc.o.d" -MT "${OBJECTDIR}/Drv/DrvAdc.o.d" -MT ${OBJECTDIR}/Drv/DrvAdc.o  -o ${OBJECTDIR}/Drv/DrvAdc.o Drv/DrvAdc.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvButton.o: Drv/DrvButton.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvButton.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvButton.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvButton.o.d" -MT "${OBJECTDIR}/Drv/DrvButton.o.d" -MT ${OBJECTDIR}/Drv/DrvButton.o  -o ${OBJECTDIR}/Drv/DrvButton.o Drv/DrvButton.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvEeprom.o: Drv/DrvEeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvEeprom.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvEeprom.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvEeprom.o.d" -MT "${OBJECTDIR}/Drv/DrvEeprom.o.d" -MT ${OBJECTDIR}/Drv/DrvEeprom.o  -o ${OBJECTDIR}/Drv/DrvEeprom.o Drv/DrvEeprom.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvIo.o: Drv/DrvIo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvIo.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvIo.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvIo.o.d" -MT "${OBJECTDIR}/Drv/DrvIo.o.d" -MT ${OBJECTDIR}/Drv/DrvIo.o  -o ${OBJECTDIR}/Drv/DrvIo.o Drv/DrvIo.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvLed.o: Drv/DrvLed.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvLed.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvLed.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvLed.o.d" -MT "${OBJECTDIR}/Drv/DrvLed.o.d" -MT ${OBJECTDIR}/Drv/DrvLed.o  -o ${OBJECTDIR}/Drv/DrvLed.o Drv/DrvLed.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvServo.o: Drv/DrvServo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvServo.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvServo.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvServo.o.d" -MT "${OBJECTDIR}/Drv/DrvServo.o.d" -MT ${OBJECTDIR}/Drv/DrvServo.o  -o ${OBJECTDIR}/Drv/DrvServo.o Drv/DrvServo.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvTick.o: Drv/DrvTick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvTick.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvTick.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvTick.o.d" -MT "${OBJECTDIR}/Drv/DrvTick.o.d" -MT ${OBJECTDIR}/Drv/DrvTick.o  -o ${OBJECTDIR}/Drv/DrvTick.o Drv/DrvTick.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvTimer.o: Drv/DrvTimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvTimer.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvTimer.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvTimer.o.d" -MT "${OBJECTDIR}/Drv/DrvTimer.o.d" -MT ${OBJECTDIR}/Drv/DrvTimer.o  -o ${OBJECTDIR}/Drv/DrvTimer.o Drv/DrvTimer.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvTwi.o: Drv/DrvTwi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvTwi.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvTwi.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvTwi.o.d" -MT "${OBJECTDIR}/Drv/DrvTwi.o.d" -MT ${OBJECTDIR}/Drv/DrvTwi.o  -o ${OBJECTDIR}/Drv/DrvTwi.o Drv/DrvTwi.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Drv/DrvUart.o: Drv/DrvUart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Drv" 
	@${RM} ${OBJECTDIR}/Drv/DrvUart.o.d 
	@${RM} ${OBJECTDIR}/Drv/DrvUart.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Drv/DrvUart.o.d" -MT "${OBJECTDIR}/Drv/DrvUart.o.d" -MT ${OBJECTDIR}/Drv/DrvUart.o  -o ${OBJECTDIR}/Drv/DrvUart.o Drv/DrvUart.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Srv/SrvComm.o: Srv/SrvComm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Srv" 
	@${RM} ${OBJECTDIR}/Srv/SrvComm.o.d 
	@${RM} ${OBJECTDIR}/Srv/SrvComm.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Srv/SrvComm.o.d" -MT "${OBJECTDIR}/Srv/SrvComm.o.d" -MT ${OBJECTDIR}/Srv/SrvComm.o  -o ${OBJECTDIR}/Srv/SrvComm.o Srv/SrvComm.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/Srv/SrvIhm.o: Srv/SrvIhm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Srv" 
	@${RM} ${OBJECTDIR}/Srv/SrvIhm.o.d 
	@${RM} ${OBJECTDIR}/Srv/SrvIhm.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/Srv/SrvIhm.o.d" -MT "${OBJECTDIR}/Srv/SrvIhm.o.d" -MT ${OBJECTDIR}/Srv/SrvIhm.o  -o ${OBJECTDIR}/Srv/SrvIhm.o Srv/SrvIhm.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	 ${MP_CC}  $(MP_EXTRA_CC_PRE) -mmcu=atmega1284p  -I "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/include" -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -x c -c -D__$(MP_PROCESSOR_OPTION)__  -funsigned-char -funsigned-bitfields -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -DNDEBUG -Wall -MD -MP -MF "${OBJECTDIR}/main.o.d" -MT "${OBJECTDIR}/main.o.d" -MT ${OBJECTDIR}/main.o  -o ${OBJECTDIR}/main.o main.c  -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=atmega1284p -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"   -gdwarf-2 -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="dist\${CND_CONF}\${IMAGE_TYPE}\hexapod.${IMAGE_TYPE}.map"    -o dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1 -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,-lm -Wl,--end-group 
	
	${MP_CC_DIR}\\avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.eep" || exit 0
	${MP_CC_DIR}\\avr-objdump -h -S "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.lss"
	${MP_CC_DIR}\\avr-objcopy -O srec -R .eeprom -R .fuse -R .lock -R .signature "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.srec"
	
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mmcu=atmega1284p -B "C:/Program Files (x86)/Atmel/Studio/7.0/packs/atmel/ATmega_DFP/1.2.209/gcc/dev/atmega1284p"  -D__$(MP_PROCESSOR_OPTION)__  -Wl,-Map="dist\${CND_CONF}\${IMAGE_TYPE}\hexapod.${IMAGE_TYPE}.map"    -o dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}      -DXPRJ_Release=$(CND_CONF)  $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION) -Wl,--gc-sections -Wl,--start-group  -Wl,-lm -Wl,-lm -Wl,--end-group 
	${MP_CC_DIR}\\avr-objcopy -O ihex "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.hex"
	${MP_CC_DIR}\\avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.eep" || exit 0
	${MP_CC_DIR}\\avr-objdump -h -S "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" > "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.lss"
	${MP_CC_DIR}\\avr-objcopy -O srec -R .eeprom -R .fuse -R .lock -R .signature "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}" "dist/${CND_CONF}/${IMAGE_TYPE}/hexapod.${IMAGE_TYPE}.srec"
	
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} -r dist/Release

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
