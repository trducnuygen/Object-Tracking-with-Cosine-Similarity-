#define STB_IMAGE_IMPLEMENTATION
#include "./header/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./header/stb_image_write.h"

#include "./header/object tracking.h"
#include <string.h>

int main ()
{    
     // set up variables for setting up paths for reading images.
     int n_img = 63;
     char *path_template = "gray_template.jpg";
     char read_gray_path[30];
     char read_path[30];
     char write_path[30];
              

     // for 1st iteration
     char graypath0[30] = "./gray/Gray0.jpg";
     char path0[] = "./images/img0.jpg";
     char write0[30] = "./result_images/New0.jpg";

     // set up var for reading images.
     unsigned char *gray_image, *template, *image;
     int width_template, height_template, width_image, height_image, channel;

     // read template
     printf("Image 0: \n");
     printf(">> Reading image, template: ");
     template = stbi_load(path_template, &width_template, &height_template, &channel, 0);
     gray_image = stbi_load(graypath0, &width_image, &height_image, &channel, 0);
     image = stbi_load(path0, &width_image, &height_image, &channel, 0);
     printf("Done\n");
     
     if (template == NULL | image == NULL)
     {
          printf("\nError in loading the image\n");
          exit(1);
     }

     // get stat for template and image for later uses.
     Stats Template, Image;
     Template.width = width_template;
     Template.channel = channel;
     Template.height = height_template;

     Image.width = width_image;
     Image.channel = channel;
     Image.height = height_image;
     
     // get length
     int length_template = width_template * height_template;
     int length_image = width_image * height_image * channel;

     
     // we getting initial coordinate for template in img0
     
     // step 1: set up variables
     Coord TopLeft, LastTopLeft;
     unsigned char* vector = (unsigned char*)calloc(height_template * width_template, sizeof(unsigned char));
     unsigned char* saved_image = (unsigned char*)calloc(length_image, sizeof(unsigned char));
     unsigned char* original_template = (unsigned char*)calloc(height_template * width_template, sizeof(unsigned char));
     
     // register value for original template
     copy_image(template, original_template, length_template);

     // copy image to saved image, graying image
     printf(">> Copying image: ");
     copy_image(image, saved_image, length_image);
     printf("Done\n");

     // step 2: get coordinate for TopLeft
     printf(">> Tracking: ");     
     TopLeft = tracking_object_fast(gray_image, template, vector, Template, Image);
     printf("Done\n");
     
     // check
     printf(">> Coord: i = %d, j = %d\n", TopLeft.i, TopLeft.j);

     // Step 3: draw rect in saved_image at coord TopLeft
     printf(">> Drawing box: ");
     rect(saved_image, TopLeft, Template, Image);
     printf("Done\n");
     
     // write.
     printf(">> Outputting image: ");
     // char* graypath = "./Gray0.jpg";

     stbi_write_png(write0, width_image, height_image, channel, saved_image, width_image * channel);
     //copy_image(grayed_image, saved_grayed_image, height_image * width_image);
     // stbi_write_png(graypath, width_image, height_image, 1, gray_image, width_image);
     
     printf("Done\n");

     
     // Step 4: update template 
     printf(">> Updating template: ");
     update_template(template, gray_image, TopLeft, Template, Image.width);
     printf("Done\n");
     
     // free(grayed_template);
     /** Steps below is to be like this:
      * 1. get path and read image.
      * 2. tracking template and get Topleft based on LastTopLeft using re_tracking_fast
      * 3. draw bounding box using rect() and write the drawn image to new_i.jpg
      * 4. update template.
      * 5. repeat step 1.
     */
     
     for (int i = 1; i < n_img; i++)
     {
          // Step 1: 
          printf("Image %d: \n", i);

          // reset path
          sprintf(read_path, "./images/img%d.jpg", i);
          sprintf(read_gray_path, "./gray/Gray%d.jpg", i);
          sprintf(write_path, "./result_images/New%d.jpg", i);

          // read images.
          printf("%s\n", read_path);
          printf(">> Reading image: ");
          gray_image = stbi_load(read_gray_path, &width_image, &height_image, &channel, 0);
          image = stbi_load(read_path, &width_image, &height_image, &channel, 0);
          
          if (image == NULL)
          {
              printf("\nError in loading the image\n");
              exit(1);
          }
          printf("Done\n");
          
          // copying image to saved_image for processing.
          printf(">> Copying image: ");
          copy_image(image, saved_image, length_image);
          printf("Done\n");
          
          // Step 2: tracking template and get Topleft based on LastTopLeft using re_tracking_fast
          LastTopLeft.i = TopLeft.i;
          LastTopLeft.j = TopLeft.j;

          printf(">> Tracking: ");
          TopLeft = re_tracking_object(gray_image, template, original_template, vector, Template, Image, LastTopLeft);
          printf("Done\n");
          printf(">> Coord: i = %d, j = %d\n", TopLeft.i, TopLeft.j);

          // Step 3: draw bounding box using rect() and write the drawn image to new_i.jpg
          printf(">> Drawing box: ");
          rect(saved_image, TopLeft, Template, Image);
          printf("Done\n");

          // write result out
          printf(">> Outputing drawn image: ");
          stbi_write_png(write_path, width_image, height_image, channel, saved_image, width_image * channel);
          printf("Done\n");
          
          // Step 4: update template.
          printf(">> Updating template: ");
          update_template(template, gray_image, TopLeft, Template, Image.width);
          printf("Done\n");
     }
     
     free(vector);
     free(saved_image);
     free(original_template);
     return 0;
}
