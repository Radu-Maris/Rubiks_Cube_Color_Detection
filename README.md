In this project I implemented a live color detector in which I use the camera from my laptop to get live footage and than find the color of each square form the Rubik cube faces.
A black grid is displayed on top of the video footage to indicate to the user where the cube should be positioned.
Even if the cube is not fully in in the center or slightly rotated and out of bounds, the algorithm should detect the color and display it, but not as accurate.
This project is adapted to my laptop camera and in testing it on different one might result in slightly different results due to different color reproduction.

Approach:
  *Take each frame of the video and apply the following transformations:
  *Convert from BGR (Blue, Green, Red) to HSV (Hue, Saturation, Value).
  *Create masks for each color of the cube using a color range.
  *Create a temporary frame which is only the final solid colors of the image.
  *Press a key to check which color is predominant in each cell (each one of the 9 sections delimited by the borders) and display it in a separate image.
  *Create the final visualization for each face of the cube.

![image](https://github.com/user-attachments/assets/2871b788-1a9c-49ac-9ab7-d759876651b0)

![image](https://github.com/user-attachments/assets/172f17f8-1fdb-415b-aabb-c546759647e0)

![image](https://github.com/user-attachments/assets/a7934d47-9ed1-4eec-927c-bdf16fffbdc3)
