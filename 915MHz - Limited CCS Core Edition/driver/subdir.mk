################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
../driver/ez430_chronos_drivers.lib 

C_SRCS += \
../driver/buzzer.c 

OBJS += \
./driver/buzzer.obj 

C_DEPS += \
./driver/buzzer.pp 

OBJS__QTD += \
".\driver\buzzer.obj" 

C_DEPS__QTD += \
".\driver\buzzer.pp" 

C_SRCS_QUOTED += \
"../driver/buzzer.c" 


# Each subdirectory must supply rules for building sources it contributes
driver/buzzer.obj: ../driver/buzzer.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/bin/cl430" --silicon_version=mspx -g -O2 --define=__CCE__ --define=ISM_US --include_path="C:/Program Files/Texas Instruments/ccsv4/msp430/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/include" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/include" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/driver" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/logic" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/bluerobin" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti" --diag_warning=225 --call_assumptions=0 --gen_opt_info=2 --printf_support=minimal --preproc_with_compile --preproc_dependency="driver/buzzer.pp" --obj_directory="driver" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


