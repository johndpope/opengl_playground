Welcome to my OpenGL playground! This currently has my OpenGL visualization project I played around with for:

*EE 590A - GPU Accelerated Scientific Visualization Techniques @ University of Washington*

![Alt Text](https://media.giphy.com/media/dagWRDJLkxaN3WY4X1/giphy.gif)

The control available in the visualization are:

Key | Control
----|--------
A | Walk left
S | Walk back
W | Walk forward
D | Walk right
X | Rotate shape about X axis
Y | Rotate shape about Y axis
Z | Rotate shape about Z axis
\+ | Scale up shape
\- | Scale down shape
J | Rotate camera to the left
L | Rotate camera to the right
U | Move contour plane up
I | Move contour plane down
N | Increase the iso value
M | Decrease the iso value

For convenience the executable program can be found here: https://github.com/samuraijourney/opengl_playground/blob/master/executable.zip

There are *2* different run modes:
* Test visualization
* Bonzai tree visualization

Unfortunately due to lack of a compute shader implementation of marching cubes on GPU, the bonzai tree visualization is VERY slow to load the mesh. Once the mesh is loaded however it can be controlled fine but if the iso value is changed the algorithm will run again to recompute the mesh resulting in more VERY slow loading times...

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/sDGsm_bVUWo/0.jpg)](https://www.youtube.com/watch?v=sDGsm_bVUWo)
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/tlTzmjlvvZU/0.jpg)](https://www.youtube.com/watch?v=tlTzmjlvvZU)