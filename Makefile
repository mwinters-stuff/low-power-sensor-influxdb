ARDUINO_VARIANT = nodemcuv2
# UPLOAD_PORT = /dev/nodemcu
# UPLOAD_PORT = /dev/frogboard
UPLOAD_PORT = /dev/ttyUSB0
# UPLOAD_SPEED=921600
UPLOAD_SPEED = 115200

SERIAL_PORT = $(UPLOAD_PORT)
SERIAL_BAUD = 115200

LOG_SERIAL_TO_FILE=yes
# USER_DEFINE = -DTEST
OTA_IP = 192.168.1.135
OTA_PORT = 8266 
OTA_AUTH = 123
ESP8266_VERSION=2.5.2
# GLOBAL_USER_LIBDIR=./libs
LOCAL_USER_LIBDIR=./libs
# USER_LIBS=\
#     $(LOCAL_LIBS)/Adafruit_IO_Arduino/src \
# 		$(LOCAL_LIBS)/Adafruit_IO_Arduino/src/util \
# 		$(LOCAL_LIBS)/Adafruit_IO_Arduino/src/wifi \
#     $(LOCAL_LIBS)/Adafruit_MQTT_Library 


# ifeq ($(ENABLE_GDB), 1)
# 	CFLAGS += -Og -ggdb -DGDBSTUB_FREERTOS=0 -DENABLE_GDB=1
# 	USER_LIBS += $(HOME)/Arduino/Esp8266-Arduino-Makefile/esp8266-2.4.2/libraries/GDBStub
# endif

include ~/Arduino/Esp8266-Arduino-Makefile/espXArduino.mk

