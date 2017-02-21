################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f0xx.s 

OBJS += \
./startup/startup_stm32f0xx.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/inc" -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/CMSIS/core" -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/CMSIS/device" -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/StdPeriph_Driver/inc" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


