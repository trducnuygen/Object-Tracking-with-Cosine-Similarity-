#define STB_IMAGE_IMPLEMENTATION
#include "./header/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./header/stb_image_write.h"

#include "./header/object tracking.h"

int main()
{
     char path[50];
     char writepath[1000];
     int n_img = 63;
     unsigned char *image;
     int width_image, height_image, channel;
     int height, width;
     width = 160 + 100;
     height = 214 + 100;
     int x, y;
     unsigned char *temp = (unsigned char*)calloc(height*width*3, sizeof(unsigned char));
     int index = 0;

     for (int i = 0; i < n_img; i++)
     {
          if (i == 0 || i == 8 || i == 16 || i == 24 || i == 32 || i == 40 || i == 48 || i == 56 || i == 62)
          {
               index = 0;
               sprintf(path, "./result_images/New%d.jpg", i);
               image = stbi_load(path, &width_image, &height_image, &channel, 0);
               printf("\nread: done");
               // 160, 214, 105, 293
               y = 105 - 50;
               x = 293 - 50;
               for (int a = y; a < y + height; a++)
               {
                    for (int b = x; b < x + width; b++)
                    {
                         for (int k = 0; k < channel; k++)
                         {
                              temp[index] = image[a*width_image*channel + b*channel + k];
                              index++;
                         }
                    }
               }
               printf("\ntemp done");
               sprintf(writepath, "./for_report_%d.jpg", i);
               stbi_write_png(writepath, width, height, channel, temp, width * channel);
               printf("\nwrite done");
          }
     }
     free(temp);
     return 0;
}