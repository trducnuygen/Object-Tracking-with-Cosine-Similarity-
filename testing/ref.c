#define STB_IMAGE_IMPLEMENTATION
#include "./header/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./header/stb_image_write.h"

#include "./header/cos_similarity.h"

int main()
{
    int t_width, t_height, t_channel;
    int i_width, i_height, i_channel;
    char template_path[] = "./template.jpg";
    char write_path[100] = "./result_images/";
    int curr_img = 0;
    int *result = calloc(2, sizeof(int));

    unsigned char *template_image = stbi_load(template_path, &t_width, &t_height, &t_channel, 0);
    // char test[50]= "test";
    // char test2[50]= "test";
    // itoa(12, test2, 10);
    // strcat(test, test2);
    // printf("%s", test);
    // Size of image
    // printf("%d %d %d\n", t_width, t_height, t_channel);

    char img_path[100] = "./images/img";

    // To string from int
    char tmp[100];
    itoa(curr_img, tmp, 10);

    char img[100] = "";
    strcat(img, img_path);
    strcat(img, tmp);
    strcat(img, ".jpg");

    char dest[100] = "";
    strcat(dest, write_path);
    strcat(dest, tmp);
    strcat(dest, ".jpg");

    // Read + print the first image
    unsigned char *image = stbi_load(img, &i_width, &i_height, &i_channel, 0);
    // printf("%d %d %d\n", i_width, i_height, i_channel);
    result = find_pixel(template_image, image, t_width, t_height, t_channel, i_width, i_height, i_channel, 275, 95);
    stbi_write_png(dest, i_width, i_height, i_channel, image, i_width * i_channel);
    printf("img %d\n", curr_img);
    curr_img++;

    while (curr_img <= 62)
    {
        char w_tmp[100];
        itoa(curr_img, w_tmp, 10);

        char w_img[100] = "";
        strcat(w_img, img_path);
        strcat(w_img, w_tmp);
        strcat(w_img, ".jpg");

        char w_dest[100] = "";
        strcat(w_dest, write_path);
        strcat(w_dest, w_tmp);
        strcat(w_dest, ".jpg");
        printf("img %d\n", curr_img);
        image = stbi_load(w_img, &i_width, &i_height, &i_channel, 0);
        result = find_pixel(template_image, image, t_width, t_height, t_channel, i_width, i_height, i_channel, result[0] - 15, result[1] - 15);
        stbi_write_png(w_dest, i_width, i_height, i_channel, image, i_width * i_channel);

        curr_img++;
    }
    stbi_write_png(write_path, i_width, i_height, i_channel, image, i_width * i_channel);
    printf("New image save to %s\n", write_path);
}