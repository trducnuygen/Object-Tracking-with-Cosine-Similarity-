#include <stdio.h>
#include <math.h>

// RGB : 20 255 10
unsigned char *find_pixel(unsigned char *template, unsigned char *image, int t_width, int t_height, int t_channel,
                int i_width, int i_height, int i_channel, int x, int y)
{
    unsigned char *result = calloc(2, sizeof(int));
    result[0] = 0;
    result[1] = 0;
    // Calculate sum
    // 20 to left, 20 to right
    // int x = 275;
    // int y = 95;
    // int t_height_10 = t_height + 20;
    // int t_width_10 = t_width + 20;

    // tmp1 = sqrt(tmp1);
    // tmp2 = sqrt(tmp2);
    double max = 0;
    for (int a = y; a < y + 20; a++)
    {
        for (int b = x; b < x + 20; b++)
        {
            // strating_pos = starting position
            int starting_pos = a * i_width * i_channel + b * i_channel;
            double sum = 0;
            double denominator = 0;
            double tmp1 = 0;
            double tmp2 = 0;

            for (int i = 0; i < t_height; i++)
            {
                for (int j = 0; j < t_width; j++)
                {
                    for (int k = 0; k < t_channel; k++)
                    {
                        sum += image[starting_pos + i * i_width * i_channel + j * t_channel + k] * template[i * t_width * t_channel + j * t_channel + k];
                        tmp1 += pow(image[starting_pos + i * i_width * i_channel + j * t_channel + k], 2);
                        tmp2 += pow(template[i * t_width * t_channel + j * t_channel + k], 2);
                    }
                }
            }
            denominator = sqrt(tmp1) * sqrt(tmp2);
            double cos_sim = sum / denominator; // calculate cosine similarity
            if (cos_sim > max)
            {
                max = cos_sim;
                result[0] = b;
                result[1] = a;
            }
        }
    }

    x = result[0];
    y = result[1];
    // Copy into the new template
    for (int i = y; i < y + t_height; i++)
    {
        for (int j = x; j < x + t_width; j++)
        {
            int t_location = (i - y) * t_width * t_channel + (j - x) * t_channel;
            int i_location = i * i_width * i_channel + j * i_channel;
            for (int k = 0; k < t_channel; k++)
            {
                template[t_location + k] = image[i_location + k];
            }
        }
    }

    for (int i = x; i < x + t_width; i++)
    {
        int tmp = y * i_width * i_channel + i * i_channel;
        image[tmp + 0] = 255;
        image[tmp + 1] = 255;
        image[tmp + 2] = 0;
    }

    for (int i = y; i < y + t_height; i++)
    {
        int tmp = i * i_width * i_channel + x * i_channel;
        image[tmp + 0] = 255;
        image[tmp + 1] = 255;
        image[tmp + 2] = 0;
    }

    for (int i = x; i < x + t_width; i++)
    {
        int tmp = (y + t_height) * i_width * i_channel + i * i_channel;
        image[tmp + 0] = 255;
        image[tmp + 1] = 255;
        image[tmp + 2] = 0;
    }

    for (int i = y; i < y + t_height; i++)
    {
        int tmp = i * i_width * i_channel + (x + t_width) * i_channel;
        image[tmp + 0] = 255;
        image[tmp + 1] = 255;
        image[tmp + 2] = 0;
    }

    return result;
}