// Author: James Kluz
// Computer Vision Assignment 3
// Professor Ioannis Stamos
// Start date: 9/29/2016
// input a binary image, performs hough transform and
// outputs a .pgm image of the Hough Transform space of a given binary edge image
// as well as a text file of the voting array and endpoints for segments

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "image.h"
using namespace ComputerVisionProjects;

void MakeHoughImage(std::vector<std::vector<int>> *input_array, Image *output_image);
//expecting that theta_step evenly divides 180 and rho_step evenly divides ceil(sqrt(M^2 + N^2)) where M and N are
//the dimensions of the input image in pixels and passed to the function as rho_max
//also expecting that output_array is of the proper size
void MakeHoughVotingVector(Image *input_image, std::vector<std::vector<int>> *output_array, 
                           double theta_step, int total_theta_steps, double rho_step, 
                           int rho_total_steps, const std::string out_txt);

int main(int argc, char **argv){
  if(argc != 4){
    std::cout << "Usage: " << argv[0] << " {input binary edge image} {output gray-level Hough image} {output Hough-voting-array}\n";
    exit(1);
  } 
  const std::string input_image(argv[1]);
  const std::string output_image(argv[2]);
  const std::string output_array_txt(argv[3]);

  Image an_image;
  if(!ReadImage(input_image, &an_image)){
    std::cout << "Failed to input image\n";
    exit(1);
  }
  double rho_step = 1.0;
  //max distance from origin
  int rho_buckets = ceil(sqrt(an_image.num_rows()*an_image.num_rows() + an_image.num_columns()*an_image.num_columns()));
  //to handle negative rho
  rho_buckets = (2*rho_buckets)/rho_step + 1;
  double theta_step = 1.0; // in degrees
  int theta_buckets = 180; // 180/theta_step

  std::vector<std::vector<int>> output_array(theta_buckets, std::vector<int>(rho_buckets));
  //for testing:
  //std::cout << output_array[5][5].votes_ << "\n";

  MakeHoughVotingVector(&an_image, &output_array, theta_step, theta_buckets, rho_step, rho_buckets, output_array_txt);
  Image hough_image;
  hough_image.AllocateSpaceAndSetSize(output_array.size(), output_array[0].size());
  hough_image.SetNumberGrayLevels(255);
  //std::cout << hough_image.num_rows() << " " << hough_image.num_columns() << std::endl;
  MakeHoughImage(&output_array, &hough_image);
  //std::cout << output_array[-5][-5] << "\n";
  if(!WriteImage(output_image, hough_image)){
    std::cout << "Failed to output image\n";
  }
  return 0;
}

void MakeHoughImage(std::vector<std::vector<int>> *input_array, Image *output_image){
  int image_rows = output_image->num_rows();
  int image_cols = output_image->num_columns();
  for(int i = 0; i < image_rows; ++i)
    for(int j = 0; j < image_cols; ++j)
      output_image->SetPixel(i, j, (*input_array)[i][j]);
}

void MakeHoughVotingVector(Image *input_image, std::vector<std::vector<int>> *output_array, 
                           double theta_step, int total_theta_steps, double rho_step, 
                           int rho_total_steps, const std::string out_txt){
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

  std::vector<std::vector<Line>> array_with_endpoints(total_theta_steps, std::vector<Line>(rho_total_steps));

  //this epsilon is our threshold for whether or not we've reached a new line segment on a line
  double epsilon = 3;
  int image_rows = input_image->num_rows();
  int image_cols = input_image->num_columns();
  int max_votes = 0;
  for(int i = 0; i < image_rows; ++i)
    for(int j = 0; j < image_cols; ++j)
      if(input_image->GetPixel(i, j) != 0)
        for(int k = 0; k < total_theta_steps; ++k){
          double current_theta = (k * theta_step)*(3.141592/180);
          double current_rho = i*cos(current_theta) + j*sin(current_theta);
          int current_col = rho_total_steps/2 + floor(current_rho/rho_step + 0.5);
          //no endpoints have been added to this line
          if(array_with_endpoints[k][current_col].votes_ == 0){
            array_with_endpoints[k][current_col].votes_  = array_with_endpoints[k][current_col].votes_  + 1;
            EndPoints temp;
            temp.x0_ = i;
            temp.x1_ = i;
            temp.y0_ = j;
            temp.y1_ = j;
            array_with_endpoints[k][current_col].segments_.push_back(temp);
          } else { //going to check all segments and see if this a new segment or an extension of an existing
            array_with_endpoints[k][current_col].votes_  = array_with_endpoints[k][current_col].votes_  + 1;
            std::vector<EndPoints> &these_segments = array_with_endpoints[k][current_col].segments_;
            //is the current point on a new segment
            bool new_segment = true;
            for(int l = 0; l < these_segments.size(); ++l){
              int x0 = these_segments[l].x0_;
              int y0 = these_segments[l].y0_;
              int x1 = these_segments[l].x1_;
              int y1 = these_segments[l].y1_;
              double dist_1 = sqrt((x0-i)*(x0-i) + (y0-j)*(y0-j));
              double dist_2 = sqrt((x1-i)*(x1-i) + (y1-j)*(y1-j));
              if(dist_1 < epsilon || dist_2 < epsilon){
                if (dist_1 < dist_2){
                  these_segments[l].x0_ = i;
                  these_segments[l].y0_ = j;
                } else {
                  these_segments[l].x1_ = i;
                  these_segments[l].y1_ = j;
                }
                new_segment = false;
                break;
              }
            }
            if(new_segment){  
              EndPoints temp;
              temp.x0_ = i;
              temp.x1_ = i;
              temp.y0_ = j;
              temp.y1_ = j;
              these_segments.push_back(temp);
            } 
          }
          if(array_with_endpoints[k][current_col].votes_  > max_votes)
            max_votes = array_with_endpoints[k][current_col].votes_;
        }
  //output hough array          
  std::ofstream out_stream(out_txt);
  out_stream << total_theta_steps << " " << rho_total_steps << " " << theta_step << " " << rho_step << " " << max_votes << "\n"; 
  for(int i = 0; i < total_theta_steps; ++i)
    for(int j = 0; j < rho_total_steps; ++j){
      int to_normalize = array_with_endpoints[i][j].votes_;
      out_stream << to_normalize << " ";
      std::vector<EndPoints> &these_segments = array_with_endpoints[i][j].segments_;
      out_stream << these_segments.size() << " ";
      for(int k = 0; k < these_segments.size(); ++k){
        int x0 = these_segments[k].x0_;
        int y0 = these_segments[k].y0_;
        int x1 = these_segments[k].x1_;
        int y1 = these_segments[k].y1_;
        out_stream << x0 << " " << y0 << " " << x1 << " " << y1 << " ";
        //out_stream << these_segments[k].x0_ < " " << these_segments[k].y0_ << " " << these_segments[k].x1_ << " " << these_segments[k].y1_ << " ";
      }
      out_stream << "\n";
      //normalize votes to be in range 0-255
      to_normalize = ((double)to_normalize/max_votes)*255.0;
      (*output_array)[i][j] = to_normalize;
    }
  out_stream.close();  
}




