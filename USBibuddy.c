/**
 * @filename USBibuddy.c
 * Bricolé par Bruno
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
#include "USBibuddy.h"

static int debug_level=0;

//=============================================================================

struct device_usb {
  struct
  usb_device *device;
  usb_dev_handle    *handle;
  int                debug;
  int                timeout;
  int                interface_claimed;
};

//=============================================================================

int usb_initialise(void) {
  usb_set_debug(0);
  usb_init();
  usb_find_busses();
  usb_find_devices();

  return EXIT_SUCCESS;
}

//=============================================================================

device_usb *usb_create(int debug, int timeout)
{
  device_usb *control = malloc(sizeof(*control));
  if (control == NULL)
    return NULL;

  control->device            = NULL;
  control->handle            = NULL;
  control->debug             = debug;
  control->timeout           = timeout;
  control->interface_claimed = 0;

  return control;
}

//=============================================================================

void usb_destroy(device_usb *control) {
  if (control == NULL)
    return;

  if (control->handle != NULL)
    usb_close(control->handle);

  free(control);
}

//=============================================================================

int usb_finddevice(device_usb *control, int device_num,
			   int device_type) {

  int found_device = 0;
  int device_count = 0;
  struct usb_bus  *bus;

  assert(control != NULL);
  assert(control->interface_claimed == 0);
  assert(device_num >= 0 && device_num < 255);

  for (bus = usb_get_busses(); bus != NULL; bus = bus->next) {

    struct usb_device *dev;

    for (dev = bus->devices; dev != NULL; dev = dev->next) {

      if (control->debug) {
	printf("Found device: %04x-%04x\n",
	       dev->descriptor.idVendor,
	       dev->descriptor.idProduct);
      }

      switch (device_type) {

      case DEVICE_TYPE_EBUD:
	if (dev->descriptor.idVendor  == EBUD_VENDOR_ID &&
	    dev->descriptor.idProduct == EBUD_PRODUCT_ID)
	  found_device = 1;
	break;


      default:
 	printf("Device Type (%d) not implemented, please do it!\n",
	       device_type);
	return EXIT_FAILURE;

      }

      if (found_device) {

	if (control->debug)
	  printf("Found control num %d\n", device_count);

	if (device_count == device_num) {

	  control->device = dev;
	  control->handle = usb_open(control->device);

	  if (control->handle == NULL) {
	    perror("usb_open failed\n");
	    return EXIT_FAILURE;
	  }

	  if (control->debug)
	    printf("Successfully opened device\n");

	  return EXIT_SUCCESS;
	}

	device_count++;
      }

    }

  }

  if (debug_level)
    fprintf(stderr,
	    "ibuddy_usb_finddevice failed to find ");

  return EXIT_FAILURE;
}

//=============================================================================

static int claim_interface(device_usb *control) {
  int  ret;

  if (control->interface_claimed == 0) {

    if (control->debug)
      printf("Trying to detach kernel driver\n");

    /* try to detach device in case usbhid has got hold of it */
    ret = usb_detach_kernel_driver_np(control->handle, 0);

    if (ret != 0) {
      if (errno == ENODATA) {
	if (control->debug)
	  printf("Device already detached\n");
      } else {
	if (control->debug) {
	  printf("Detach failed: %s[%d]\n", strerror(errno), errno);
	  printf("Continuing anyway\n");
	}
      }
    } else {
      if (control->debug)
	printf("detach successful\n");
    }

    /* And now for the other interface */
    ret = usb_detach_kernel_driver_np(control->handle, 1);

    if (ret != 0) {
      if (errno == ENODATA) {
	if (control->debug)
	  printf("Device already detached\n");
      } else {
	if (control->debug) {
	  printf("Detach failed: %s[%d]\n", strerror(errno), errno);
	  printf("Continuing anyway\n");
	}
      }
    } else {
      if (control->debug)
	printf("detach successful\n");
    }

    ret = usb_set_configuration(control->handle, 1);
    if (ret < 0) {
      perror("usb_set_configuration failed\n");
      return EXIT_FAILURE;
    }

    ret = usb_claim_interface(control->handle, 0);
    if (ret < 0) {
      perror("usb_claim_interface failed\n");
      return EXIT_FAILURE;
    }

    ret = usb_claim_interface(control->handle, 1);
    if (ret < 0) {
      perror("usb_claim_interface failed\n");
      return EXIT_FAILURE;
    }

    control->interface_claimed = 1;

  }

  return EXIT_SUCCESS;
}

//=============================================================================

