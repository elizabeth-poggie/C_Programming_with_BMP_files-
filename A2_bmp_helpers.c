/* Helper functions required to let the main program function */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>



/* This allows for the opening of a BMP image and the initialization
 * of key attributes
 */

int bmp_open( char* bmp_filename,        unsigned int *width,
	      unsigned int *height,      unsigned int *bits_per_pixel,
	      unsigned int *padding,     unsigned int *data_size,
	      unsigned int *data_offset, unsigned char** img_data ){

  *width=0;
  *height=0;
  *bits_per_pixel=0;
  *padding=0;
  *data_size=0;
  *data_offset=0;
  *img_data=NULL;

  FILE *f = fopen(bmp_filename, "rb");
  if(f == NULL){
    printf("Bad file name :,(\n");
    exit(0);
  }

  //check to see if its a BMP file

  // *ptr - is of min size size*nmeb bytes
  // size - size in bytes of each element read
  // nmeb - number of elements
  // stream - pointer to the file object

  char b,m;
  fread(&b, 1, 1, f);
  fread(&m, 1, 1, f);
  if (b != 'B' || m != 'M'){
    printf("this is not a BMP file >:(\n");
    exit(0);
  }

  unsigned int *fileSize = (unsigned int*) malloc (sizeof(unsigned int));
  fseek(f, 2, SEEK_SET);
  fread(fileSize, 1, sizeof(unsigned int), f);
  *data_size = *fileSize;
  printf("%d\n", *fileSize);

  unsigned int *doo = (unsigned int *) malloc (sizeof(unsigned int));
  fseek(f, 10, SEEK_SET);
  fread(doo, 1, sizeof(unsigned int*), f);
  *data_offset = *doo;

  unsigned int *w = (unsigned int*) malloc (sizeof(unsigned int));
  fseek(f, 18, SEEK_SET);
  fread(w, 1, sizeof(unsigned int), f);
  *width = *w;

  unsigned int *h = (unsigned int*) malloc (sizeof(unsigned int));
  fseek(f, 22, SEEK_SET);
  fread(h, 1, sizeof(unsigned int), f);
  *height = *h;

  unsigned int *bpp = (unsigned int*) malloc (sizeof(unsigned int));
  fseek(f, 28, SEEK_SET);
  fread(bpp, 1, sizeof(unsigned int), f);
  *bits_per_pixel = *bpp;

  *padding = (*w)%4;

  unsigned char *id = (unsigned char *) malloc (*fileSize);
  fseek(f,0,SEEK_SET);
  fread(id, *fileSize, 1, f);
  *img_data = id;

  fclose(f);

  return 0;
}

/* This frees up allocated memory that the bmp image was taking up
 */
void bmp_close( unsigned char **img_data ){

  if( *img_data != NULL ){
    free( *img_data );
    *img_data = NULL;
  }
}


/* This is a function designed to mask a specific region of an image
 */
