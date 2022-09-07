## SDL integration

This sample demonstrates integration with C. SDL functions are exposed as Lisp functions using _addBuiltInFunction_. To build on Windows, run the the setup64.bat or setup32.bat followed by build64.bat or build32.bat scripts. On unix, simply run make (you would need SDL2 - on Ubuntu, you could install it using - apt install libsdl2-dev ).
Run the sample as follows -
```bash
./lisp sample.l
```
This should open up an SDL window where you can type in (demo) and hit enter.