int ibuddy_do(device_usb *control, int cmd) {

unsigned char  commande;
    /* Two fixed-format initiator packets appear to be required */
    /* pour ibuddy*/
/*device_type=device_type;*/
commande= (unsigned char)cmd ;
/* proteger ibuddy de fausse manipulation */
/* evite les cas 0 0  des bits 1 et 2 ainsi que  3 et 4*/
        if((commande & 0x3) == 0) commande |= 0x3;
        if((commande & 0xC) == 0) commande |= 0xC;


    if (ibuddy_usb_sendcommand(control, 'U', 'S', 'B', 'C', 0,'@' , 2, commande))
    {
      fprintf(stderr,
	      "ebuddy_usb_sendcommand() failed: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

//=============================================================================

int ibuddy_usb_sendcommand(device_usb *control,
                unsigned char a, unsigned char b, unsigned char c, unsigned char  d,
                unsigned char e, unsigned char f, unsigned char g, unsigned char h) {

  unsigned char buf[8];
  int  ret;
  ret = claim_interface(control);
  if (ret != 0) {
    return EXIT_FAILURE;
  }

  buf[0] = a;
  buf[1] = b;
  buf[2] = c;
  buf[3] = d;
  buf[4] = e;
  buf[5] = f;
  buf[6] = g;
  buf[7] = h;

  if (control->debug) {
    printf("sending bytes %d, %d, %d, %d, %d, %d, %d, %d\n",
	   a, b, c, d, e, f, g, h );
  }

  ret = usb_control_msg( control->handle, 0x21, 9, 0x2, 0x01, (char*) buf,
			 8, control->timeout);
  if (ret != 8) {
    perror("usb_control_msg failed\n");
    return EXIT_FAILURE;
  }

  return 0;
}

//=============================================================================
/* quel 64 bits ? */
int objet_usb_sendcommand64(device_usb *control,
                 unsigned char a, unsigned char b, unsigned char c, unsigned char  d,
                 unsigned char e, unsigned char f, unsigned char g, unsigned char h) {

  unsigned char buf[64];
  int  ret;

  assert(control != NULL);

  ret = claim_interface(control);
  if (ret != 0) {
    return EXIT_FAILURE;
  }

  memset(buf, 0, 64 );
  buf[0] = a;
  buf[1] = b;
  buf[2] = c;
  buf[3] = d;
  buf[4] = e;
  buf[5] = f;
  buf[6] = g;
  buf[7] = h;

  if (control->debug) {
    printf("sending bytes %d, %d, %d, %d, %d, %d, %d, %d\n",
	   a, b, c, d, e, f, g, h );
  }

  ret = usb_control_msg(control->handle, 0x21, 9,
			0x2, 0x0, (char*) buf, 64,  control->timeout);

  if (ret != 64) {
    perror("usb_control_msg failed\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

//=============================================================================

/*
 * Local Variables:
 * mode: C
 * End:
 */
int ebuddy_envoi(unsigned char command)
{

#define USB_TIMEOUT 1000 /* milliseconds */

  /*debug_level = 0;*/
  device_usb *control;


if ((command & 0x01) && (command & 0x02))   {
                                            command = command | (unsigned char)~0x03;
                                            printf ("PERDU ! ebuddy is dead , corps droit et gauche ! il faut choisir \n");

                                            }
if ((command & 0x04) && (command & 0x08))   {
                                            command = command | (unsigned char)~0xC0;
                                            printf ("PERDU ! ebuddy is dead ,ailes hautes et basses ! il faut choisir  \n");

                                            }

  unsigned char msg = command;
  int device_type = 1;
  //char *device = NULL;


    if (usb_initialise() != 0)
  	{
      	fprintf(stderr, "usb_initalise failed: %s\n", strerror(errno));
      	return EXIT_FAILURE;
  	}

    control = usb_create(debug_level, USB_TIMEOUT);
    if (control == NULL) {
      fprintf(stderr, "usb_create() failed\n");
      return EXIT_FAILURE;
    }

    if (usb_finddevice(control, 0, device_type) != 0) {
      fprintf(stderr, "USB  device not found\n");
      return EXIT_FAILURE;
    }
  /*
  //  coeur ! couleur corps !  Ailes  ! corps
  //     X     x    x    x     Y   Y    Z  Z
  // attention ne pas alimenter Ailes 0 et O et corps  0 0 sous peine de destruction
  #define  ebuddy_coeur           ~0x80
  #define  ebuddy_bleu            ~0x40
  #define  ebuddy_vert            ~0x20
  #define  ebuddy_rouge           ~0x10
  #define  ebuddy_corps_droite    ~0x02
  #define  ebuddy_corps_gauche    ~0x01
  #define  ebuddy_ailes_hautes    ~0x08
  #define  ebuddy_ailes_basses    ~0x04
  */
  /* msg = (unsigned char)(ebuddy_coeur & ebuddy_rouge & ebuddy_ailes_hautes & ebuddy_corps_droite ); */

   /*msg = (unsigned char)(ebuddy_coeur & ebuddy_bleu & ebuddy_ailes_hautes & ebuddy_corps_gauche );*/


   ibuddy_do(control, (unsigned char)~msg);


  return EXIT_SUCCESS;
}
