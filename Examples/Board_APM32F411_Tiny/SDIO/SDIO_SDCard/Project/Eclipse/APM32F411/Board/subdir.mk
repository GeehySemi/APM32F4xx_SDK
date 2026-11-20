################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board.c \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board_APM32F411_Tiny/src/board_delay.c \
E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board_APM32F411_Tiny/src/log_printf.c 

C_DEPS += \
./Board/Board.d \
./Board/board_delay.d \
./Board/log_printf.d 

OBJS += \
./Board/Board.o \
./Board/board_delay.o \
./Board/log_printf.o 


# Each subdirectory must supply rules for building sources it contributes
Board/Board.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board.c Board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F411xx -DAPM32F411_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards/Board_APM32F411_Tiny/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Board/board_delay.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board_APM32F411_Tiny/src/board_delay.c Board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F411xx -DAPM32F411_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards/Board_APM32F411_Tiny/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Board/log_printf.o: E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Boards/Board_APM32F411_Tiny/src/log_printf.c Board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -DAPM32F411xx -DAPM32F411_TINY -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Boards/Board_APM32F411_Tiny/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/APM32F4xx_StdPeriphDriver/inc" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/CMSIS/Include" -I"E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F411_Tiny/SDIO/SDIO_SDCard/Project/Eclipse/../../../../../../Libraries/Device/Geehy/APM32F4xx/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Board

clean-Board:
	-$(RM) ./Board/Board.d ./Board/Board.o ./Board/board_delay.d ./Board/board_delay.o ./Board/log_printf.d ./Board/log_printf.o

.PHONY: clean-Board

