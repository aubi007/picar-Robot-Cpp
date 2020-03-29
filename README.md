# picar-Robot-Cpp

The project goal is to create a basis for autonomous drive for a SunFounder's piCar-V

The vendor's python tutorial was used as a base for the software
python/django web server to provide a remote control for the piCar
PCA9685 C++ library forked from https://github.com/TeraHz/PCA9685

The aim is to:
* Use python only for the web server, providing picar's vide ostream and passing user's input to the bot
* Use C++ to implement all the sensor and servo manipulation and the bot logic

Video stream analysis utilizes openCV 4

Debug version
- Builds and debugs on Windows using MS Visual studio Community 2019
- Pyhton web server works as well as video streaming and analysis
- Picar specific HW (servos) not available :)

Full version
- Builds on Raspberry PI using cmake and make
- Should work

MS Windows setup:
- Install openCV 4.x
  - download from https://opencv.org/releases/ 
  - unpack, f.eg. to d:\Work\opencv\
  - add opencv to you system PATH variable (D:\Work\opencv\build\x64\vc15\bin\)
- Install MS Visual Studio Communiy 2019, include Python development (Python 3 32-bit and 64-bit) and Desktop Development with C++
- Within python environment tab do the following:
  - upgrade pip 
  - setup Django 2.x  (pip install django<3.0.0)
  - setup pybind11 (pip install pybind11)
- RobotApp project configured to openCV at d:\Work\opencv\, change paths in project properties


Raspberry Pi setup:
- openCV 4.1 - using slightly adjusted tutorial from https://www.pyimagesearch.com/2018/09/26/install-opencv-4-on-your-raspberry-pi/
