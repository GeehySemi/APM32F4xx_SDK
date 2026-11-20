################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Source/apm32f4xx_int.c \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Source/main.c \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Source/sdio.c 

C_DEPS += \
./Application/apm32f4xx_int.d \
./Application/main.d \
./Application/sdio.d 

OBJS += \
./Application/apm32f4xx_int.o \
./Application/main.o \
./Application/sdio.o 


# Each subdirectory must supply rules for building sources it contributes
Application/apm32f4xx_int.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Source/apm32f4xx_int.c Application/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F411xx -DAPM32F411_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards/Board_APM32F411_Tiny/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/main.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Source/main.c Application/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F411xx -DAPM32F411_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards/Board_APM32F411_Tiny/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/sdio.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Source/sdio.c Application/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F411xx -DAPM32F411_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards/Board_APM32F411_Tiny/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Application

clean-Application:
	-$(RM) ./Application/apm32f4xx_int.d ./Application/apm32f4xx_int.o ./Application/main.d ./Application/main.o ./Application/sdio.d ./Application/sdio.o

.PHONY: clean-Application

