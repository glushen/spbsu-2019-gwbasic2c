Транслятор из языка GW-BASIC в C++.

### Сборка и запуск
Склонируйте и соберите проект:
```
git clone https://github.com/glushen/gwbasic2c.git
cd gwbasic2c
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
```
После чего запустите транслятор: `gwbasic2c input.bas output.cpp`.