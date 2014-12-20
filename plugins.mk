
# extra sources can be found in the plugins/ folder
SOURCES := $(wildcard $(addprefix plugins/, *.c *.cc *.cpp *.C))

# plugin magic
PLUGINS := $(notdir $(basename $(wildcard $(addprefix plugins/, *.c *.cc *.cpp *.C))))
PLUGIN_MSG_BASES := $(shell echo "$(PLUGINS)" | tr " " "\n" | tr "a-z" "A-Z" | awk '{print "-DMSG_BASE_"$$1"="(FNR-1)*100}')
CPPFLAGS += $(PLUGIN_MSG_BASES)
CFLAGS += $(PLUGIN_MSG_BASES)

define plugins_h_static_contents
// #include MCU_HEADER(x)   =>   #include "x_<MCU>.h"
#define MCU_HEADER(x) STR(JOIN_TOKENS(x,_,MCU.h))
// Token pasting magic (two-level macro is to allow for dereferencing since
// the # and ## operators will not dereference):
#define STR_(x) #x
#define STR(x) STR_(x)
#define JOIN_TOKENS(x,y,z) JOIN_TOKENS_(x,y,z)
#define JOIN_TOKENS_(x,y,z) x ## y ## z
endef
export plugins_h_static_contents

CLEAN_ALSO := plugins.h
empty :=
space :=
space +=
plugins.h: plugins/ Makefile
	echo "$${plugins_h_static_contents}\\n\\n" >$@
	echo >>$@ "#define PLUGINS \\\\\n\\t$(subst $(space), \\\\\n\\t,$(strip $(patsubst %,PLUGIN(%),$(PLUGINS))))"
