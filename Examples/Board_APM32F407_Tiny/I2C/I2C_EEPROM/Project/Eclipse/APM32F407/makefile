################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include StdPeriphDriver/subdir.mk
-include CMSIS/subdir.mk
-include Board/subdir.mk
-include Application/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := I2C_EEPROM
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
I2C_EEPROM.hex \

SECONDARY_SIZE += \
I2C_EEPROM.siz \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: I2C_EEPROM.elf secondary-outputs

# Tool invocations
I2C_EEPROM.elf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Arm Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wextra  -g3 -T "E:/workspace/project/APM32F4xx/SDK/APM32F4xx_SDK/Examples/Board_APM32F407_Tiny/I2C/I2C_EEPROM/Project/Eclipse/apm32f407xg_flash.ld" -Xlinker --gc-sections -Wl,-Map,"I2C_EEPROM.map" --specs=nano.specs -u _printf_float --specs=nosys.specs -o "I2C_EEPROM.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

I2C_EEPROM.hex: I2C_EEPROM.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU Arm Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "I2C_EEPROM.elf"  "I2C_EEPROM.hex"
	@echo 'Finished building: $@'
	@echo ' '

I2C_EEPROM.siz: I2C_EEPROM.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU Arm Cross Print Size'
	arm-none-eabi-size --format=berkeley "I2C_EEPROM.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) I2C_EEPROM.elf I2C_EEPROM.hex I2C_EEPROM.siz
	-@echo ' '

post-build:
	-arm-none-eabi-objcopy -O binary "I2C_EEPROM.elf" "I2C_EEPROM.bin"; arm-none-eabi-objdump -D "I2C_EEPROM.elf" > "I2C_EEPROM.dump"
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
