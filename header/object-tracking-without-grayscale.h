#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Coord
{
    int i;
    int j;
} Coord;


// aim to reduce amount of parameters in image processing functions.
typedef struct Stats
{
    int width;
    int height;
    int channel;
} Stats;

/**
 * Object tracking steps:
 * load template, for loop the images path and then load them all out.
 * hmm
 * after tracking the region in img i, updating template with that region and using that to track
 * object in img i+1
 */

// ideas for tracking objects now:
// use 4 corners of bounding box to loop over everything.

// DO BE VERY CAREFUL WHEN SETTING BOTTOM RIGHT CORNER, CUS ITS 1D ARRAY!

// taking dot product
float dotprod_norm(unsigned char* vector1, unsigned char* vector2, int length)
{
    // calculate dot product of 2 vectors
    float result = 0;
    for (int i = 0; i < length; i++)
    {
        // divided by 255 to take the norm
        result += vector1[i] / 255.0 * vector2[i] / 255.0;
    }

    return result;
}

// calculate cosine similarity between 2 flatten arrays, which are divided by 255
// (taking the norm) to reduce memory consumption
// cosine similarity = dot product of 2 vectors / product of L2 norm of each vector
float similarity(unsigned char* v1, unsigned char* v2, int length)
{
    float nominator = dotprod_norm(v1, v2, length);

    double len_v1 = 0, len_v2 = 0;

    for(int i = 0; i < length; i++)
    {
        len_v1 += pow(v1[i] / 255.0, 2);
        len_v2 += pow(v2[i] / 255.0, 2);
    }
    float denominator = sqrt(len_v1) * sqrt(len_v2);

    return nominator / denominator;
}

// specify the coordinate of the box that contain the region most similar
// to the template
// input: image, template, vector, with vector size should be the same as template.
// this function now is to get approximate frame within 30 pixels near last obtained topleft.
Coord re_tracking_object(unsigned char* image, unsigned char* template, unsigned char* original_template, unsigned char* vector, Stats Template, Stats Image, Coord LastTopLeft)
{
    // setting variables
    Coord TopLeft;
    int topleft_i, topleft_j;
    float best_similarity = 0;
    int count = 0;
    float current_similarity;
    // get the height, width, channel of image and template through stats
    int height_image, height_template, width_image, width_template, channel;
    
    height_image = Image.height;
    width_image = Image.width;

    height_template = Template.height;
    width_template = Template.width;
    channel = Image.channel;

    int offset = 10;
    if (LastTopLeft.i == 0) {LastTopLeft.i = offset;}
    if (LastTopLeft.j == 0) {LastTopLeft.j = offset;}    

    // move topleft to pixel at index i, j, start from somewhere offset pixels away diagonally from last topleft.
    for(int i = LastTopLeft.i - offset; i < LastTopLeft.i + offset; i++)
    {
        for (int j = LastTopLeft.j - offset; j < LastTopLeft.j + offset; j++)
        {
            // getting all values in the region into vector, and then calculate cosine similarity with template vector. 
            int index = 0;
            for (int a = i; a < i + height_template; a++)
            {
                for (int b = j; b < j + width_template; b++)
                {
                    for (int k = 0; k < channel; k++)
                    {
                        // assign a, b, k to vector[index]
                        if (index < height_template * width_template * channel)
                            {
                                vector[index]= image[a*width_image*channel + b*channel + k] ; 
                                index++; 
                            }     
                        else printf("%d\n", index);               
                    }
                }
            }
            // calculate cosine sims
            // current_similarity = similarity(vector, template, width_template * height_template * channel);

            float current_similarity_old = similarity(vector, original_template, width_template * height_template * channel);
            float current_similarity_new = similarity(vector, template, width_template * height_template * channel);

            // check if its better similarity
            if (current_similarity_new > current_similarity_old) {current_similarity = current_similarity_new;}
            if (current_similarity_new < current_similarity_old) {current_similarity = current_similarity_old;}
            // current_similarity = similarity(vector, template, width_template * height_template);

            // check if its better similarity
            if (current_similarity > best_similarity)
            {
                best_similarity = current_similarity;
                topleft_i = i;
                topleft_j = j; 
            }
        }
    }
   
   // assiging i j to the TopLeft var.
    TopLeft.j = topleft_j;
    TopLeft.i = topleft_i;

    return TopLeft;
}

