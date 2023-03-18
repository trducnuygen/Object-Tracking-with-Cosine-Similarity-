# Object-Tracking-with-Cosine-Similarity

## 1. Introduction

This is a university project, under the supervision of Professor Dinh Quang Vinh.

### 1.1   Goal

Inputting images and a template, the goal is to track the template throughout 64 images by using cosine similarity algorithm. The template used in an image is updated to be the templated tracked in the previous image.

### 1.2  Dataset

The dataset is stored in `images` folder, which consists of 64 images, generated from a person sitting from a chair and rotating himself 360 degrees. In the background stands another person with the aim of confusing the algorithm. 

The template is the file `template.jpg`, which shows the head of the person in the center of dataset images.


## 2. Dependencies 

Reading and writing images functions are stored in the two stb headers files: 
- stb_image_write.h: https://github.com/nothings/stb/blob/master/stb_image_write.h
- stb_image.h: https://github.com/nothings/stb/blob/master/stb_image.h


## 3. Conclusion 
Report of the project: https://drive.google.com/file/d/1mMmREFAwa2dAdAcMfgXZgPtb7G-X0mhX/view

Remarks:
- the template is tracked properly in the first ten images. After that, the rotation of the template person confuses the algorithm, making it unable to track the template well. 
- the cause is the algorithm itself and the template updating process: 
  - Cosine similarity algorithm by itself depends on only the template, which means previously tracked templates are not used.
  - The template updating process hurts the algorithm when there is a significant change of the object between two images: transition from back of the head to the side of the head, causing notable changes in color values.
- grayscaling and normalising images and template as a processing step significantly speed up the tracking process. However, they do not help the performance.
