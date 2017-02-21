################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/level0.c \
../src/level1.c \
../src/level2.c \
../src/main.c \
../src/stm32f0xx_it.c \
../src/system_stm32f0xx.c \
../src/usart1.c 

OBJS += \
./src/level0.o \
./src/level1.o \
./src/level2.o \
./src/main.o \
./src/stm32f0xx_it.o \
./src/system_stm32f0xx.o \
./src/usart1.o 

C_DEPS += \
./src/level0.d \
./src/level1.d \
./src/level2.d \
./src/main.d \
./src/stm32f0xx_it.d \
./src/system_stm32f0xx.d \
./src/usart1.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -std=c99 -DROUTER -DSTM32F030F4Px -DSTM32F0 -DSTM32 -DUSE_STDPERIPH_DRIVER -DSTM32F030 -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/inc" -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/CMSIS/core" -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/CMSIS/device" -I"/Volumes/UserWzl/workplace/eclipse/STM32/wzl-WZLoRa/StdPeriph_Driver/inc" -Os -Wall -fmessage-length=0 -ffunction-sections -fdata-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


