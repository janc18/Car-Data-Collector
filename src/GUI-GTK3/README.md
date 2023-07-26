# Steps to create and test GUI's on the Raspberry pi

## Nedded dependencies

- libgtk-3-0
- libgtk-3-dev
- cmake
- gcc
- xorg
- gtk-3-examples _(optionally)_

## Gui Raspberry Pi test

*The actual setup is a Raspberry Pi Zero with a HDMI screen (800x600)

To test if the GUI is correctly rendered, I connected through ssh, but it's needed to make a few changes before: 

1. Add the next line to this file **/etc/.rc.local**

```sh

export DISPLAY=:0 

```
2. Clone this repository

3. Create a new folder under GUi-GTK3 with the name build

4. cd to build

5. Execute:

```sh
cmake
make
```
## How to run the GUI without windows manager

In the folder build, after the execution of the commands shown before, run:
```sh
sudo startx ./CarDataCollector 
```

## First GUI version

This version will just contain the following elements:

- 3 Level bar indicator to view in graphical way the data
- 3 Labels with raw values for each axis
