################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/startup_lpc55s16.c 

OBJS += \
./startup/startup_lpc55s16.o 

C_DEPS += \
./startup/startup_lpc55s16.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c startup/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DPRINTF_FLOAT_ENABLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\board" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\component\serial_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\utilities" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\component\uart" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\component\timer" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\component\pwm" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\component\lists" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\CMSIS" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev_noUSB\device" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


