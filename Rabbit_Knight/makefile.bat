@echo off 

IF EXIST "rk.o" (
  del "rk.o"
) ELSE (
  echo "Creating rk .o"
)

IF EXIST "rk.gb" (
  del "rk.gb"
) ELSE (
  echo "Creating rk .gb"
)

cd C:\Users\Jelli\OneDrive\Desktop\Rabbit_Knight

rgbasm.exe -o rk.o rk.asm
rgblink.exe -o rk.gb rk.o
rgblink.exe -d rk.o -m rk.map
rgbfix.exe -v rk.gb

echo "Program complete"
