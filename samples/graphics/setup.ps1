$file="SDL2-devel-2.0.20-VC.zip"

Invoke-WebRequest -Uri "https://www.libsdl.org/release/SDL2-devel-2.0.20-VC.zip" -OutFile $file
Expand-Archive -Path $file -DestinationPath WindowsSDL2
