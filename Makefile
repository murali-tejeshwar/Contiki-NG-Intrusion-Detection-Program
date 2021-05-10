# Contiki network configuration
MAKE_MAC = MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET

CONTIKI_PROJECT = client basestation test-adxl345 test-tmp login
APPS=serial-shell

all: $(CONTIKI_PROJECT)

CONTIKI = <your contiki-ng root path>
include $(CONTIKI)/Makefile.include
