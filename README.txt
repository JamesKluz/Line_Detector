Author: James Kluz
Purpose: To find edges in images
         This implementation does find endpoints

         TODO:
         Use Gradient information to construct better line segments

_____________________________________

h1:    Applies a Gaussian Filter and then detects edges using Sobel algorithm
       outputs a .pgm with grey scale set to gradient intensity
input: {input gray-level image} {output gray-level edge image}

h2:    Converts a .pgm file into a binary .pgm file
input: {input-file} {gray level threshold (1-255)} {output-file}

h3:    Takes a binary image, performs hough transform and outputs a .pgm 
       image of the Hough Transform space of a given binary edge image
       as well as a text file of the voting array and endpoints of
       line segments on a line
input: {input binary edge image} {output gray-level Hough image} {output Hough-voting-array}

h4:    takes an image, the corresponding hough array and a threshold (1-255), 
       outputs a .pgm image of the image with lines drawn   
input: {input original gray-level image} {input Hough-voting-array} {input Hough threshold (1-255)} 
       {output gray-level line image}           