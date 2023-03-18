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

unsigned char *uc_arrayNew_1d(int _size)
{
    return (unsigned char *)calloc(_size, sizeof(unsigned char));
}

/**
 * Object tracking steps:
 * load template, for loop the images path and then load them all out.
 * hmm
 * after tracking the region in img i, updating template with that region and using that to track
 * object in img i+1
 */

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

// grayscaling image, source: https://tannerhelland.com/2011/10/01/grayscale-image-algorithm-vb6.html
// Gray = (Red * 0.3 + Green * 0.59 + Blue * 0.11)
unsigned char *grayscale(unsigned char *image, Stats Image)
{
    // set up variables
    int height, width, channel;
    height = Image.height;
    width = Image.width;
    channel = Image.channel;
    // unsigned char* grayed_image = (unsigned char*)calloc(height * width, sizeof(unsigned char));
    
    unsigned char* grayed_image = uc_arrayNew_1d(height * width);
    float gray;
    float red, green, blue;
    int index;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            red = image[i * width * channel + j * channel + 0];
            green = image[i * width * channel + j * channel + 1];
            blue = image[i * width * channel + j * channel + 2];

            // Gray = (Red * 0.3 + Green * 0.59 + Blue * 0.11)
            gray = red * 0.3 + green * 0.59 + blue * 0.11;
            // gray = (red + green + blue) / 3;
            grayed_image[i * width + j] = (int) gray;
        }
    }
    return grayed_image;
}

// specify the coordinate of the box that contain the region most similar
// to the template
// input: image, template, vector, with vector size should be the same as template.
// this function now is to get approximate frame within 30 pixels near last obtained topleft.
Coord re_tracking_object(unsigned char* grayed_image, unsigned char* grayed_template, unsigned char* original_template, unsigned char* vector, Stats Template, Stats Image, Coord LastTopLeft)
{
    // setting variables
    Coord TopLeft;
    int topleft_i, topleft_j;
    float best_similarity = 0;
    float current_similarity = 0;
    // int count = 0;
    
    // get the height, width, channel of image and template through stats
    int height_image, height_template, width_image, width_template, channel;
    
    height_image = Image.height;
    width_image = Image.width;

    height_template = Template.height;
    width_template = Template.width;
    channel = Image.channel;

    int offset = 10;
    if (LastTopLeft.i < offset) {LastTopLeft.i = offset;}
    if (LastTopLeft.j < offset) {LastTopLeft.j = offset;}    

    // move topleft to pixel at index i, j, start from somewhere offset pixels away diagonally from last topleft.
    for(int i = LastTopLeft.i - offset; i < LastTopLeft.i + offset; i+=1)
    {
        for (int j = LastTopLeft.j - offset; j < LastTopLeft.j + offset; j+=1)
        {
            // getting all values in the region into vector, and then calculate cosine similarity with template vector. 
            int index = 0;
            for (int a = i; a < i + height_template; a++)
            {
                for (int b = j; b < j + width_template; b++)
                {
                    // assign a, b, k to vector[index]
                    vector[index]= grayed_image[a*width_image + b]; 
                    index++; 
                }
            }
            // calculate cosine sims
            float current_similarity_old = similarity(vector, original_template, width_template * height_template * channel);
            float current_similarity_new = similarity(vector, grayed_template, width_template * height_template * channel);

            // check if its better similarity
            if (current_similarity_new > current_similarity_old) {current_similarity = current_similarity_new;}
            if (current_similarity_new < current_similarity_old) {current_similarity = current_similarity_old;}
            current_similarity = similarity(vector, grayed_template, width_template * height_template);

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
    printf("\nsimilarity: %f\n", best_similarity);
    return TopLeft;
}

// referencing to ShiRikka object tracking code, aim to track the template fast.
Coord tracking_object_fast(unsigned char* grayed_image, unsigned char* grayed_template, unsigned char* vector, Stats Template, Stats Image)
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
    int amount = 5;
    for(int i = 0; i < height_image - height_template; i+= amount)
    {
        for (int j = 0; j < width_image - width_template; j+= amount)
        {
            // getting all values in the region into vector, and then calculate cosine similarity with template vector. 
            // sliding image they say.
            int index = 0;
            for (int a = i; a < i + height_template; a++)
            {
                for (int b = j; b < j + width_template; b++)
                {
                    // convert the entry in a, b into int and assign it to vector[index]
                    vector[index]= grayed_image[a*width_image + b] ;
                    index++;                        
                    
                }
            }
            // calculate cosine sims
            float current_similarity = similarity(vector, grayed_template, width_template * height_template);

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

   printf("\nsimilarity: %f\n", best_similarity);
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
void update_template(unsigned char* template, unsigned char* image, Coord TopLeft, Stats Template, int width_image)
{
    // getting index for template
    int index = 0;
    
    Coord BottomRight;
    
    BottomRight.i = TopLeft.i + Template.height;
    BottomRight.j = TopLeft.j + Template.width;

    // int channel = Template.channel;
        
    // looping through the region with i j starting at topleft and ending at bottom right
    for (int i = TopLeft.i; i < BottomRight.i; i++) 
    {
        for (int j = TopLeft.j; j < BottomRight.j; j++)
        {
            // copy content of region to template.
            template[index] = image[i*width_image + j];
            index++;
            
        }
    }
}













