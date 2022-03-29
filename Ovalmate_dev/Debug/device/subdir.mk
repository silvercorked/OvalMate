################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_LPC55S16.c 

OBJS += \
./device/system_LPC55S16.o 

C_DEPS += \
./device/system_LPC55S16.d 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DPRINTF_FLOAT_ENABLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\drivers" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\serial_manager" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\utilities" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\uart" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\timer" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\pwm" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\lists" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\device\class\hid" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\output\source\device\class" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\device\include" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\device\source" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\device\source\lpcip3511" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\include" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\device\class" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\usb\output\source\device" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\osa" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\source\generated" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\device" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\CMSIS" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\drivers" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\serial_manager" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\utilities" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\uart" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\timer" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\pwm" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\component\lists" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\device" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\CMSIS" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\board" -I"C:\Users\Alex\Documents\MCUXpressoIDE_11.5.0_7232\workspace\OvalMate\Ovalmate_dev\source" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


