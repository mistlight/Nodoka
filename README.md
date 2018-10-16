# Project Nodoka

A free and open source cross platform Audiobook reader. Written in Qt/C++, and makes use of `libvlc` to play audio.

# Build Notes

Make sure that `CMAKE_PREFIX_PATH` points to the directory of Qt.
For example, if your Qt installation is at `/opt/qt`, then
`CMAKE_PREFIX_PATH` should be set to `/opt/Qt/5.11.2/gcc_64`

If you are building under Ubuntu and get the error message related to `libdrm`, try
installing `gl` via:

```
sudo apt-get install build-essential libgl1-mesa-dev
```

# License

See LICENSE