################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
../bluerobin/BlueRobin_RX_915MHz.lib 


# Each subdirectory must supply rules for building sources it contributes
bluerobin/BlueRobin_RX_915MHz.out: ../bluerobin/BlueRobin_RX_915MHz.lib $(OBJS)
	@echo 'Building file: $<'
	@echo 'Invoking: Linker'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/bin/cl430" --cmd_file="C:\eZ430-Chronos\Software Projects\Chronos Watch\CCS\Sports Watch\simpliciti\Applications\configuration\smpl_nwk_config.dat" --cmd_file="C:\eZ430-Chronos\Software Projects\Chronos Watch\CCS\Sports Watch\simpliciti\Applications\configuration\End Device\smpl_config.dat"  --silicon_version=mspx -g -O3 --define=__CCE__ --define=ISM_US --define=__CC430F6137__ --define=MRFI_CC430 --diag_warning=225 --call_assumptions=0 --gen_opt_info=2 --printf_support=minimal -z -m"ez430_chronos.map" --stack_size=80 --heap_size=80 --use_hw_mpy=F5 --warn_sections -i"C:/Program Files/Texas Instruments/ccsv4/msp430/include" -i"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/lib" -i"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/include" --reread_libs --rom_model -o "$@" "$<" "../lnk_cc430f6137.cmd" $(ORDERED_OBJS)
	@echo 'Finished building: $<'
	@echo ' '


