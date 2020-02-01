
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "kcftracker.hpp"
#include "opencv2/video.hpp"
#include "opencv2/core/utility.hpp"
#include "cstring"
#include "opencv2/core.hpp"
#include <ctime>
#include <string>
#include <iostream>
#include "../include/tools.h"
#include "gnuplot-iostream.h"
#include "algorithm"

using namespace cv;
using namespace std;



bool HOG = true;
bool FIXEDWINDOW = false;
bool MULTISCALE = true;
bool LAB = false;

std::vector<cv::Rect2f> getGroundTruth(const std::string & txt_path)
{
    std::vector<cv::Rect2f> ground_truths{};
    ifstream groundTruth(txt_path, ios_base::app);
    if(groundTruth.is_open())
    {
        cv::Rect2f trackWindow;
        bool withComma{};

        string fetch_line;
        getline(groundTruth, fetch_line);
        while(!fetch_line.empty())
        {
            istringstream objectIss(fetch_line);
            objectIss >> trackWindow.x >> trackWindow.y >> trackWindow.width >> trackWindow.height;
            withComma = trackWindow.x != 0 && trackWindow.y == 0;
            if (withComma) {
                istringstream temp_stream(fetch_line);
                char temp;
                temp_stream >> trackWindow.x >> temp >> trackWindow.y >> temp >> trackWindow.width >> temp
                            >> trackWindow.height;
            }
            ground_truths.emplace_back(trackWindow);
            if(groundTruth.eof())
                break;
            getline(groundTruth, fetch_line);
        }
        groundTruth.close();
    } else
        std::cout<<"can't open the "<<txt_path<<std::endl;
    return ground_truths;
}
float bbOverlap(const cv::Rect2f & box1,const cv::Rect2f & box2)
{
    if (box1.x > box2.x+box2.width) { return 0.0; }
    if (box1.y > box2.y+box2.height) { return 0.0; }
    if (box1.x+box1.width < box2.x) { return 0.0; }
    if (box1.y+box1.height < box2.y) { return 0.0; }
    float colInt =  min(box1.x+box1.width,box2.x+box2.width) - max(box1.x, box2.x);
    float rowInt =  min(box1.y+box1.height,box2.y+box2.height) - max(box1.y,box2.y);
    float intersection = colInt * rowInt;
    float area1 = box1.width*box1.height;
    float area2 = box2.width*box2.height;
    return intersection / (area1 + area2 - intersection);
}
/*Main Function*/
int main(int argc, char**argv)
{

    Mat frame,oldframe;
    /*tracker using*/
    Rect2f trackWindow;

    KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

    const string base_path{"E:\\Backup\\Benchmark\\"};
    std::vector<std::string> seq_names{Tools::getPureFileNames("E:\\Backup\\Benchmark","zip")};
    const string file_path = base_path+"Coke";
    std::vector<cv::Rect2f> ground_truth{getGroundTruth(file_path+"\\groundtruth_rect.txt")};

    std::vector<std::string> frame_names{Tools::getFileNames(file_path+"\\img","jpg")};
    if(ground_truth.size()<frame_names.size())
    {
        frame_names.erase(frame_names.begin()+ground_truth.size()+1,frame_names.end());
        std::cout<<"delete "<<frame_names.size()<<"-"<<ground_truth.size()<<" elements"<<std::endl;
    }
    bool first_time{true};
    trackWindow=ground_truth[0];
    std::vector<cv::Point2d> results{};
    cv::Point2d result;
    /*Tracking begin */
    clock_t  start=clock();
    int i=0;
    for(const auto & frame_name : frame_names)
    {
        /*upadate the tracker*/
        frame=imread(frame_name,CV_LOAD_IMAGE_COLOR);
        if (frame.empty())
            break;
        if(first_time)
        {
            tracker.init(trackWindow,frame);
            first_time=false;
        }
        else
            trackWindow = tracker.update(frame);
        result.x = sqrt((trackWindow.x - ground_truth[i].x)*(trackWindow.x - ground_truth[i].x) +
                        (trackWindow.y - ground_truth[i].y)*(trackWindow.y - ground_truth[i].y));
        result.y = bbOverlap(trackWindow,ground_truth[i]);
        results.emplace_back(result);
        i++;
        rectangle(frame, trackWindow, Scalar(0, 255, 0));
        imshow("Track",frame);
        waitKey (1);
    }
    clock_t end=clock();
    std::cout<<"sequence name is "<<"Car24"<<" FPS is "<<ground_truth.size()/((end-start)/CLOCKS_PER_SEC)<<std::endl;
    double precision_number=std::count_if(results.begin(),results.end(),[](const cv::Point2d & result_){return result_.x>=0&&result_.x<=20;});
    double overlap_number = std::count_if(results.begin(),results.end(),[](const cv::Point2d & result_){return result_.y>=0.5;});
    std::cout<<"precision 20 px is "<<precision_number/results.size()<<" overlap 0.5 is "<<overlap_number/results.size()<<std::endl;
    Gnuplot gp("F:\\gnuplot\\bin\\gnuplot.exe");
    gp << "set xrange [1:"<<ground_truth.size()<<"]\nset yrange ["<<*std::min_element(tracker.energy.begin(),tracker.energy.end())<<":1]\n";
    gp << "plot '-' with points title 'peak_value', '-' with points title 'mid', '-' with points title 'energy'\n";
    gp.send1d(tracker.peak_values);
    gp.send1d(tracker.mid);
    gp.send1d(tracker.energy);
    system("pause");
}
