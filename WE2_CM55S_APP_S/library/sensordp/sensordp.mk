# directory declaration
LIB_SENSORDP_OBJS =
LIB_SENSORDP_DIR = $(LIBRARIES_ROOT)/sensordp

LIB_SENSORDP_ASMSRCDIR	= $(LIB_SENSORDP_DIR)
LIB_SENSORDP_CSRCDIR	= $(LIB_SENSORDP_DIR)
LIB_SENSORDP_CXXSRCSDIR    = $(LIB_SENSORDP_DIR)
LIB_SENSORDP_INCDIR	= $(LIB_SENSORDP_DIR)/inc
LIB_SENSORDP_INCDIR	+= $(LIB_SENSORDP_DIR)/internal_inc

# find all the source files in the target directories
LIB_SENSORDP_CSRCS = $(call get_csrcs, $(LIB_SENSORDP_CSRCDIR))
LIB_SENSORDP_CXXSRCS = $(call get_cxxsrcs, $(LIB_SENSORDP_CXXSRCSDIR))
LIB_SENSORDP_ASMSRCS = $(call get_asmsrcs, $(LIB_SENSORDP_ASMSRCDIR))

# get object files
LIB_SENSORDP_COBJS = $(call get_relobjs, $(LIB_SENSORDP_CSRCS))
LIB_SENSORDP_CXXOBJS = $(call get_relobjs, $(LIB_SENSORDP_CXXSRCS))
LIB_SENSORDP_ASMOBJS = $(call get_relobjs, $(LIB_SENSORDP_ASMSRCS))
LIB_SENSORDP_OBJS = $(LIB_SENSORDP_COBJS) $(LIB_SENSORDP_ASMOBJS) $(LIB_SENSORDP_CXXOBJS)

# get dependency files
LIB_SENSORDP_DEPS = $(call get_deps, $(LIB_SENSORDP_OBJS))

# extra macros to be defined
LIB_SENSORDP_DEFINES = -DLIB_SENSORDP

# genearte library
ifeq ($(SENSORDP_LIB_FORCE_PREBUILT), y)
override LIB_SENSORDP_OBJS:=
endif
SENSORDP_LIB_NAME = libsensordp.a
LIB_LIB_SENSORDP := $(subst /,$(PS), $(strip $(OUT_DIR)/$(SENSORDP_LIB_NAME)))

# library generation rule
$(LIB_LIB_SENSORDP): $(LIB_SENSORDP_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_SENSORDP_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(SENSORDP_LIB_NAME) $(LIB_LIB_SENSORDP)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_SENSORDP_OBJS)
	$(CP) $(LIB_LIB_SENSORDP) $(PREBUILT_LIB)$(SENSORDP_LIB_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_SENSORDP_INCDIR)
LIB_CSRCDIR += $(LIB_SENSORDP_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_SENSORDP_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_SENSORDP_ASMSRCDIR)

LIB_CSRCS += $(LIB_SENSORDP_CSRCS)
LIB_CXXSRCS += $(LIB_SENSORDP_CXXSRCS)
LIB_ASMSRCS += $(LIB_SENSORDP_ASMSRCS)
LIB_ALLSRCS += $(LIB_SENSORDP_CSRCS) $(LIB_SENSORDP_ASMSRCS)

LIB_COBJS += $(LIB_SENSORDP_COBJS)
LIB_CXXOBJS += $(LIB_SENSORDP_CXXOBJS)
LIB_ASMOBJS += $(LIB_SENSORDP_ASMOBJS)
LIB_ALLOBJS += $(LIB_SENSORDP_OBJS)

LIB_DEFINES += $(LIB_SENSORDP_DEFINES)
LIB_DEPS += $(LIB_SENSORDP_DEPS)
LIB_LIBS += $(LIB_LIB_SENSORDP)