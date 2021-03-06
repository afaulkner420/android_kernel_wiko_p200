#ifndef _LINUX_ELAN_FP_H
#define _LINUX_ELAN_FP_H

#define FINGERPRINT_IOCTL               0x80
#define ID_IOCTL_READ_REGISTER          _IOW(FINGERPRINT_IOCTL, 2,  int)
#define ID_IOCTL_WRITE_REGISTER         _IOW(FINGERPRINT_IOCTL, 3,  int)
#define ID_IOCTL_RESET                  _IOW(FINGERPRINT_IOCTL, 6,  int)
#define ID_IOCTL_GET_RAW_IMAGE          _IOW(FINGERPRINT_IOCTL, 10, int)
#define ID_IOCTL_READ_CMD               _IOW(FINGERPRINT_IOCTL, 15, int)
#define ID_IOCTL_WRITE_CMD              _IOW(FINGERPRINT_IOCTL, 16, int)
#define ID_IOCTL_IOIRQ_STATUS           _IOW(FINGERPRINT_IOCTL, 17, int)
#define ID_IOCTL_POLL_INIT              _IOW(FINGERPRINT_IOCTL, 20, int) 
#define ID_IOCTL_READ_ALL               _IOW(FINGERPRINT_IOCTL, 21, int)
#define ID_IOCTL_INPUT_KEYCODE          _IOW(FINGERPRINT_IOCTL, 22, int) 
#define ID_IOCTL_POLL_EXIT              _IOW(FINGERPRINT_IOCTL, 23, int)
#define ID_IOCTL_SET_KEYCODE            _IOW(FINGERPRINT_IOCTL, 24, int)
#define ID_IOCTL_XY_SETTING             _IOW(FINGERPRINT_IOCTL, 25, int)
#define ID_IOCTL_READ_FACTORY_STATUS    _IOW(FINGERPRINT_IOCTL, 26, int)
#define ID_IOCTL_WRITE_FACTORY_STATUS   _IOW(FINGERPRINT_IOCTL, 27, int)
#define ID_IOCTL_INPUT_KEYCODE_DOWN     _IOW(FINGERPRINT_IOCTL, 28, int)
#define ID_IOCTL_INPUT_KEYCODE_UP       _IOW(FINGERPRINT_IOCTL, 29, int)
#define ID_IOCTL_GET_RAW_IMAGE_POLL     _IOW(FINGERPRINT_IOCTL, 30, int)
#define ID_IOCTL_RAW_BYTES              _IOW(FINGERPRINT_IOCTL, 31, int)
#define ID_IOCTL_INT_STATUS             _IOW(FINGERPRINT_IOCTL, 40, int)
#define ID_IOCTL_WAKE_LOCK_UNLOCK       _IOW(FINGERPRINT_IOCTL, 41, int)
#define ID_IOCTL_EN_IRQ                 _IOW(FINGERPRINT_IOCTL, 55, int)
#define ID_IOCTL_DIS_IRQ                _IOW(FINGERPRINT_IOCTL, 66, int)
#define ID_IOCTL_SET_IRQ_TYPE           _IOW(FINGERPRINT_IOCTL, 91, int)
#define ID_IOCTL_SET_IC                 _IOW(FINGERPRINT_IOCTL, 92, int)
#define ID_IOCTL_DISPLAY_STATUS         _IOW(FINGERPRINT_IOCTL, 93, int)
#define ID_IOCTL_DISPLAY_NOTIFY         _IOW(FINGERPRINT_IOCTL, 94, int)
#define ID_IOCTL_REQUEST_IRQ            _IOW(FINGERPRINT_IOCTL, 95, int)
#define ID_IOCTL_FREE_IRQ               _IOW(FINGERPRINT_IOCTL, 96, int)

#define ELAN_IOCTLID                    0xD0 // For customer define
#define IOCTL_SPI_CONFIG                _IOW(ELAN_IOCTLID, 8, int)
#define IOCTL_READ_KEY_STATUS           _IOR(ELAN_IOCTLID, 10, int)
#define IOCTL_WRITE_KEY_STATUS          _IOR(ELAN_IOCTLID, 11, int)

#endif /* _LINUX_ELAN_FP_H */

