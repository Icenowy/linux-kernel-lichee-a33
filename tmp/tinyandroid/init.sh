#!busybox sh
echo 0 >/sys/class/android_usb/android0/enable 
echo 18d1 >/sys/class/android_usb/android0/idVendor 
echo 0003 >/sys/class/android_usb/android0/idProduct 
echo mass_storage,adb >/sys/class/android_usb/android0/functions 
echo 1 >/sys/class/android_usb/android0/enable
export PATH=/bin:/usr/bin:$PATH
adbd &
