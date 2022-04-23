################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/composite.c \
../source/semihost_hardfault.c \
../source/usb_device_descriptor.c \
../source/virtual_com.c 

OBJS += \
./source/composite.o \
./source/semihost_hardfault.o \
./source/usb_device_descriptor.o \
./source/virtual_com.o 

C_DEPS += \
./source/composite.d \
./source/semihost_hardfault.d \
./source/usb_device_descriptor.d \
./source/virtual_com.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_LPC55S16JBD100=1 -DCPU_LPC55S16JBD100_cm33 -D_DEBUG=1 -DUSB_STACK_BM -DUSB_STACK_USE_DEDICATED_RAM=1 -DFSL_OSA_BM_TASK_ENABLE=0 -DFSL_OSA_BM_TIMER_CONFIG=0 -DSDK_DEBUGCONSOLE=0 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\board" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\source\lpcip3511" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\utilities" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\phy" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\component\uart" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\component\lists" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\component\osa" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\CMSIS" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.5.0_7232\workspace\LCP55S16-evk\lpcxpresso55s16_dev_composite_cdc_vcom_cdc_vcom_bm\usb\device\class\cdc" -O0 -fno-common -g3 -mno-unaligned-access -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

