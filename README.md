# JXcoreWindowsEmbedded
Sample Visual Studio C++ solution to embed JXcore Node.js

#### Compile JXcore for Windows
Chakra: (Win10)
> vcbuild --shared-library --engine-chakra

V8 engine:
> vcbuild --shared-library

SpiderMonkey:
> vcbuild --shared-library --engine-mozilla

For 32 bit, add `ia32` argument
