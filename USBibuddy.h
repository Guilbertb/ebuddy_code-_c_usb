/**
 * @filename USB.h
 * bricolé par Bruno
 */

#ifndef DEPEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>
#include <assert.h>
#include <usb.h>
#include <errno.h>
#endif

#ifndef __INC_USBMissileLauncher_h
#define __INC_USBMissileLauncher_h

// Device type, such as Missile Launcher and Circus Cannon
#define DEVICE_TYPE_UNKNOWN          0
#define DEVICE_TYPE_EBUD 1

// ibuddy usb vendor and product id
//modifier pour ibuddy
#define EBUD_VENDOR_ID  0x1130
#define EBUD_PRODUCT_ID 0x0004
//1130:0004

//  coeur ! couleur corps !  Ailes  ! corps
//     X     x    x    x     Y   Y    Z  Z
// attention ne pas alimenter Ailes 0 et O et corps  0 0 sous peine de destruction
/*#define  ebuddy_coeur           ~0x80

#define  ebuddy_bleu            ~0x40
#define  ebuddy_vert            ~0x20
#define  ebuddy_rouge           ~0x10

#define  ebuddy_ailes_hautes    ~0x08
#define  ebuddy_ailes_basses    ~0x04

#define  ebuddy_corps_droite    ~0x02
#define  ebuddy_corps_gauche    ~0x01
*/



#ifdef __cplusplus
extern "C" {
#endif

  /** opaque handle for device */
  typedef struct device_usb device_usb;

  int usb_initialise(void);

  void usb_finalise(void);

  /**
   * @brief Create a device handle
   *
   * @param debug   set to 1 to enable debug, 0 otherwise
   * @param timeout usb comms timeout in milliseconds (1000 would be
   * reasonable)
   *
   * @return pointer to opaque device handle on success, NULL on failure
   */
  device_usb *usb_create(int debug, int timeout);

  /**
   * @brief Destroy a device handle
   *
   * @param control device handle to destroy
   */
  void usb_destroy(device_usb *control);

  /**
   * @brief Attempt to find the usb missile launcher device
   *
   * @note Scans through all the devices on all the usb busses and
   * finds the 'device_num'th device.
   *
   * @param control    library handle
   * @param device_num 0 for first device, 1 for second, etc
   *
   * @return 0 on success, -1 on failure
   */
  int usb_finddevice(device_usb *control, int device_num,
			     int device_type);

  /**
   * @brief Perform an USB Missile Launcher Command
   *
   * @note Thanks to Florent Papin <florent.papin@free.fr> for his
   *       code for the Circus Cannon
   */
  int ibuddy_do(device_usb *control, int cmd);

  /**
   * @brief Send a usb command
   *
   * @note missile_usb_sendcommand64 not used for circus cannon
   *
   * @param control    library handle
   * @param ...        (first) 8 bytes of packet
   *
   * @return 0 on success, -1 on failure
   */
  int ibuddy_usb_sendcommand(device_usb *control,
                  unsigned char a, unsigned char b, unsigned char c, unsigned char  d,
                  unsigned char e, unsigned char f, unsigned char g, unsigned char h);
  int ibuddy_usb_sendcommand64(device_usb *control,
                  unsigned char a, unsigned char b, unsigned char c, unsigned char  d,
                  unsigned char e, unsigned char f, unsigned char g, unsigned char h);

   int ebuddy_envoi(unsigned char command);
  //===========================================================================

#ifdef __cplusplus
}
#endif

#endif //__INC_USBMissileLauncher_h

//=============================================================================

/*
 * Local Variables:
 * mode: C
 * End:
 */
