################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_anactrl.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_ctimer.c \
../drivers/fsl_flexcomm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_i2c.c \
../drivers/fsl_inputmux.c \
../drivers/fsl_lpadc.c \
../drivers/fsl_pint.c \
../drivers/fsl_power.c \
../drivers/fsl_reset.c \
../drivers/fsl_spi.c \
../drivers/fsl_usart.c 

OBJS += \
./drivers/fsl_anactrl.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_ctimer.o \
./drivers/fsl_flexcomm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_i2c.o \
./drivers/fsl_inputmux.o \
./drivers/fsl_lpadc.o \
./drivers/fsl_pint.o \
./drivers/fsl_power.o \
./drivers/fsl_reset.o \
./drivers/fsl_spi.o \
./drivers/fsl_usart.o 

C_DEPS += \
./drivers/fsl_anactrl.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_ctimer.d \
./drivers/fsl_flexcomm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_i2c.d \
./drivers/fsl_inputmux.d \
./drivers/fsl_lpadc.d \
./drivers/fsl_pint.d \
./drivers/fsl_power.d \
./drivers/fsl_reset.d \
./drivers/fsl_spi.d \
./drivers/fsl_usart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DPRINTF_FLOAT_ENABLE=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\serial_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\utilities" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\uart" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\timer" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\pwm" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\lists" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\class\hid" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\output\source\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\source\lpcip3511" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\output\source\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\osa" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\source\generated" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\CMSIS" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\serial_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\utilities" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\uart" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\timer" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\pwm" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\lists" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\class\hid" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\output\source\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\source\lpcip3511" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\usb\output\source\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\component\osa" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\source\generated" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\CMSIS" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\board" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\Ovalmate_dev\source" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


