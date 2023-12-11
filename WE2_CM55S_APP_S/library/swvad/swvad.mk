# directory declaration
LIB_SWVAD_DIR = $(LIBRARIES_ROOT)/swvad

LIB_SWVAD_ASMSRCDIR	= $(LIB_SWVAD_DIR)
LIB_SWVAD_CSRCDIR	= $(LIB_SWVAD_DIR)
LIB_SWVAD_CXXSRCSDIR    = $(LIB_SWVAD_DIR)
LIB_SWVAD_INCDIR	= $(LIB_SWVAD_DIR) 

# find all the source files in the target directories
LIB_SWVAD_CSRCS = $(call get_csrcs, $(LIB_SWVAD_CSRCDIR))
LIB_SWVAD_CXXSRCS = $(call get_cxxsrcs, $(LIB_SWVAD_CXXSRCSDIR))
LIB_SWVAD_ASMSRCS = $(call get_asmsrcs, $(LIB_SWVAD_ASMSRCDIR))

# get object files
LIB_SWVAD_COBJS = $(call get_relobjs, $(LIB_SWVAD_CSRCS))
LIB_SWVAD_CXXOBJS = $(call get_relobjs, $(LIB_SWVAD_CXXSRCS))
LIB_SWVAD_ASMOBJS = $(call get_relobjs, $(LIB_SWVAD_ASMSRCS))
LIB_SWVAD_OBJS = $(LIB_SWVAD_COBJS) $(LIB_SWVAD_ASMOBJS) $(LIB_SWVAD_CXXOBJS)

# get dependency files
LIB_SWVAD_DEPS = $(call get_deps, $(LIB_SWVAD_OBJS))

# extra macros to be defined
LIB_SWVAD_DEFINES = -DLIB_SWVAD -DHX_AUTOCORR -DDC_UPDATE #-DSWVAD_TICK_TEST #-DRECORD #-DINNER_SWVAD_TICK_TEST#-DSWVAD_TICK_TEST

# genearte library
ifeq ($(SWVAD_LIB_FORCE_PREBUILT), y)
override LIB_SWVAD_OBJS:=
endif
LIB_LIB_SWVAD_NAME = libswvad.a
LIB_LIB_SWVAD := $(subst /,$(PS), $(strip $(OUT_DIR)/$(LIB_LIB_SWVAD_NAME)))

# library generation rule
$(LIB_LIB_SWVAD): $(LIB_SWVAD_OBJS)
	$(TRACE_ARCHIVE)
ifeq "$(strip $(LIB_SWVAD_OBJS))" ""
	$(CP) $(PREBUILT_LIB)$(LIB_LIB_SWVAD_NAME) $(LIB_LIB_SWVAD)
else
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_SWVAD_OBJS)
	$(CP) $(LIB_LIB_SWVAD) $(PREBUILT_LIB)$(LIB_LIB_SWVAD_NAME)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_SWVAD_INCDIR)
LIB_CSRCDIR += $(LIB_SWVAD_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_SWVAD_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_SWVAD_ASMSRCDIR)

LIB_CSRCS += $(LIB_SWVAD_CSRCS)
LIB_CXXSRCS += $(LIB_SWVAD_CXXSRCS)
LIB_ASMSRCS += $(LIB_SWVAD_ASMSRCS)
LIB_ALLSRCS += $(LIB_SWVAD_CSRCS) $(LIB_SWVAD_ASMSRCS)

LIB_COBJS += $(LIB_SWVAD_COBJS)
LIB_CXXOBJS += $(LIB_SWVAD_CXXOBJS)
LIB_ASMOBJS += $(LIB_SWVAD_ASMOBJS)
LIB_ALLOBJS += $(LIB_SWVAD_OBJS)

LIB_DEFINES += $(LIB_SWVAD_DEFINES)
LIB_DEPS += $(LIB_SWVAD_DEPS)
LIB_LIBS += $(LIB_LIB_SWVAD)