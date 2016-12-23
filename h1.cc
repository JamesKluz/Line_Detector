// Author: James Kluz
// Computer Vision Assignment 3
// Professor Ioannis Stamos
// Start date: 9/29/2016
// Apply a Gaussian Filter and then
// detect edges using Sobel algorithm
// outputs a .pgm with colors set to gradient intensity

#include <iostream>
#include <vector>
#include <math.h>
#include "image.h"
using namespace ComputerVisionProjects;

void SmoothImage(Image * an_image);
void SobelEdgeDetector(Image * an_image);

int main(int argc, char **argv){
  if(argc != 3){
    std::cout << "Usage: " << argv[0] << " {input gray-level image} {output gray-level edge image}\n";
    exit(1);
  }	
  const std::string input_image(argv[1]);
  const std::string output_image(argv[2]);

  Image an_image;
  if(!ReadImage(input_image, &an_image)){
  	std::cout << "Failed to input image\n";
  	exit(1);
  }
  SmoothImage(&an_image);
  SobelEdgeDetector(&an_image);
  if(!WriteImage(output_image, an_image)){
    std::cout << "Failed to output image\n";
  }
  return 0;
}

void SobelEdgeDetector(Image * an_image){
  int m_2 = 1;
  size_t rows = an_image->num_rows();
  size_t cols = an_image->num_columns();
  //vectors of partial derivatives with respect to i and j
  std::vector<std::vector<double>> partial_i(rows, std::vector<double>(cols, 0.0));
  std::vector<std::vector<double>> partial_j(rows, std::vector<double>(cols, 0.0));
  //Sobel Convolutions for partial derivatives
  std::vector<std::vector<double>> mask_i{{-1.0, -2.0, -1.0}, {0.0, 0.0, 0.0}, {1.0, 2.0, 1.0}};
  std::vector<std::vector<double>> mask_j{{-1.0, 0.0, 1.0}, {-2.0, 0.0, 2.0}, {-1.0, 0.0, 1.0}};
  //get partial with repect to i
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j){
      double temp = 0;
      for(int k = -m_2; k <= m_2; ++k)
        for(int l = -m_2; l <= m_2; ++l)
          if(i + k > -1 && i + k < rows && j + l > -1 && j + l < cols)
            temp = temp + (mask_i[k + m_2][l + m_2] * an_image->GetPixel(i + k, j + l));
      partial_i[i][j] = temp;
    }
  //get partial with repect to j
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j){
      double temp = 0;
      for(int k = -m_2; k <= m_2; ++k)
        for(int l = -m_2; l <= m_2; ++l)
          if(i + k > -1 && i + k < rows && j + l > -1 && j + l < cols)
            temp = temp + (mask_j[k + m_2][l + m_2] * an_image->GetPixel(i + k, j + l));
      partial_j[i][j] = temp;
    }  
  //Make binary image representing edges
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j){
      double g = sqrt(partial_i[i][j] * partial_i[i][j] + partial_j[i][j] * partial_j[i][j]);
      an_image->SetPixel(i, j, g);
    }    
}

void SmoothImage(Image * an_image){
  //when time add extra function parameters and automate the gauss filter
  int m = 5;
  int m_2 = 5/2;
  std::vector<double> one_d_Gauss{.0625, .25, .375, .25, .0625};
  size_t rows = an_image->num_rows();
  size_t cols = an_image->num_columns();
  //to store results of initial convolution
  std::vector<std::vector<double>> image_values(rows, std::vector<double>(cols, 0.0));
  //horizontal convolution:
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j){
      double temp = 0;
      for(int k = -m_2; k <= m_2; ++k)
        if(j + k > -1 && j + k < cols)
          temp = temp + (one_d_Gauss[k + m_2] * an_image->GetPixel(i, j + k));
      image_values[i][j] = temp;
    }
  //vertical convolution:  
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j){
      double temp = 0;
      for(int k = -m_2; k <= m_2; ++k)
        if(i + k > -1 && i + k < rows)
          temp = temp + (one_d_Gauss[k + m_2] * image_values[i + k][j]);
      an_image->SetPixel(i, j, floor(temp+.5));
    }  
}