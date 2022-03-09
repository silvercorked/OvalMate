################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include usb/device/source/lpcip3511/subdir.mk
-include usb/device/source/subdir.mk
-include usb/device/class/cdc/subdir.mk
-include usb/device/class/subdir.mk
-include startup/subdir.mk
-include source/modules/utils/subdir.mk
-include source/modules/steppers/subdir.mk
-include source/modules/sevenSegmentLED/subdir.mk
-include source/modules/servo/subdir.mk
-include source/modules/mainInclude/subdir.mk
-include source/modules/irSensor/subdir.mk
-include source/modules/buttons/subdir.mk
-include source/modules/USB/subdir.mk
-include source/generated/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/timer/subdir.mk
-include component/serial_manager/subdir.mk
-include component/pwm/subdir.mk
-include component/osa/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Ovalmate_dev
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Ovalmate_dev.axf

# Tool invocations
Ovalmate_dev.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="Ovalmate_dev.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -T Ovalmate_dev_Debug.ld -o "Ovalmate_dev.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Ovalmate_dev.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Ovalmate_dev.axf"; # arm-none-eabi-objcopy -v -O binary "Ovalmate_dev.axf" "Ovalmate_dev.bin" ; # checksum -p LPC55S16 -d "Ovalmate_dev.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
