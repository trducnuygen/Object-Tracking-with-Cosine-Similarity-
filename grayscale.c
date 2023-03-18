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

     char path0[30] = "./images/img0.jpg";
     char path[30];
     char graypath[30];
     char *path_template = "template.jpg";

     // set up var for reading images.
     printf("Image 0: \n");
     int width_image, height_image, channel, width_template, height_template;
     unsigned char *image = stbi_load(path0, &width_image, &height_image, &channel, 0);
     unsigned char *template = stbi_load(path_template, &width_template, &height_template, &channel, 0);
     
     // get stat for template and image for later uses.
     Stats Template, Image;
     Template.width = width_template;
     Template.channel = channel;
     Template.height = height_template;

     Image.width = width_image;
     Image.channel = channel;
     Image.height = height_image;

     // copy image to saved image, graying image
     //unsigned char* grayed_image = grayscale(image, Image);
     unsigned char* grayed_template = grayscale(template, Template);

     // write.
     printf(">> Outputting image: ");
     
     char grayyy[] = "./gray_template.jpg";
     
     stbi_write_png(grayyy, width_template, height_template, 1, grayed_template, width_template);
     
     
     printf("Done\n");
     
     for (int i = 0; i < n_img; i++)
     {
          sprintf(path, "./images/img%d.jpg", i);
          unsigned char* img = stbi_load(path, &width_image, &height_image, &channel, 0);

          if (image == NULL)
          {
               printf("\nError in loading the image\n");
               exit(1);
          }
          unsigned char* grayed_image = grayscale(img, Image);
          sprintf(graypath, "./gray/Gray%d.jpg", i);
          stbi_write_png(graypath, width_image, height_image, 1, grayed_image, width_image);
     }
     printf("done grayscaling.");
     return 0;
}
