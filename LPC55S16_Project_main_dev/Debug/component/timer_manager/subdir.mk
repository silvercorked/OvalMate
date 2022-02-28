################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/timer_manager/fsl_component_timer_manager.c 

OBJS += \
./component/timer_manager/fsl_component_timer_manager.o 

C_DEPS += \
./component/timer_manager/fsl_component_timer_manager.d 


# Each subdirectory must supply rules for building sources it contributes
component/timer_manager/%.o: ../component/timer_manager/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_USBCDC=1 -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING -DUSB_DEVICE_CONFIG_CDC_ACM=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\CMSIS_driver" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\LPC55S16\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\CMSIS\DSP\Include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\serial_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\utilities" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\uart" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\serial_manager\usb_cdc_adapter" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\audio" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\output\source\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\ccid" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\cdc" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\cdc_rndis" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\dfu" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\hid" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\source\lpcip3511" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\msc" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\phdc" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\printer" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\output\source\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\video" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\phy" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\timer" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\pwm" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\gpio" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\lists" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\osa" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\timer_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\mem_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\board" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\source\generated" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\CMSIS" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\board" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\CMSIS_driver" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\source\generated" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\LPC55S16\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\drivers" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\CMSIS\DSP\Include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\serial_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\utilities" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\uart" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\serial_manager\usb_cdc_adapter" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\audio" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\output\source\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\ccid" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\cdc" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\cdc_rndis" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\include" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\source" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\dfu" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\hid" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\source\lpcip3511" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\msc" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\phdc" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\printer" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\output\source\device" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\device\class\video" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\usb\phy" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\timer" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\pwm" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\gpio" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\lists" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\osa" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\timer_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\component\mem_manager" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\CMSIS" -I"C:\Users\awiss\Documents\MCUXpressoIDE_11.4.1_6260\workspace\OvalMate\LPC55S16_Project_main_dev\device" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


