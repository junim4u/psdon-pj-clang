################################################################################
# Makefile to make sample application
################################################################################
.EXPORT_ALL_VARIABLES:

CC=gcc
RM := rm -rf
#LIBS := -lm -lpthread -lrt -lssl -lcrypto -lcurl -lz
LIBS := -lm -lpthread

CFLAGS += -O1
CFLAGS += -fPIC


TOPDIR=.
SRCDIR:=$(TOPDIR)

APP = psim

SRC=$(wildcard $(SRCDIR)/main_sim_alg.c)
OBJS=$(SRC:.c=.o)

.SUFFIXES:.o .c

.PHONY: clean all


# All Target
all: BUILT_TIME $(APP)

$(APP): $(OBJS)
	@echo 'Building target: $@'
	$(CC) $(LIB_PATH) -o $(APP) $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *.o
	-$(RM) $(APP)
	-@echo ' '

dist:
	-$(RM) *.o
	-$(RM) $(APP)
	-@echo ' '

BUILT_TIME:
	@echo  \#define BUILT_TIME \"`TZ=UTC date -u "+%Y.%m.%d-%H:%M%z" `\" > ./built_time
