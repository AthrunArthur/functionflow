#include <wx/wx.h>
#include <wx/image.h>
#include <sstream>
#include <fstream>
// #include <iostream>
#include "common/log.h"
#include "canny_edge_detector.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

int main(int argc, char *argv[])
{
    ff::rt::set_hardware_concurrency(8);//Set concurrency
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");
    
    wxImage image;
    wxImageHandler * bmpLoader = new wxBMPHandler();
    wxImage::AddHandler(bmpLoader);
//    wxImageHandler * jpegLoader = new wxJPEGHandler();
//    wxImage::AddHandler(jpegLoader);
    //wxString inFileName(_T("/home/athrun/designBig.bmp"));
    wxString inFileName(_T("../canny/ff/pic/bmp/lena512.bmp"));
    wxString outFileName = _T("out.bmp");
//    wxString inFileName(_T("../canny/ff/pic/jpg/child.jpg"));
//    wxString outFileName = _T("out.jpg");
    string inFileStr;
    
    /*Record the running time.*/
    ofstream out_time_file;
    
    bool bIsPara = false;//false;
    
    if(argc > 1) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[1];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;     
    }
    if(argc > 2) {
        stringstream ss_argv;
        ss_argv << argv[2];
        ss_argv >> inFileStr;
	inFileName = wxString(inFileStr.c_str(), wxConvUTF8);
    }    

//     cout << "Input File Name " << inFileName.mb_str() << endl;
    if (!image.LoadFile(inFileName,wxBITMAP_TYPE_BMP)) {
//    if (!image.LoadFile(inFileName,wxBITMAP_TYPE_JPEG)) {
        cout << "Cannot open "<< inFileName.mb_str() << "!" << endl;
        return -1;
    }
    if (!image.IsOk()) {
        cout << "Image is not OK!"<< endl;
        return -1;
    }

    //Pre initialization thread_pool when using parallelizing.
    if(bIsPara) {
	_DEBUG(ff::fflog<>::init(ff::INFO, "log.txt"))
	_DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
        ff::para<int> a;
        int num = 10;
        a([&num]() {
            return num;
        }).then([](int x) {});
        ff::para<> b;
        b[a]([&num, &a]() {
            num + a.get();
        }).then([]() {});
    }  
	//cout << "start" << endl;
    CannyEdgeDetector *canny = new CannyEdgeDetector(bIsPara);
	//cout << "start Processing!" << endl;
    canny->ProcessImage(image.GetData(),image.GetWidth(),image.GetHeight(),1.0f, 15, 21);
    // The processed data will be stored in both the image.GetData() and the return data pointer.

    if(bIsPara)
        pt.put("para-elapsed-time", canny->GetHysteresisTime());
    else
        pt.put("sequential-elapsed-time", canny->GetHysteresisTime());
    boost::property_tree::write_json("time.json", pt);
    cout << "Elapsed time: " << canny->GetHysteresisTime() << "us" << endl;
    image.SaveFile(outFileName, wxBITMAP_TYPE_BMP);
//    image.SaveFile(outFileName, wxBITMAP_TYPE_JPEG);
    
    if(bIsPara) {
        out_time_file.open("para_time.txt",ios::app);
        if(!out_time_file.is_open()) {
            cout << "Can't open the file para_time.txt" << endl;
            return -1;
        }
        out_time_file << canny->GetHysteresisTime() << endl;
        out_time_file.close();
    }
    else{
      out_time_file.open("time.txt");
        if(!out_time_file.is_open()) {
            cout << "Can't open the file time.txt" << endl;
            return -1;
        }
        out_time_file << canny->GetHysteresisTime() << endl;
        out_time_file.close();
    }
    
    return 0;
}
