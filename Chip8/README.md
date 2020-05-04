# Chip 8 模擬器 Chip 8 Emulator  
## 介紹 What is this? 
就chip8的模擬器，目前測試INVADERS,BRIX跑起來沒什麼問題，其他沒測試過不確定。  
This is a CHIP8 emulator. I have run the INVADERS and BRIX roms on the emulator, and it did work. But I have no idea how are other roms.  

## 建置環境 Build Environment  
OS: Windows 10 Education  
Compiler: 
g++ (GCC) 4.9.3  
GNU Make 3.81  
OpenGL library:  
freeglut 3.0.0-1.mp for MSVC  
glew 2.1.0  
## 如何編譯? How to build?  
在Emulators/Chip8資料夾下執行make:  
Run make in Emulators/Chip8:
```
.make
```

## 執行方式
開啟cmd，輸入以下指令(INVADERS可換成任何rom檔名稱):  
Open cmd in this directory and input the instruction below:  
```
./a c8games/INVADERS
```  
## 操作方式  
數字鍵 1-9，英文字母 A-F  
keyboard 1-9 and A-E  
INVADERS的操作方法為:46左右移動，5發射  
4 and 6 moves left and right, and 5 is fire in INVADERS.  
BRIX的操作方法是:46移動  
4 and 6 moves left and right in BRIX.  
## ROM來源  Where the roms came from?  
https://github.com/cheukyin699/chip-8-tools

## 實際運行影片 Demo  
https://youtu.be/_KT1Q2-vluM  

## 參考資料  
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM  
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/  
