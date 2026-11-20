################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Source/system_apm32f4xx.c 

S_UPPER_SRCS += \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Libraries/Device/Geehy/APM32F4xx/Source/gcc/startup_apm32f407xx.S 

C_DEPS += \
./CMSIS/system_apm32f4xx.d 

OBJS += \
./CMSIS/startup_apm32f407xx.o \
./CMSIS/system_apm32f4xx.o 

S_UPPER_DEPS += \
./CMSIS/startup_apm32f407xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/startup_apm32f407xx.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Libraries/Device/Geehy/APM32F4xx/Source/gcc/startup_apm32f407xx.S CMSIS/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/system_apm32f4xx.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Source/system_apm32f4xx.c CMSIS/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F407xx -DAPM32F407_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-CMSIS

clean-CMSIS:
	-$(RM) ./CMSIS/startup_apm32f407xx.d ./CMSIS/startup_apm32f407xx.o ./CMSIS/system_apm32f4xx.d ./CMSIS/system_apm32f4xx.o

.PHONY: clean-CMSIS