int bmp_mask( char* input_bmp_filename, char* output_bmp_filename,
	      unsigned int x_min, unsigned int y_min, unsigned int x_max, unsigned int y_max,
	      unsigned char red, unsigned char green, unsigned char blue )
{

  //start trying to extract image information now
  unsigned int img_width;
  unsigned int img_height;
  unsigned int bits_per_pixel;
  unsigned int data_size;
  unsigned int padding;
  unsigned int data_offset;
  unsigned char* img_data    = NULL;

  int open_return_code = bmp_open( input_bmp_filename, &img_width, &img_height, &bits_per_pixel, &padding, &data_size, &data_offset, &img_data );

  if( open_return_code ){ printf( "bmp_open failed. Returning from bmp_mask without attempting changes.\n" ); return -1; }

 
  //first allocate memory for a new image
  char *new_img = (char *) malloc (data_size);

  //check to see if image opened properly
  if (img_data == NULL){
    printf("Image did not open properly :,(\n");
    exit(0);
  }
  memcpy(new_img, img_data, data_size);

  //now write a loop to itterate over the pixels in the mask region
  char *pixel_data = new_img + data_offset;
  unsigned int num_colors = bits_per_pixel/8;

  //check to see if x or y are out of bounds//////////////////////////////////
  if (y_min<0 || x_min<0 || img_height<y_max || img_width<x_max){
    printf("This is out of bounds.\n");
    exit(0);
  }

  //check color values to see if they are legal //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(red<0 || green<0 || blue<0){
    printf("these are illigal colour values.\n");
    exit(0);
  }

  
  for (int x=x_min; x<=x_max; x++){
    for(int y=y_min; y<=y_max; y++){
      pixel_data[y*(img_width*num_colors + padding) + x*num_colors +2] = red;
      pixel_data[y*(img_width*num_colors + padding) + x*num_colors +1] = green;
      pixel_data[y*(img_width*num_colors + padding) + x*num_colors +0] = blue;
    }
  }

  //now save this to a new bmp
  FILE *f = fopen(output_bmp_filename, "wb");
  if(f == NULL){
    printf("Error writing file to a new location\n");
    exit(0);
  }
  fwrite(new_img, data_size, 1, f);
  fclose(f);

  bmp_close( &img_data );

  return 0;
}

/* This allows for multiple bmp images to be placed together in one 
 */

