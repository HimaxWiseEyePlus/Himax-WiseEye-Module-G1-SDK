# directory declaration
LIB_AUDIO_PREROLLING_DIR = $(LIBRARIES_ROOT)/audio_prerolling

LIB_AUDIO_PREROLLING_ASMSRCDIR	= $(LIB_AUDIO_PREROLLING_DIR)
LIB_AUDIO_PREROLLING_CSRCDIR	= $(LIB_AUDIO_PREROLLING_DIR)
LIB_AUDIO_PREROLLING_CXXSRCSDIR    = $(LIB_AUDIO_PREROLLING_DIR)
LIB_AUDIO_PREROLLING_INCDIR	= $(LIB_AUDIO_PREROLLING_DIR) 

# find all the source files in the target directories
LIB_AUDIO_PREROLLING_CSRCS = $(call get_csrcs, $(LIB_AUDIO_PREROLLING_CSRCDIR))
LIB_AUDIO_PREROLLING_CXXSRCS = $(call get_cxxsrcs, $(LIB_AUDIO_PREROLLING_CXXSRCSDIR))
LIB_AUDIO_PREROLLING_ASMSRCS = $(call get_asmsrcs, $(LIB_AUDIO_PREROLLING_ASMSRCDIR))

# get object files
LIB_AUDIO_PREROLLING_COBJS = $(call get_relobjs, $(LIB_AUDIO_PREROLLING_CSRCS))
LIB_AUDIO_PREROLLING_CXXOBJS = $(call get_relobjs, $(LIB_AUDIO_PREROLLING_CXXSRCS))
LIB_AUDIO_PREROLLING_ASMOBJS = $(call get_relobjs, $(LIB_AUDIO_PREROLLING_ASMSRCS))
LIB_AUDIO_PREROLLING_OBJS = $(LIB_AUDIO_PREROLLING_COBJS) $(LIB_AUDIO_PREROLLING_ASMOBJS) $(LIB_AUDIO_PREROLLING_CXXOBJS)

# get dependency files
LIB_AUDIO_PREROLLING_DEPS = $(call get_deps, $(LIB_AUDIO_PREROLLING_OBJS))

# extra macros to be defined
LIB_AUDIO_PREROLLING_DEFINES = -DLIB_AUDIO_PREROLLING

# genearte library
LIB_LIB_PREROLLING_AUDIO = $(OUT_DIR)/liblibaudioprerolling.a


# library generation rule
$(LIB_LIB_PREROLLING_AUDIO): $(LIB_AUDIO_PREROLLING_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_AUDIO_PREROLLING_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
LIB_INCDIR += $(LIB_AUDIO_PREROLLING_INCDIR)
LIB_CSRCDIR += $(LIB_AUDIO_PREROLLING_CSRCDIR)
LIB_CXXSRCDIR += $(LIB_AUDIO_PREROLLING_CXXSRCDIR)
LIB_ASMSRCDIR += $(LIB_AUDIO_PREROLLING_ASMSRCDIR)

LIB_CSRCS += $(LIB_AUDIO_PREROLLING_CSRCS)
LIB_CXXSRCS += $(LIB_AUDIO_PREROLLING_CXXSRCS)
LIB_ASMSRCS += $(LIB_AUDIO_PREROLLING_ASMSRCS)
LIB_ALLSRCS += $(LIB_AUDIO_PREROLLING_CSRCS) $(LIB_AUDIO_PREROLLING_ASMSRCS)

LIB_COBJS += $(LIB_AUDIO_PREROLLING_COBJS)
LIB_CXXOBJS += $(LIB_AUDIO_PREROLLING_CXXOBJS)
LIB_ASMOBJS += $(LIB_AUDIO_PREROLLING_ASMOBJS)
LIB_ALLOBJS += $(LIB_AUDIO_PREROLLING_OBJS)

LIB_DEFINES += $(LIB_AUDIO_PREROLLING_DEFINES)
LIB_DEPS += $(LIB_AUDIO_PREROLLING_DEPS)
LIB_LIBS += $(LIB_LIB_PREROLLING_AUDIO)