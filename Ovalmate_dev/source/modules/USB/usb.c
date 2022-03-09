#include "fsl_fro_calib.h"
//#include "usb_device_lpcip3511.h"
#define USB_LPC3511IP_MAX_PHY_ENDPOINT_MASK		(0xFFFFu)
#define USB_LPC3511IP_INTSTAT_DEV_INT_MASK		USBHSD_INTSTAT_DEV_INT_MASK
#define USB_DEVICE_SOF_EVENT_ENABLE				1U
//usb_device_control_type_t type;
void USB_CrystalLessInit(void)
{
	unsigned int test = fro_calib_Get_Lib_Ver();
	Chip_TIMER_Instance_Freq(CTIMER4, 96000);
	/* enable interrupts */
	USB0->INTEN = USB_LPC3511IP_INTSTAT_DEV_INT_MASK |
	USB_LPC3511IP_MAX_PHY_ENDPOINT_MASK | USB_INTSTAT_FRAME_INT_MASK;
}
// call USB_DeviceTasks(); in main loop for operation
