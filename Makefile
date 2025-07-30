CC=gcc
RM=rm
CFLAGS=-Wall -std=gnu99
TARGET:=cNet
TARGET_CONF:=cNetConf
SOURCES=cNet.c config.c memPool.c threadPool.c tunnel.c client.c server.c
CONF_SOURCES=cNet.c config.c
OPT=-O1
all: $(TARGET)
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

conf: $(TARGET_CONF)
$(TARGET_CONF): $(CONF_SOURCES)
	$(CC) $(CFLAGS) $^ -o $@
clean:
	$(RM) -f *.o
	$(RM) -f $(TARGET_CONF)
	$(RM) -f $(TARGET)

.PHONY: all conf clean