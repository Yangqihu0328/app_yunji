CUR_PATH		    := $(shell pwd)
HOME_PATH		    := $(abspath $(CUR_PATH)/../../../../../..)

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

PRJ_OUT_HOME        := $(HOME_PATH)/build/out/$(PROJECT)
OBJ_OUT_PATH        := $(PRJ_OUT_HOME)/objs
MSP_OUT_PATH        := $(HOME_PATH)/msp/out
SRC_PATH            := $(CUR_PATH)
LIB_PATH            := $(MSP_OUT_PATH)/lib
INC_PATH            := $(MSP_OUT_PATH)/include
SRC_RELATIVE_PATH   := $(subst $(HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH     := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
ROOTFS_TARGET_PATH  := $(MSP_OUT_PATH)
DEMO_SRC_PATH       := $(HOME_PATH)/app/demo/src
DEMO_BIN_PATH       := $(HOME_PATH)/app/demo/bin/aicard/master
THIRD_PARTY_PATH    := $(HOME_PATH)/third-party
3RD_PATH        	:= $(DEMO_SRC_PATH)/3rd
BIN_PATH            := $(HOME_PATH)/app/demo/bin/aicard/master

# output
MOD_NAME            := aicard_master
OUTPUT              := $(TARGET_OUT_PATH)/obj

# source
SRCCPPS             := $(wildcard $(SRC_PATH)/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/vdec/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/vdec/stream/FileStreamer.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/log/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/stage/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/vo/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/pcie/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/pool/AXPoolManager.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/utils/ElapsedTimer.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/utils/SpsParser.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/utils/yuv/BmpOSD.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/utils/yuv/YuvHandler.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/encoder/VideoEncoder.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/rtsp/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/config_parser/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/linkage/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/ivps/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/ivps/osd/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/sensor/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/dsp/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/ives/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/subfunc/eocw/*.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/AlgoOptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/AvsOptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/EncoderOptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/IvpsOptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/OptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/OsdOptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/SensorOptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/option_helper/AudioOptionHelper.cpp) \
                       $(wildcard $(DEMO_SRC_PATH)/audio/*.cpp) \
                       $(DEMO_SRC_PATH)/ppl/nvr/hal/linker.cpp \
                       $(wildcard $(DEMO_SRC_PATH)/config/ini/*.cpp)

CINCLUDE            := -I$(SRC_PATH) \
                       -I$(INC_PATH) \
                       -I$(DEMO_SRC_PATH)/header \
					   -I$(DEMO_SRC_PATH)/vdec \
                       -I$(DEMO_SRC_PATH)/vdec/stream \
                       -I$(DEMO_SRC_PATH)/option_helper \
                       -I$(DEMO_SRC_PATH)/utils \
                       -I$(DEMO_SRC_PATH)/utils/yuv \
                       -I$(DEMO_SRC_PATH)/log \
                       -I$(DEMO_SRC_PATH)/stage \
                       -I$(DEMO_SRC_PATH)/vo \
                       -I$(DEMO_SRC_PATH)/pcie \
                       -I$(DEMO_SRC_PATH)/pool \
                       -I$(DEMO_SRC_PATH)/config/ini \
                       -I$(DEMO_SRC_PATH)/encoder \
                       -I$(DEMO_SRC_PATH)/rtsp \
                       -I$(DEMO_SRC_PATH)/config_parser \
                       -I$(DEMO_SRC_PATH)/config/ini \
					   -I$(DEMO_SRC_PATH)/config/json \
                       -I$(DEMO_SRC_PATH)/linkage \
                       -I$(DEMO_SRC_PATH)/ivps \
                       -I$(DEMO_SRC_PATH)/ivps/osd \
                       -I$(DEMO_SRC_PATH)/sensor \
                       -I$(DEMO_SRC_PATH)/dsp \
                       -I$(DEMO_SRC_PATH)/ives \
                       -I$(DEMO_SRC_PATH)/subfunc/eocw \
                       -I$(DEMO_SRC_PATH)/audio \
                       -I$(DEMO_SRC_PATH)/webserver \
                       -I$(DEMO_SRC_PATH)/ppl/nvr/hal \
                       -I$(3RD_PATH)/inc/appweb \
                       -I$(3RD_PATH)/inc/live/groupsock \
                       -I$(3RD_PATH)/inc/live/liveMedia \
                       -I$(3RD_PATH)/inc/live/UsageEnvironment \
                       -I$(3RD_PATH)/inc/live/BasicUsageEnvironment \
                       -I$(THIRD_PARTY_PATH)/ffmpeg/include \
                       -I$(THIRD_PARTY_PATH)/openssl/include

 OBJS               := $(SRCS:%.c=$(OUTPUT)/%.o)
 CPPOBJS            := $(SRCCPPS:%.cpp=$(OUTPUT)/%.o)

 DEPS               := $(OBJS:%.o=%.d)
 CPPDEPS            := $(CPPOBJS:%.o=%.d)

# exec
TARGET              := $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
CPPFLAGS            := --std=c++11
CFLAGS              := $(CPPFLAGS) -Wall -Wl,-rpath-link=$(LIB_PATH):$(BIN_PATH)/lib:$(3RD_PATH)/lib:$(THIRD_PARTY_PATH)/ffmpeg/lib
CFLAGS              += -DAPP_BUILD_VERSION=\"$(SDK_VERSION)\"

CFLAGS             += -D__RTSP_UNSUPPORT__

# Independant debug
# CFLAGS             += -D__MASTER_DEBUG__

# dummy vo: not send to vo
# CFLAGS            += -D__DUMMY_VO__

# dump vdec decoded yuv saved in ./dump_stridexh_grpx_chnx.nv12.img
# CFLAGS            += -D__DUMP_VDEC_FRAME__

# dump nalu before send to vdec
# CFLAGS            += -D__DUMP_VDEC_NALU__

ifeq ($(debug),yes)
CFLAGS              += -O0 -ggdb3
else
CFLAGS              += -O2
endif

# dependency
CLIB                := -lstdc++ -pthread -lm -lrt
CLIB                += -L$(LIB_PATH)
CLIB                += -lax_sys
CLIB                += -lax_vdec
CLIB                += -lax_ivps
CLIB                += -lax_vo
CLIB                += -lax_pcie_dma
CLIB                += -lax_pcie_msg
CLIB                += -L$(THIRD_PARTY_PATH)/drm/lib -ldrm
CLIB                += -L$(THIRD_PARTY_PATH)/ffmpeg/lib -lavcodec -lavutil -lavformat
CLIB                += -L$(THIRD_PARTY_PATH)/openssl/lib -lssl -lcrypto
CLIB                += -L$(BIN_PATH)/lib -L$(3RD_PATH)/lib -lappweb -lmpr -lhttp -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment
CLIB                += -ldl
CLIB                += -lax_venc
CLIB                += -lax_ives
CLIB                += -lax_engine
CLIB                += -lax_dmadim
CLIB                += -lax_dsp -lax_dsp_cv
CLIB			    += -lax_nt_ctrl -lax_proton -lax_mipi -lax_3a -lax_audio -lax_audio_3a
CLIB			    += -L$(HOME_PATH)/third-party/tinyalsa/lib
CLIB			    += -ltinyalsa
CLIB			    += -L$(HOME_PATH)/third-party/fdk-aac/lib
CLIB			    += -lfdk-aac
CLIB			    += -L$(HOME_PATH)/third-party/libsamplerate/lib
CLIB			    += -lsamplerate
CLIB			    += -L$(HOME_PATH)/third-party/opus/lib
CLIB			    += -lopus

# install
INSTALL_TARGET      := $(TARGET)
INSTALL_TARGET      += $(DEMO_BIN_PATH)/*
INSTALL_DIR         := $(ROOTFS_TARGET_PATH)/bin/AiCard/master

# link
LINK = $(CC)

include $(HOME_PATH)/build/rules.mak

