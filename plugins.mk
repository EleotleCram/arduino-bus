
# extra sources can be found in the plugins/ folder
SOURCES := $(wildcard $(addprefix plugins/, *.c *.cc *.cpp *.C))

# plugin magic
PLUGINS := $(notdir $(basename $(wildcard $(addprefix plugins/, *.c *.cc *.cpp *.C))))
PLUGIN_MSG_BASES := $(shell echo "$(PLUGINS)" | tr " " "\n" | tr "a-z" "A-Z" | awk '{print "-DMSG_BASE_"$$1"="(FNR-1)*100}')
CPPFLAGS += $(PLUGIN_MSG_BASES)
CFLAGS += $(PLUGIN_MSG_BASES)

CLEAN_ALSO := plugins.h
empty :=
space :=
space +=
plugins.h: plugins/ Makefile
	echo >$@ "#define PLUGINS \\\\\n\\t$(subst $(space), \\\\\n\\t,$(strip $(patsubst %,X(%),$(PLUGINS))))"
