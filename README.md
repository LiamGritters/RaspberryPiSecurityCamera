# RaspberryPiSecurityCamera

Motion Detection and Logging Security Camera Built on a Raspberry Pi

## Hardware 
- Raspberry Pi 3
- Raspberry Pi Camera Module 2

## Dependencies 
- opencv (version 4.5 currently supported)
- v4l2loopback (https://wiki.archlinux.org/title/V4l2loopback)
- build-essential
- ffmpeg
- v4l2tools

Install these dependencies before attempting to build 
```
sudo apt-get install libopencv-dev build-essential v4l2loopback-dkms
```

Install V4L2Tools
```
git clone https://github.com/mpromonet/v4l2tools
cd v4l2tools
make install
```


## Building 
Currently the only supported on the Raspberry Pi 3. You must also have a complier that supports C++11

clone this repository
```
git clone https://github.com/LiamGritters/RaspberryPiSecurityCamera.git

cd cpp

make

sudo cp ./bin/RaspberryPiMotionDetection /usr/local/bin/
```

## V4L2Loopback

Add virtual v4l2loopback video device 8 & 9
```
sudo echo "v4l2loopback" > /etc/modules-load.d/v4l2loopback.conf 
sudo echo "options v4l2loopback video_nr=8,9" > /etc/modprobe.d/v4l2loopback.conf
sudo update-initramfs -c -k $(uname -r)
reboot
```

Note: if there are issues with the kernel version please follow instructions here https://github.com/RPi-Distro/rpi-source

## Hosting 

Follow the instruction below to set up web hosting on your raspberry pi
https://www.instructables.com/Host-your-website-on-Raspberry-pi/

## Simple Webpage

copy the contents of the html folder into /var/www/html/ 
```
cd html
cp ./* /var/www/html/
```

## Systemd Service

Add systemd service to enable functionality to start on boot-up of your raspberry pi
```
sudo cp ./*.service /etc/systemd/system/

sudo systemctl daemon-reload

sudo systemctl enable MotionDetection.service
sudo systemctl enable Compression.service
sudo systemctl enable Dash.service

sudo systemctl start MotionDetection.service
sudo systemctl start Compression.service
sudo systemctl start Dash.service

```

