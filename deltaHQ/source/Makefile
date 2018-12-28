COMPILER  = qrintf /usr/local/opt/llvm/bin/clang
CFLAGS    = -O3 -Wall -D_GNU_SOURCE -DMESSAGE_WARNING -pipe -march=native -DMEASURE -MMD -MP
ifeq "$(shell getconf LONG_BIT)" "64"
  LDFLAGS = -lpthread -ljansson -lexpat
else
  LDFLAGS =
endif
LIBS      =
INCLUDE   = -I../include
TARGET    = ../../bin/deltaHQ
SRCDIR    = ./
ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.c)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.c=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
