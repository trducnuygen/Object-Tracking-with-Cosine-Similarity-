#define STB_IMAGE_IMPLEMENTATION
#include "./header/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./header/stb_image_write.h"

#include "./header/object-tracking-without-grayscale.h"
#include <string.h>

int main ()
{    
     // set up variables for setting up paths for reading images.
     int n_img = 63;
     char *path_template = "template.jpg";
     
     char num[3];
     char read_path[30] = "./images/img";
     char write_path[100] = "./result_images_without_grayscale/New";
     char save_path_read[30] = "./images/img";
     char save_path_write[100] = "./result_images_without_grayscale/New";
         

     // for 1st iteration
     char path0[30] = "./images/img0.jpg";
     char write0[100] = "./result_images_without_grayscale/New0.jpg";

     // set up var for reading images.
     unsigned char *image, *template;
     int width_template, height_template, width_image, height_image, channel;

     // read template
     template = stbi_load(path_template, &width_template, &height_template, &channel, 0);
     image = stbi_load(path0, &width_image, &height_image, &channel, 0);

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
     int length_template = width_template * height_template * channel;
     int length_image = width_image * height_image * channel;

     
     // we getting initial coordinate for template in img0
     printf("Image 0: \n");
     // step 1: set up variables
     Coord TopLeft, LastTopLeft;
     unsigned char* vector = (unsigned char*)calloc(length_template, sizeof(unsigned char));
     unsigned char* saved_image = (unsigned char*)calloc(length_image, sizeof(unsigned char));
     unsigned char* original_template = (unsigned char*)calloc(length_template, sizeof(unsigned char));
     
     // copy image to saved image
     printf(">> Copying image: ");
     copy_image(image, saved_image, length_image);
     copy_image(template, original_template, length_template);
     printf("Done\n");
     
     // step 2: get coordinate for TopLeft
     printf(">> Tracking: ");     
     LastTopLeft.i = 105;
     LastTopLeft.j = 293;
     // TopLeft = re_tracking_object(image, template, vector, Template, Image, LastTopLeft);
     TopLeft = tracking_object_fast(image, template, vector, Template, Image);
     
     printf("Done\n");
     
     // check
     printf(">> Coord: i = %d, j = %d\n", TopLeft.i, TopLeft.j);

     // Step 3: draw rect in saved_image at coord TopLeft
     printf(">> Drawing box: ");
     rect(saved_image, TopLeft, Template, Image);
     printf("Done\n");
     
     // write.
     printf(">> Outputting image: ");
     stbi_write_png(write0, width_image, height_image, channel, saved_image, width_image * channel);
     printf("Done\n");

     
     // Step 4: update template 
     printf(">> Updating template: ");
     update_template(template, image, TopLeft, Template, Image.width);
     printf("Done\n");
     

     /** Steps below is to be like this:
      * 1. get path and read image.
      * 2. tracking template and get Topleft based on LastTopLeft using re_tracking_fast
      * 3. draw bounding box using rect() and write the drawn image to new_i.jpg
      * 4. update template.
      * 5. repeat step 1.
     */
     int *result = calloc(2, sizeof(int));
     for (int i = 1; i < n_img; i++)
     {
          // Step 1: 
          printf("Image %d: \n", i);

          // reset path
          sprintf(read_path, "./images/img%d.jpg", i);
          sprintf(write_path, "./result_images_without_grayscale/New%d.jpg", i);

          // read images.
          printf("%s\n", read_path);
          printf(">> Reading image: ");
          unsigned char* image_new;
          int w, h, c;

          image_new = stbi_load(read_path, &w, &h, &c, 0);
          // image = stbi_load(path0, &width_image, &height_image, &channel, 0);
          if (image_new == NULL)
          {
              printf("\nError in loading the image\n");
              exit(1);
          }
          printf("Done\n");
          
          // copying image to saved_image for processing.
          printf(">> Copying image: ");
          copy_image(image, saved_image, length_image);
          printf("Done\n");
          
          result = find_pixel(template, image, width_template, height_template, channel, width_image, height_image, channel, result[0], result[1]);
          // Step 2: tracking template and get Topleft based on LastTopLeft using re_tracking_fast
          // LastTopLeft.i = TopLeft.i;
          // LastTopLeft.j = TopLeft.j;
          TopLeft.i = result[1];
          TopLeft.j = result[0];
          printf(">> Tracking: ");
          // TopLeft = re_tracking_object(image, template, original_template, vector, Template, Image, LastTopLeft);
          printf("Done\n");
          printf(">> Coord: i = %d, j = %d\n", TopLeft.i, TopLeft.j);

          // Step 3: draw bounding box using rect() and write the drawn image to new_i.jpg
          printf(">> Drawing box: ");
          //rect(saved_image, TopLeft, Template, Image);
          printf("Done\n");

          // write result out
          printf(">> Outputing drawn image: ");
          stbi_write_png(write_path, width_image, height_image, channel, saved_image, width_image * channel);
          printf("Done\n");
          
          // Step 4: update template.
          printf(">> Updating template: ");
          //update_template(template, image, TopLeft, Template, Image.width);
          printf("Done\n");
     }
     
     free(vector);
     free(saved_image);
     free(original_template);
     free(result);
     return 0;
}


