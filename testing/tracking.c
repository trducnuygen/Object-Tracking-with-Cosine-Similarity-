#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

#include "object tracking.h"
#include <time.h>

unsigned char *generate_image_memory(int size)
{
    return (unsigned char *)calloc(size, sizeof(unsigned char));
}



int main ()
{
    // declare variables
    
    char *path_template = "template.jpg";
    char path_image[] = "./images/img62.jpg";
    char path_image1[] = "./images/img1.jpg";

    char save_path[] = "./new.jpeg";

    // read image data and store all the width, height, channel
    int width_template, height_template, width_image, height_image, channel;

    unsigned char *template = stbi_load(path_template, &width_template, &height_template, &channel, 0);

    unsigned char *image = stbi_load(path_image, &width_image, &height_image, &channel, 0);

    Stats Template, Image;
    Template.width = width_template;
    Template.channel = channel;
    Template.height = height_template;

    Image.width = width_image;
    Image.channel = channel;
    Image.height = height_image;

    if (template == NULL || image == NULL)
    {
        printf("\nError in loading the image\n");
        exit(1);
    }

    printf("Image: Width: %d\nHeight: %d\nChannel: %d\n", width_image, height_image, channel);
    printf("Template: Width: %d\nHeight: %d\nChannel: %d\n", width_template, height_template, channel);
    
    
    // save image
    int length_template = width_template * height_template * channel;
    unsigned char* vector = (unsigned char*)calloc(length_template, sizeof(unsigned char));

    time_t start, end;
    double diff;
    Coord LastTopLeft;
    LastTopLeft.i = 105;
    LastTopLeft.j = 293;

    /*
    start = clock();
    Coord TopLeft = re_tracking_object(image, template, vector, Template, Image, LastTopLeft);
    Coord TopLeft = tracking_object_fast(image, template, vector, Template, Image);
    update_template(vector, image, LastTopLeft, Template, width_image);
    end = clock();
    
    // i = 105, j = 293
    //printf("i: %d\nj: %d", TopLeft.i, TopLeft.j);
    
    diff = ((double) (end - start)) / CLOCKS_PER_SEC; 
    printf("Time taken: %ds",diff);
    
    // stbi_write_png(save_path, width_template, height_template, channel, template, width_template * channel);
    stbi_write_png(save_path, width_image, height_image, channel, image, width_image * channel);

    // int length_image = width_image * height_image * channel;
    unsigned char* copy = (unsigned char*)calloc(length_image, sizeof(unsigned char));
    */
    Coord TopLeft;
    TopLeft.i = 105;
    TopLeft.j = 293;

    Coord bottom_right_index;
    bottom_right_index.i = TopLeft.i + height_template;
    bottom_right_index.j = TopLeft.j + width_template;
    int length_image = width_image * height_image * channel;
    
    // copy and draw rect
    unsigned char* copy = (unsigned char*)calloc(length_image, sizeof(unsigned char));
    copy_image(image, copy, length_image);
    //rect(copy, TopLeft, Template, Image);
    
    // write output image
    // stbi_write_png(save_path, width_image, height_image, channel, copy, width_image * channel);
    // printf("New image saved to %s\n", save_path);
    update_template(template, image, TopLeft, Template, Image.width);
    image = stbi_load(path_image1, &width_image, &height_image, &channel, 0);
    free(vector);
    free(copy);
    return 0;
}
