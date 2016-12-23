// Author: James Kluz
// Computer Vision Assignment 3
// Professor Ioannis Stamos
// Start date: 9/29/2016
// input an image, the corresponding hough array and a threshold (1-255), 
// outputs a .pgm image of the image with line segments drawn

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <sstream>
#include "image.h"
using namespace ComputerVisionProjects;

void DrawLinesFromHough(const std::string input_file, Image *an_image, int hough_threshold);

int main(int argc, char **argv){
  if(argc != 5){
    std::cout << "Usage: " << argv[0] << " {input original gray-level image} {input Hough-voting-array} {input Hough threshold (1-255)} {output gray-level line image}\n";
    exit(1);
  } 
  const std::string input_original_image(argv[1]);
  const std::string voting_array(argv[2]);
  const int hough_threshold = std::stoi(argv[3]);
  const std::string output_line_image(argv[4]);

  Image an_image;
  if(!ReadImage(input_original_image, &an_image)){
    std::cout << "Failed to input image\n";
    exit(1);
  }

  DrawLinesFromHough(voting_array, &an_image, hough_threshold);

  if(!WriteImage(output_line_image, an_image)){
    std::cout << "Failed to output image\n";
  }
  return 0;
}

void DrawLinesFromHough(const std::string input_file, Image *an_image, int hough_threshold){
  struct EndPoints{
    int x0_;
    int y0_;
    int x1_;
    int y1_;
  };

  struct Line{
    int votes_;
    std::vector<EndPoints> segments_;
  };

  std::ifstream in_stream(input_file);
  if(!in_stream.is_open()){
    std::cout << "Couldn't open hough array input\n";
    exit(1);
  }
  std::string line;
  std::getline(in_stream, line);
  std::stringstream parameters(line);
  int rows, cols, max_votes;
  double theta_step, rho_step;
  parameters >> rows >> cols >> theta_step >> rho_step >> max_votes;
  std::vector<std::vector<Line>> input_hough(rows, std::vector<Line>(cols));
  int threshold = ((double)hough_threshold/255.0)*max_votes +.5;
  int i = 0;
  int j = 0;
  while(std::getline(in_stream, line) && i < rows && j < cols){
    std::stringstream ss(line);
    int votes;
    ss >> votes;
    if(votes > threshold){
      input_hough[i][j].votes_ = votes;
      int num_segments;
      ss >> num_segments;
      for(int k = 0; k < num_segments; ++k){
        EndPoints temp;
        ss >> temp.x0_ >> temp.y0_ >> temp.x1_ >> temp.y1_;
        input_hough[i][j].segments_.push_back(temp); 
      }
    } else {
      input_hough[i][j].votes_ = 0;
    }
    ++j;
    if(j == cols){
      j = 0;
      ++i;
    }
  }
  in_stream.close();
  //now we draw lines
  for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j)
      if(input_hough[i][j].votes_ != 0){
        //check to see if this is a local max
        int local_max = input_hough[i][j].votes_;
        bool at_max = true;
        for(int k = -11; k <= 11 && at_max; ++k)
          for(int l = -11; l <= 11 && at_max; ++l)
            if(i+k >=0 && i+k < rows && j+l >=0 && j+l < cols)
              if(input_hough[i+k][j+l].votes_ > local_max)
                at_max = false;
        if(at_max)
          for(int k = 0; k < input_hough[i][j].segments_.size(); ++k){
            int x0 = input_hough[i][j].segments_[k].x0_;
            int y0 = input_hough[i][j].segments_[k].y0_;
            int x1 = input_hough[i][j].segments_[k].x1_;
            int y1 = input_hough[i][j].segments_[k].y1_;
            DrawLine(x0, y0, x1, y1, 255, an_image);
          } 
      }
}