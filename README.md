# Cosmic Mouse

This is 6DOF mouse for CAD, inspired by the Space Mouse by 3DConexion, it uses 3x 3D Hall effect sensors over I2C with a custom single-piece isotropic spring which floats the knob in space. Firmware compensates for magnetic cross-talk by using a physics-based magnetic dipole model and the Levenberg-Marquardt algorithm to iteratively solve for the knob's pose by minimizing the difference between measured and predicted magnetic fields. Its high accuracy is achieved by enforcing a rigid-body constraint on the magnets and using an over-determined system to filter noise and absorb hardware imperfections. The enclosure and knob are resin printed for smoothness and accuracy, the diffuser is also resin printed which helps to de-peak the LED ring. The knob shaft is printed in PETG and sping in TPU. I also used a steel base weight and non-slip silicone pad to keep it planted during movements. This project is sponsored by [PCBWay](https://www.pcbway.com/).

The Cosmic Mouse is *likely capable* of emulating a real Space Mouse... On an unrelated note, you should see firmware/src/config.h for some random funny strings. 

Pending Video and Blog Post.


### Working Model:
![](/media/final_on_desk.jpg)

### Cross Section:
![](/media/cross-section.png)

### Isotropic Spring:
![](/media/spring.png)


