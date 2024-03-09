// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.hpp>

using std::placeholders::_1;

class Red_Obj_Identifyer : public rclcpp::Node
{
public:
  Red_Obj_Identifyer()
  : Node("red_obj_identifyer")
  {
    subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
      "image_raw", 10, std::bind(&Red_Obj_Identifyer::topic_callback, this, _1)); //create a subscriptor to the "image_raw" topic. The topic_callback function
                                                                                    //will run when we're getting images via the topic.
    
    publisher_ = this->create_publisher<sensor_msgs::msg::Image>("red_objects", 10); //create a publisher for the processed image.
  }

private:
  void topic_callback(const sensor_msgs::msg::Image &image) const
  {
    try {
     
      cv::Mat cv_img = cv_bridge::toCvCopy(image,"bgr8")->image; //convert the image from ROS2 msg to opencv image.
      
      //the red color has 2 ranges, therefore we need 2 masks.
      cv::Mat red_obj_mask1;
      cv::Mat red_obj_mask2; 

      cv::Mat hsv_img; //for some reason HSV format had better results
      cv::cvtColor(cv_img, hsv_img, cv::COLOR_BGR2HSV);
     
      //apply the masks to the image
      cv::inRange(hsv_img, cv::Scalar(0, 70, 50) , cv::Scalar(10, 255, 255), red_obj_mask1); 
      cv::inRange(hsv_img, cv::Scalar(170, 70, 50) , cv::Scalar(180, 255, 255), red_obj_mask2);

      //apply OR between the masks.
      cv::Mat red_obj_mask = red_obj_mask1 | red_obj_mask2;

      std::vector < std::vector < cv::Point>> contours;
      cv::findContours(red_obj_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
      
      
      //take the largest contour.
      
      int largest_contour_index = -1;
      double largest_contour_area = 0.0;
      for(size_t i=0 ; i<contours.size(); i++){
        double area = cv::contourArea(contours[i]);
          if(area > largest_contour_area){
            largest_contour_area = area;
            largest_contour_index = i;
          }
      }

      // I chose to draw the contours as circles, so that I could easily find the center.
    
      if (largest_contour_index !=-1) {
          cv::Point2f center;
          float radius;
          cv::minEnclosingCircle(contours[largest_contour_index], center, radius); //find the circle that best represents the contour.

          //draw the circle
          cv::circle(cv_img, cv::Point(center), static_cast<int>(radius), cv::Scalar(0, 255, 0), 2);
          
          // now, draw an X in the center of the circle
          cv::line(cv_img, cv::Point(center.x - (radius * 0.5), center.y - (radius * 0.5)),
                  cv::Point(center.x + (radius * 0.5), center.y + (radius * 0.5)), cv::Scalar(0, 255, 0), 2);
          cv::line(cv_img, cv::Point(center.x - (radius * 0.5), center.y + (radius * 0.5)),
                  cv::Point(center.x + (radius * 0.5), center.y - (radius * 0.5)), cv::Scalar(0, 255, 0), 2);
      }
    
      
      auto processed_img = cv_bridge::CvImage(std_msgs::msg::Header(), "bgr8", cv_img).toImageMsg(); // convert the image with the contours, to a ROS msg.
      publisher_ ->publish(*processed_img); //publish the converted image.
    }
    catch (...){
      RCLCPP_ERROR(this->get_logger(), "ERROR");
    }
  }




  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Red_Obj_Identifyer>());
  rclcpp::shutdown();
  return 0;
}
