################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include usb/phy/subdir.mk
-include usb/device/source/lpcip3511/subdir.mk
-include usb/device/source/subdir.mk
-include usb/device/class/video/subdir.mk
-include usb/device/class/printer/subdir.mk
-include usb/device/class/phdc/subdir.mk
-include usb/device/class/msc/subdir.mk
-include usb/device/class/hid/subdir.mk
-include usb/device/class/dfu/subdir.mk
-include usb/device/class/cdc_rndis/subdir.mk
-include usb/device/class/cdc/subdir.mk
-include usb/device/class/ccid/subdir.mk
-include usb/device/class/audio/subdir.mk
-include usb/device/class/subdir.mk
-include startup/subdir.mk
-include source/modules/utils/subdir.mk
-include source/modules/steppers/subdir.mk
-include source/modules/sevenSegmentLED/subdir.mk
-include source/modules/servo/subdir.mk
-include source/modules/mainInclude/subdir.mk
-include source/modules/irSensor/subdir.mk
-include source/modules/buttons/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/timer_manager/subdir.mk
-include component/timer/subdir.mk
-include component/serial_manager/usb_cdc_adapter/subdir.mk
-include component/serial_manager/subdir.mk
-include component/pwm/subdir.mk
-include component/osa/subdir.mk
-include component/mem_manager/subdir.mk
-include component/lists/subdir.mk
-include component/gpio/subdir.mk
-include board/subdir.mk
-include LPC55S16/drivers/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LPC55S16_Project_main_dev.axf

# Tool invocations
LPC55S16_Project_main_dev.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\LPC55S16\mcuxpresso" -L"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\CMSIS\DSP" -Xlinker -Map="LPC55S16_Project_main_dev.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -T LPC55S16_Project_main_dev_Debug.ld -o "LPC55S16_Project_main_dev.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) LPC55S16_Project_main_dev.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LPC55S16_Project_main_dev.axf"; # arm-none-eabi-objcopy -v -O binary "LPC55S16_Project_main_dev.axf" "LPC55S16_Project_main_dev.bin" ; # checksum -p LPC55S16 -d "LPC55S16_Project_main_dev.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
