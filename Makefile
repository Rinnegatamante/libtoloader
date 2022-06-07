TARGET          := libtoloader
SOURCES         := source source/utils

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
HEADERS  := $(CGFILES:.cg=.h)
OBJS     := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX     = $(PREFIX)-g++
AR      = $(PREFIX)-gcc-ar
CFLAGS  = -g -Wl,-q -O3 -ffast-math -mtune=cortex-a9 -mfpu=neon
ASFLAGS = $(CFLAGS)

CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -Wno-write-strings

all: $(TARGET).a

$(TARGET).a: $(OBJS)
	$(AR) -rc $@ $^

shaders: $(HEADERS)
	
clean: $(SAMPLES_CLR)
	@rm -rf $(TARGET).a $(TARGET).elf $(OBJS)
	
install: $(TARGET).a
	@mkdir -p $(VITASDK)/$(PREFIX)/lib/
	cp $(TARGET).a $(VITASDK)/$(PREFIX)/lib/
	@mkdir -p $(VITASDK)/$(PREFIX)/include/
	cp source/tinyiest_obj_loader.h $(VITASDK)/$(PREFIX)/include/
	
samples: $(SAMPLES)