int bmp_collage( char* bmp_input1, char* bmp_input2, char* bmp_result, int x_offset, int y_offset ){

  unsigned int img_width1;
  unsigned int img_height1;
  unsigned int bits_per_pixel1;
  unsigned int data_size1;
  unsigned int padding1;
  unsigned int data_offset1;
  unsigned char* img_data1    = NULL;

  int open_return_code = bmp_open( bmp_input1, &img_width1, &img_height1, &bits_per_pixel1, &padding1, &data_size1, &data_offset1, &img_data1 );

  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input1 ); return -1; }

  unsigned int img_width2;
  unsigned int img_height2;
  unsigned int bits_per_pixel2;
  unsigned int data_size2;
  unsigned int padding2;
  unsigned int data_offset2;
  unsigned char* img_data2    = NULL;

  open_return_code = bmp_open( bmp_input2, &img_width2, &img_height2, &bits_per_pixel2, &padding2, &data_size2, &data_offset2, &img_data2 );

  if( open_return_code ){ printf( "bmp_open failed for %s. Returning from bmp_collage without attempting changes.\n", bmp_input2 ); return -1; }

  //check to see if bpp is the same
  if (bits_per_pixel1 != bits_per_pixel2){
    printf("These images do not have the same bpp so i cannot do :,(\n");
    exit(0);
  }
  unsigned int bpp = bits_per_pixel1;
  //calculating new dimensions
  unsigned int width, height;
  if (x_offset>0) {
    if (img_width2 + x_offset > img_width1){
      width = (img_width2 + x_offset);
    } else {
      width = (img_width1);
    }
  }
  if (x_offset<0){
    if( img_width2 < img_width1 || img_width2 - img_width1 < abs(x_offset)){
      width = img_width1 + abs(x_offset);
    } else {
      width = img_width2;
    }
  }
  if (x_offset == 0){
    if (img_width2 > img_width1){
      width = img_width2;
    } else {
      width = img_width1;
    }
  }

  if (y_offset>0) {
    if (img_height2 + y_offset > img_height1){
      height = (img_height2 + y_offset);
    } else {
      height = (img_height1);
    }
  }
  if (y_offset<0){
    if( img_height2 < img_height1 || img_height2 - img_height1 < abs(y_offset)){
      height = img_height1 + abs(y_offset);
    } else {
      height = img_height2;
    }
  }
  if (y_offset == 0){
    if(img_height2 > img_height1){
      height = img_height2;
    } else {
      height = img_height1;
    }
  }

  //calculating padding
  unsigned int what_up_padding = ((width * bpp)/8); //total number of bites in a single row
  unsigned int padding;

  //should our padding be 0 bois?
  if(what_up_padding%4 == 0){
    padding = 0;
  } else {
    padding = (4 - what_up_padding%4);
  }
  //calculating data size
  unsigned int data_offset;
  if(data_offset1 > data_offset2){
    data_offset = data_offset1;
  } else {
    data_offset = data_offset2;
  }
  unsigned int header_size = data_offset;

  unsigned int new_img_size = (width*bpp/8 + padding)*height;
  unsigned int data_size = new_img_size + header_size;


  unsigned char *new_img = (unsigned char*) malloc (data_size);

  unsigned int img_1_col_start, img_2_col_start, img_1_row_start, img_2_row_start;
  if (x_offset > 0){
    img_1_col_start = 0;
    img_2_col_start = x_offset;
  }
  if (x_offset < 0){
    img_1_col_start = abs(x_offset);
    img_2_col_start = 0;
  }
  if (x_offset == 0){
    img_1_col_start = 0;
    img_2_col_start = 0;
  }
  if (y_offset > 0){
    img_1_row_start = 0;
    img_2_row_start = y_offset;
  }
  if (y_offset < 0){
    img_1_row_start = abs(y_offset);
    img_2_row_start = 0;
  }
  if ( y_offset == 0){
    img_1_row_start = 0;
    img_2_row_start = 0;
  }
  //generating a new image
  //fist add the new header
  if(data_offset1 > data_offset2){
    memcpy(new_img, (img_data1+0), data_offset1);
  } else {
    memcpy(new_img, (img_data2+0), data_offset2);
  }

  //update the new file size within the thing
  memcpy((new_img+2), &data_size, 4);
  memcpy((new_img+18), &width, 4);
  memcpy((new_img+22), &height, 4);

  unsigned char *pixel_data = (new_img + data_offset); //scooting it past where the headers are
  unsigned int num_colours = bpp/8;
  unsigned char *pixel_data1 = img_data1 + data_offset1;
  unsigned char *pixel_data2 = img_data2 + data_offset2;

  //next fill the image data
  for (int i=0; i<height; i++){
    for (int j=0; j<width; j++){
      if (i>=img_2_row_start && i<(img_2_row_start+img_height2) && j>=img_2_col_start && j<(img_2_col_start+img_width2)) {

	pixel_data[i*(width*num_colours + padding) + j*num_colours + 2] =
	  pixel_data2[(i-img_2_row_start)*(img_width2*num_colours + padding2) + (j-img_2_col_start)*num_colours + 2];
	pixel_data[i*(width*num_colours + padding) + j*num_colours + 1] =
	  pixel_data2[(i-img_2_row_start)*(img_width2*num_colours + padding2) + (j-img_2_col_start)*num_colours + 1];
	pixel_data[i*(width*num_colours + padding) + j*num_colours + 0] =
	  pixel_data2[(i-img_2_row_start)*(img_width2*num_colours + padding2) + (j-img_2_col_start)*num_colours + 0];

      } else if (i>=img_1_row_start && i< (img_1_row_start + img_height1) && j>=img_1_col_start && j<(img_1_col_start+img_width1)) {

	pixel_data[i*(width*num_colours + padding) + j*num_colours + 2] =
	  pixel_data1[(i-img_1_row_start)*(img_width1*num_colours + padding1) + (j-img_1_col_start)*num_colours + 2];
	pixel_data[i*(width*num_colours + padding) + j*num_colours + 1] =
	  pixel_data1[(i-img_1_row_start)*(img_width1*num_colours + padding1) + (j-img_1_col_start)*num_colours + 1];
	pixel_data[i*(width*num_colours + padding) + j*num_colours + 0] =
	  pixel_data1[(i-img_1_row_start)*(img_width1*num_colours + padding1) + (j-img_1_col_start)*num_colours + 0];
      }
    }
  }

  //time to store the new data in a new image thing
  FILE *f = fopen(bmp_result, "wb");
  if (f == NULL){
    printf("file did not open properly");
    exit(0);
  }
  fwrite(new_img, data_size, 1, f);
  fclose(f);
 
  bmp_close( &img_data1 );
  bmp_close( &img_data2 );

  return 0;
}
