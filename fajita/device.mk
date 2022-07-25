
#
# Copyright (C) 2011 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


#
#The files wont copy over as a block of PRODUCT_COPY_FILES so copy them one by one
#

#fstab ramdisk
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/fstab.ramdisk:$(TARGET_COPY_OUT_RAMDISK)/fstab.fajita

#fstab vendor
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/fstab.ramdisk:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.fajita

#audio
PRODUCT_COPY_FILES += \
device/generic/sdm845/shared/etc/audio.sdm845.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio.fajita.xml

#init sdm845
PRODUCT_COPY_FILES += \
device/generic/sdm845/shared/init.sdm845.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.fajita.rc

#init sdm845 usb
PRODUCT_COPY_FILES += \
device/generic/sdm845/shared/init.sdm845.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.fajita.usb.rc

#keylayout
PRODUCT_COPY_FILES += \
device/generic/sdm845/shared/key_layout.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/fajita.kl

#Amplifier Firmware :/
PRODUCT_COPY_FILES += \
    device/generic/sdm845/fajita/tfa98xx.cnt:$(TARGET_COPY_OUT_VENDOR)/etc/firmware/tfa98xx.cnt


# Build generic Audio HAL
PRODUCT_PACKAGES := audio.primary.fajita

# Build generic lights HAL
PRODUCT_PACKAGES += lights.fajita
