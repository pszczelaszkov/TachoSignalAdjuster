# TachoSignalAdjuster
Digital signal shifter</br>
This device is for educational purposes only!</br>
Device takes 2 digital inputs, which in this case are engine and vehicle speed.</br>
As the result it can generate signal at different frequency shifted by desired amount.</br>
Usefull in case of engine/transmission changes to keep feeding tacho with correct values.</br>
As bonus it has one high current output for driving things like solenoids(constant 2A).</br>
In this example it controls servotronic power steering valve.</br>
PCB project can be found at https://easyeda.com/pszczelaszkov/TachoSignalAdjuster.</br>
Its fully designed on one side PCB, with usage of "through hole" components for ease of assembly.</br>
uC:<b>ATtiny44</b></br>
<b>IMPORTANT!</b></br>
This revision 1.0 is in beta stage so there are some bugs in design:</br>
-Do not use D2(wrong coupling)</br>
-SPI connector lacks power supply, so during programming operation power supply must be delivered through main 12v rail alongside with common ground!. 