// referencing to ShiRikka object tracking code, aim to track the template fast.
Coord tracking_object_fast(unsigned char* image, unsigned char* template, unsigned char* vector, Stats Template, Stats Image)
{
    // setting variables
    Coord TopLeft;
    int topleft_i, topleft_j;
    float best_similarity = 0;
    int count = 0;
    
    // get the height, width, channel of image and template through stats
    int height_image, height_template, width_image, width_template, channel;
    
    height_image = Image.height;
    width_image = Image.width;

    height_template = Template.height;
    width_template = Template.width;
    channel = Image.channel;

    // get pixel at index i, j.
    for(int i = 0; i < height_image - height_template; i+= 5)
    {
        for (int j = 0; j < width_image - width_template; j+= 5)
        {
            // getting all values in the region into vector, and then calculate cosine similarity with template vector. 
            int index = 0;
            for (int a = i; a < i + height_template; a++)
            {
                for (int b = j; b < j + width_template; b++)
                {
                    for (int k = 0; k < channel; k++)
                    {
                        // convert the entry in a, b, k into int and assign it to vector[index]
                        vector[index]= image[a*width_image*channel + b*channel + k] ;
                        index++;                        
                    }
                }
            }
            // calculate cosine sims
            float current_similarity = similarity(vector, template, width_template * height_template * channel);

            // check if its better similarity
            if (current_similarity > best_similarity)
            {
                best_similarity = current_similarity;
                topleft_i = i;
                topleft_j = j; 
            }
        }
    }
   
   // assiging i j to the TopLeft var.
   TopLeft.j = topleft_j;
   TopLeft.i = topleft_i;

   return TopLeft;
}



void copy_image(unsigned char* image, unsigned char* copy, int length_image)
{
    for(int i = 0; i < length_image; i++)
    {
        copy[i] = image[i];
    }
}

// drawing the borderline: #00FF00 (0, 255, 0)
void rect(unsigned char* copy, Coord TopLeft, Stats Template, Stats Image)
{
    // getting channel at index i, j
    // and set the border to be green
  
    // coordinate i of bottom right is i plus height of template, same for width
    Coord BottomRight;
    BottomRight.i = TopLeft.i + Template.height;
    BottomRight.j = TopLeft.j + Template.width;

    int width = Image.width;
    int channel = Image.channel;

    // getting i as the y axis
    for (int i = TopLeft.i; i <= BottomRight.i; i++) 
    {
        // getting j as the x axis
        for (int j = TopLeft.j; j <= BottomRight.j; j++)
        {
            for (int k = 0; k < channel; k++)
            {
                // if at borderline
                // if i at topleft, j at topleft
                if ((i == TopLeft.i) || (j == TopLeft.j) || (i == BottomRight.i) || (j == BottomRight.j))
                {
                    // reach green channel in rgb pixel
                    if(k == 1)
                    {
                        // let green channel be 255
                        copy[i*width*channel + j*channel + k] = 255;
                    }

                    else
                    {
                        // and let other channels be 0 to produce green color
                        copy[i*width*channel + j*channel + k] = 0;
                    }
                }
            }
        }
    }
}



// updating template to be region of object tracked, length should be w*h*c
// STH IS REALLY FKING WRONG WITH THIS FUNCTION. NEED FIXED REAL QUICK.
void update_template(unsigned char* template, unsigned char* image, Coord TopLeft, Stats Template, int width_image)
{
    // getting index for template
    int index = 0;
    
    Coord BottomRight;
    
    BottomRight.i = TopLeft.i + Template.height;
    BottomRight.j = TopLeft.j + Template.width;

    int channel = Template.channel;
    printf("%d\n", channel);
    
    // looping through the region with i j starting at topleft and ending at bottom right
    for (int i = TopLeft.i; i < BottomRight.i; i++) 
    {
        for (int j = TopLeft.j; j < BottomRight.j; j++)
        {
            for (int k = 0; k < channel; k++)
            {
                // copy content of region to template.
                template[index] = image[i*width_image*channel + j*channel + k];
                index++;
            }
        }
    }
}




int *find_pixel(unsigned char *template, unsigned char *image, int t_width, int t_height, int t_channel,
                int i_width, int i_height, int i_channel, int x, int y)
{
    int *result = calloc(2, sizeof(int));
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








