#
#  Copyright (c) 2016, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

include $(abs_top_nlbuild_autotools_dir)/automake/pre.am

lib_LIBRARIES                                                                                = libopenthread-kw2x.a

NXP_COMMON_CPPFLAGS                                                                          = \
    -DgTargetKW24D512_USB_d=1                                                                  \
    -DgDualPanEnabled_d=0                                                                      \
    -DPHY_PARAMETERS_VALIDATION                                                                \
    -I$(top_srcdir)/include                                                                    \
    -I$(top_srcdir)/examples/platforms                                                         \
    -I$(top_srcdir)/src/core                                                                   \
    $(NULL)

NXP_SDK_CPPFLAGS                                                                             = \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source                                        \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Interface                                     \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/NVIC                                   \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/USB                                    \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/USB/Common                             \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/USB/Class                              \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/USB/Driver                             \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/USB/APP/CDC                            \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/USB/APP/Descriptor                     \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/NVM                                    \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/Common/Interface                       \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/PLM/Source/Common/MC1324xDrv                      \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/MacPhy/Phy/Interface                              \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/MacPhy/Interface                                  \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/BeeApps/Configure                                 \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/SSM/FunctionLib/Interface                         \
    -I$(top_srcdir)/third_party/nxp/kw2x/bsp/SSM/TS/Interface                                  \
    -Wno-unknown-pragmas                                                                       \
    -Wno-sign-compare                                                                          \
    -Wno-unused-function                                                                       \
    -Wno-unused-parameter                                                                      \
    -Wno-empty-body                                                                            \
    #-Wno-shadow                                                                               \
    #-Wno-type-limits                                                                          \
    #-Wno-unused-variable                                                                      \
    #-Wno-missing-field-initializers                                                           \
    #-fno-strict-aliasing                                                                      \
    $(NULL)

PLATFORM_SOURCES                                                                             = \
    alarm.c                                                                                    \
    atomic.c                                                                                   \
    cstartup.c                                                                                 \
    cortex-m_cstartup.c                                                                        \
    flash.c                                                                                    \
    led.c                                                                                      \
    logging.c                                                                                  \
    misc.c                                                                                     \
    platform.c                                                                                 \
    radio.c                                                                                    \
    random.c                                                                                   \
    uart.c                                                                                     \
    usbcdc.c                                                                                   \
    vector.c                                                                                   \
    $(NULL)

NXP_SDK_SOURCES                                                                              = \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/NVIC/Interrupt.c                        \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/GPIO/GPIO.c                             \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/RNG/RNG.c                               \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/MCG/mcg.c                               \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/Common/MC1324xDrv/MC1324xDrv.c          \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/APP/CDC/virtual_com.c               \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/APP/CDC/usb_cdc_descriptor.c        \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/APP/Descriptor/usb_descriptor.c     \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Class/usb_cdc.c                     \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Class/usb_cdc_pstn.c                \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Class/usb_hid.c                     \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Common/USB.c                        \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Common/usb_framework.c              \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Common/usb_class.c                  \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Common/UsbMsgQueue.c                \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Driver/usb_driver.c                 \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Driver/usb_dci.c                    \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/USB/Driver/usb_sim_settings.c           \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/NVM/NV_FlashHAL.c                       \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/LED/LED.c                               \
    @top_builddir@/third_party/nxp/kw2x/bsp/PLM/Source/TMR/Timer.c                             \
    @top_builddir@/third_party/nxp/kw2x/bsp/SSM/TS/Source/TS_kernel.c                          \
    @top_builddir@/third_party/nxp/kw2x/bsp/SSM/FunctionLib/Source/FunctionLib.c               \
    @top_builddir@/third_party/nxp/kw2x/bsp/MacPhy/Phy/Source/PhyPacketProcessor.c             \
    @top_builddir@/third_party/nxp/kw2x/bsp/MacPhy/Phy/Source/PhyPlmeData.c                    \
    @top_builddir@/third_party/nxp/kw2x/bsp/MacPhy/Phy/Source/PhyISR.c                         \
    $(NULL)

if OPENTHREAD_ENABLE_DIAG
PLATFORM_SOURCES                                                                            += \
    diag.c                                                                                     \
    $(NULL)
endif

libopenthread_kw2x_a_CPPFLAGS                                                                = \
    $(NXP_COMMON_CPPFLAGS)                                                                     \
    $(NXP_SDK_CPPFLAGS)                                                                        \
    $(NULL)

libopenthread_kw2x_a_SOURCES                                                                 = \
    $(PLATFORM_SOURCES)                                                                        \
    $(NXP_SDK_SOURCES)                                                                         \
    $(NULL)

noinst_HEADERS                                                                               = \
    atomic.h                                                                                   \
    led.h                                                                                      \
    platform-kw2x.h                                                                            \
    usbcdc.h                                                                                   \
    $(NULL)

Dash                                                                                         = -
libopenthread_kw2x_a_LIBADD                                                                  = \
    $(shell find $(top_builddir)/examples/platforms/utils $(Dash)type f $(Dash)name "*.o")

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
