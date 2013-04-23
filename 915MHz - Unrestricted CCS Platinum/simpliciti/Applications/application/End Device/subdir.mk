################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../simpliciti/Applications/application/End\ Device/main_ED_BM.c 

OBJS += \
./simpliciti/Applications/application/End\ Device/main_ED_BM.obj 

C_DEPS += \
./simpliciti/Applications/application/End\ Device/main_ED_BM.pp 

OBJS__QTD += \
".\simpliciti\Applications\application\End Device\main_ED_BM.obj" 

C_DEPS__QTD += \
".\simpliciti\Applications\application\End Device\main_ED_BM.pp" 

C_SRCS_QUOTED += \
"../simpliciti/Applications/application/End Device/main_ED_BM.c" 


# Each subdirectory must supply rules for building sources it contributes
simpliciti/Applications/application/End\ Device/main_ED_BM.obj: ../simpliciti/Applications/application/End\ Device/main_ED_BM.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/bin/cl430" --cmd_file="C:\eZ430-Chronos\Software Projects\Chronos Watch\CCS\Sports Watch\simpliciti\Applications\configuration\smpl_nwk_config.dat" --cmd_file="C:\eZ430-Chronos\Software Projects\Chronos Watch\CCS\Sports Watch\simpliciti\Applications\configuration\End Device\smpl_config.dat"  --silicon_version=mspx -g -O3 --define=__CCE__ --define=ISM_US --define=__CC430F6137__ --define=MRFI_CC430 --include_path="C:/Program Files/Texas Instruments/ccsv4/msp430/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/include" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/include" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/driver" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/logic" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/bluerobin" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Applications/application/End Device" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/bsp" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/bsp/boards" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/bsp/boards/CC430EM" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/bsp/boards/CC430EM/bsp_external" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/bsp/drivers" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/bsp/drivers/code" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/bsp/mcus" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/mrfi" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/mrfi/radios" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/mrfi/radios/family5" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/mrfi/smartrf" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/nwk" --include_path="C:/eZ430-Chronos/Software Projects/Chronos Watch/CCS/Sports Watch/simpliciti/Components/nwk_applications" --diag_warning=225 --call_assumptions=0 --gen_opt_info=2 --printf_support=minimal --preproc_with_compile --preproc_dependency="simpliciti/Applications/application/End Device/main_ED_BM.pp" --obj_directory="simpliciti/Applications/application/End Device" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


