################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_flexcomm.c \
../drivers/fsl_gpio.c \
../drivers/fsl_power.c \
../drivers/fsl_reset.c \
../drivers/fsl_usart.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_flexcomm.o \
./drivers/fsl_gpio.o \
./drivers/fsl_power.o \
./drivers/fsl_reset.o \
./drivers/fsl_usart.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_flexcomm.d \
./drivers/fsl_gpio.d \
./drivers/fsl_power.d \
./drivers/fsl_reset.d \
./drivers/fsl_usart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_LPC55S16JBD100=1 -DCPU_LPC55S16JBD100_cm33 -D_DEBUG=1 -DUSB_STACK_BM -DUSB_STACK_USE_DEDICATED_RAM=1 -DFSL_OSA_BM_TASK_ENABLE=0 -DFSL_OSA_BM_TIMER_CONFIG=0 -DSDK_DEBUGCONSOLE=0 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\board" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\source\lpcip3511" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\utilities" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\phy" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\component\uart" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\component\lists" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\component\osa" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\CMSIS" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\class\cdc" -O0 -fno-common -g3 -mno-unaligned-access -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


