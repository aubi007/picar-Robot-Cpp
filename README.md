# picar-Robot-Cpp

The project goal is to create a basis for autonomous drive for a SunFounder's piCar-V

The vendor's python tutorial was used as a base for the software
python/django web server to provide a remote control for the piCar

The aim is to:
* Use python only for the web server, providing picar's vide ostream and passing user's input to the bot
* Use C++ to implement all the sensor and servo manipulation and the bot logic

Video stream analysis utilizes openCV 4

Debug version
- builds and debugs on Windows using MS Visual studio Community 2019
- pyhton web server works as well as video streaming and analysis
- picar specific HW (servos) not available :)

Full version
- builds on Raspberry PI using cmake and make
- should work

MS Windows setup:
- install MS Visual Studio Communiy 2019, include Python development (Python 3 32-bit and 64-bit) and Desktop Development with C++
- Within python environment tab do the following:
  - upgrade pip 
  - setup openCV 4.x
  - setup Dhango 2.x  (pip install django<3.0.0)

Raspberry Pi setup:
* openCV 4.1 - using slightly adjusted tutorial from https://www.pyimagesearch.com/2018/09/26/install-opencv-4-on-your-raspberry-pi/
