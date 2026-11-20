################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board.c 

C_DEPS += \
./Board/Board.d 

OBJS += \
./Board/Board.o 


# Each subdirectory must supply rules for building sources it contributes
Board/Board.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board.c Board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F407xx -DAPM32F407_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Board

clean-Board:
	-$(RM) ./Board/Board.d ./Board/Board.o

.PHONY: clean-Board

