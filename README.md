# Border-Detection-in-Hardware

   The project develops the possibility of taking pictures with an infrared camera and obtaining borders information of the picture via processing through a TI MSP432 microcontroller. The project is part of an effort towards the automated counting of moths in traps for plague control in the countryside. The project was developed in C, through Code Composer Studio for Ubuntu.  
   
The whole process can be divided in the following steps:  

Establish UART communications for PC - MSP432 and MSP432 - Camera.  
Take picture with LinkSprite JPEG Color Camera by sending commands through MSP432.  
Receiving JPEG picture through UART in MSP432 and decompressing to RAW using picojpeg library.  
Applying and perfecting fuzzy logic border detection algorithm for RAW picture.  
Sending results via UART to PC.  
