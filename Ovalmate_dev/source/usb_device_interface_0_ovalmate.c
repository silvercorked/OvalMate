/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_hid.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#include "usb_device_composite.h"

#include <stdbool.h>
#include <modules/blackDots/blackDots.h>
#include <modules/sevenSegmentLED/sevenSegmentLED.h>

#include <stdio.h>
#include "fsl_debug_console.h"  // this gives access to PRINTF for debugging

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct _usb_hid_generic_struct
{
    uint8_t *inBuffer;
    uint8_t *outBuffer;
    uint8_t idleRate;
} usb_hid_generic_struct_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static usb_status_t USB_DeviceHidGenericAction(void);
usb_status_t USB_DeviceInterface0OvalmateCallback(class_handle_t handle, uint32_t event, void *param);
usb_status_t USB_DeviceInterface0OvalmateSetConfiguration(class_handle_t handle, uint8_t configuration_idx);
usb_status_t USB_DeviceInterface0OvalmateSetInterface(class_handle_t handle, uint8_t alternateSetting);
usb_status_t USB_DeviceInterface0OvalmateInit(usb_device_composite_struct_t *deviceComposite);


/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericInBuffer[USB_INTERFACE_0_OVALMATE_INPUT_REPORT_LENGTH];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_GenericOutBuffer[USB_INTERFACE_0_OVALMATE_OUTPUT_REPORT_LENGTH];
static usb_device_composite_struct_t *s_UsbDeviceComposite;
static usb_hid_generic_struct_t s_UsbDeviceHidGeneric;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
* @brief Sends received data and prepares for the next IN transaction.
*
* @return usb_status_t Status of USB transaction.
*/
static usb_status_t USB_DeviceHidGenericAction(void)
{
    for (int i = 0; (i < USB_INTERFACE_0_OVALMATE_OUTPUT_REPORT_LENGTH) && (i < USB_INTERFACE_0_OVALMATE_INPUT_REPORT_LENGTH); i++)

    {
        s_UsbDeviceHidGeneric.inBuffer[i] = s_UsbDeviceHidGeneric.outBuffer[i];
    }
    uint8_t x = s_UsbDeviceHidGeneric.outBuffer[0]; uint8_t y = s_UsbDeviceHidGeneric.outBuffer[1];
    PRINTF("\r\n got (%d, %d)", x, y);
   if (!jobReady) { // dont wanna modify while running
	   if (x <= BLACKDOTS_MAXCOLS && y <= BLACKDOTS_MAXROWS) {
		BLACKDOTS_setPoint(x, y);
		PRINTF("\r\n got (%d, %d)", x, y);
	  }
	  else if (x == 127 && y == 127) {
		jobReady = true;
		PRINTF("\r\n got (%d, %d) USBJOB COMPLETE", x, y);
	  }
	  else {
		PRINTF("\r\n got (%d, %d) doesn't fit", x, y);
	  }
   }

    USB_DeviceHidSend(s_UsbDeviceComposite->interface0OvalmateHandle, USB_INTERFACE_0_OVALMATE_SETTING_0_EP_1_INTERRUPT_IN,
        (uint8_t *)s_UsbDeviceHidGeneric.inBuffer,
        USB_INTERFACE_0_OVALMATE_INPUT_REPORT_LENGTH);

    return USB_DeviceHidRecv(s_UsbDeviceComposite->interface0OvalmateHandle, USB_INTERFACE_0_OVALMATE_SETTING_0_EP_1_INTERRUPT_OUT,
        (uint8_t *)s_UsbDeviceHidGeneric.outBuffer,
        USB_INTERFACE_0_OVALMATE_OUTPUT_REPORT_LENGTH);
}

/*!
* @brief Function that processes class specific events.
*
* @param handle Handle to USB device class.
* @param event Class event code.
* @param param The parameter of the class specific event.
* @retur usb_status_t Status of USB transaction.
*/
usb_status_t USB_DeviceInterface0OvalmateCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_InvalidRequest;
    PRINTF("got USB event, %d", event);
    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            break;
        case kUSB_DeviceHidEventRecvResponse:
            if (s_UsbDeviceComposite->attach)
            {
                return USB_DeviceHidGenericAction();
            }
            break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        case kUSB_DeviceHidEventRequestReportBuffer:
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            error = kStatus_USB_Success;
            break;
        default:
            break;
    }

    return error;
}

/*!
* @brief Notifies application layer about set configuration event.
*
* @param handle Handle to USB device class.
* @param configuration_idx Id of device configuration.
* @return usb_status_t Status of USB transaction.
*/
usb_status_t USB_DeviceInterface0OvalmateSetConfiguration(class_handle_t handle, uint8_t configuration_idx)
{
   usb_status_t error = USB_DeviceHidRecv(s_UsbDeviceComposite->interface0OvalmateHandle, USB_INTERFACE_0_OVALMATE_SETTING_0_EP_1_INTERRUPT_OUT,
        (uint8_t *)s_UsbDeviceHidGeneric.outBuffer,
        USB_INTERFACE_0_OVALMATE_OUTPUT_REPORT_LENGTH);
   return error;
}

/*!
* @brief Notifies application layer about set configuration event.
*
* @param class_handle_t Handle to USB device class.
* @param alternateSetting Id of device alternative setting.
* @return usb_status_t Status of USB transaction.
*/
usb_status_t USB_DeviceInterface0OvalmateSetInterface(class_handle_t handle, uint8_t alternateSetting)
{
   usb_status_t error = USB_DeviceHidRecv(s_UsbDeviceComposite->interface0OvalmateHandle, USB_INTERFACE_0_OVALMATE_SETTING_0_EP_1_INTERRUPT_OUT,
        (uint8_t *)s_UsbDeviceHidGeneric.outBuffer,
        USB_INTERFACE_0_OVALMATE_OUTPUT_REPORT_LENGTH);
   return error;
}

/*!
* @brief Initializes device structure and buffer pointers.
*
* @param *device Pointer to structure to initialize to.
* @return usb_status_t Always return kStatus_USB_Success value.
*/
usb_status_t USB_DeviceInterface0OvalmateInit(usb_device_composite_struct_t *deviceComposite)
{
    s_UsbDeviceComposite = deviceComposite;
    s_UsbDeviceHidGeneric.inBuffer = s_GenericInBuffer;
    s_UsbDeviceHidGeneric.outBuffer = s_GenericOutBuffer;
    return kStatus_USB_Success;
}
