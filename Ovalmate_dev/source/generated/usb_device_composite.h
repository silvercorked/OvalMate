/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef __USB_DEVICE_COMPOSITE_H__
#define __USB_DEVICE_COMPOSITE_H__

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#if (defined(USB_DEVICE_CHARGER_DETECT_ENABLE) && (USB_DEVICE_CHARGER_DETECT_ENABLE > 0U))
#include "usb_device_hid.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

 /*! @brief USB controller ID */
#define USB_DEVICE_CONTROLLER_ID kUSB_ControllerLpcIp3511Fs0
/*! @brief USB interrupt priority ID */
#define USB_DEVICE_INTERRUPT_PRIORITY (3U)

typedef enum _usb_power_status
{
    kStatus_Idle = 0U,
    kStatus_StartSuspend,
    kStatus_Suspending,
    kStatus_Suspended,
    kStatus_StartResume,
    kStatus_Resuming,
    kStatus_Resumed,
} usb_power_status_t;

#if (defined(USB_DEVICE_CONFIG_CHARGER_DETECT) && (USB_DEVICE_CONFIG_CHARGER_DETECT > 0U))
/*! @brief USB DCD charging detect status */
typedef enum _usb_device_dcd_dev_status
{
    kUSB_DeviceDCDDectionInit = 0x0U,
    kUSB_DeviceDCDDectionError,
    kUSB_DeviceDCDDectionTimeOut,
    kUSB_DeviceDCDDectionSDP,
    kUSB_DeviceDCDDectionCDP,
    kUSB_DeviceDCDDectionDCP,
    kUSB_DeviceDCDDectionFinished,
} usb_device_dcd_dev_status_t;
#endif

/*!
 * @brief Structure containing device handle, hadle of interfaces and information on curren configuration, alternate setting, speed and attachment status.
 *
 */
typedef struct _usb_device_composite_struct
{
    usb_device_handle deviceHandle;
    class_handle_t interface0OvalmateHandle;
    uint8_t currentConfiguration; /*Current configuration number*/
    uint8_t currentInterfaceAlternateSetting[USB_COMPOSITE_INTERFACE_COUNT]; /*alternate setting number*/
    uint8_t speed;    /*USB speed code, one of the following: USB_SPEED_FULL(0x00U),USB_SPEED_LOW(0x01U),USB_SPEED_HIGH(0x02U)*/
    volatile uint8_t attach;    /*status of device attachment*/
    volatile uint64_t hwTick;
    uint64_t startTick; 
    volatile uint8_t remoteWakeup;
    volatile uint8_t selfWakeup;
    volatile uint8_t isResume;
    volatile usb_power_status_t suspend;
#if (defined(USB_DEVICE_CONFIG_DETACH_ENABLE) && (USB_DEVICE_CONFIG_DETACH_ENABLE > 0U))
    volatile uint8_t connectStateChanged;
    volatile uint8_t connectState;
#endif
#if (defined(USB_DEVICE_CONFIG_CHARGER_DETECT) && (USB_DEVICE_CONFIG_CHARGER_DETECT > 0U))
    usb_device_dcd_dev_status_t dcdDectionStatus;
#endif
} usb_device_composite_struct_t;

#if defined(__cplusplus)
extern "C" {
#endif
/*!
 * @brief Initialize USB module hardware and software.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceApplicationInit(void);

/*!
 * @brief Check power status of device
 */
usb_power_status_t getPowerStatus(void);

/*!
 * @brief USB device tasks function.
 */
void USB_DeviceTasks(void);

#if defined(__cplusplus)
}
#endif
#endif /* __USB_DEVICE_COMPOSITE_H__ */
