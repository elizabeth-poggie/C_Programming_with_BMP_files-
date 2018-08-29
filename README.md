# BMP File Experience 

This project was designed to allow students to get comfortable with binary data in C and gain experience in dealing with visual information.

## Key Functions

### BMP Helpers

These are the functions that allow for BMP file reading and closing. As well, it provides for the heavy lifting involved in image masking and collages. 


### BMP Mask 

This C file allows for user interaction with a single image. Users will select a portion of the image and change its colour.

It is run like this:

```
./bmp_mask example_input.bmp example_output.bmp x_min y_min x_max y_max R G B
```

### BMP Collage 

This C file allows for user interaction with multiple images. Users will select 2 images to combine them together.  

It is run like this:

```
 ./bmp_collage image_1.bmp image_2.bmp images_combined.bmp x_offset y_offset
```
