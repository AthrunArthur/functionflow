#include <wx/wx.h>
#include <wx/image.h>
#include <sstream>
// #include <iostream>

#include "canny_edge_detector.h"


using namespace std;

int main(int argc, char *argv[])
{
    wxImage image;
    wxImageHandler * bmpLoader = new wxBMPHandler();
    wxImage::AddHandler(bmpLoader);
//     wxImageHandler * jpegLoader = new wxJPEGHandler();
//     wxImage::AddHandler(jpegLoader);
    wxString inFileName(_T("../ff/pic/bmp/lena512.bmp"));
    wxString outFileName = _T("out.bmp");
//     wxString inFileName(_T("pic/jpg/adele1.jpg"));
//     wxString outFileName = _T("out.jpg");

    string inFileStr;
    bool bIsPara = false;
    // Task scheduler: Automatic number of threads.

    if(argc > 1) {
        stringstream ss_argv;
        ss_argv << argv[1];
        ss_argv >> inFileStr;
        inFileName = wxString(inFileStr.c_str(), wxConvUTF8);
    }
    if(argc > 2) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[2];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;
        inFileName = wxString(inFileStr.c_str(), wxConvUTF8);

    }

    cout << "Input File Name " << inFileName.mb_str() << endl;
    if (!image.LoadFile(inFileName,wxBITMAP_TYPE_BMP)) {
//     if (!image.LoadFile(inFileName,wxBITMAP_TYPE_JPEG)) {
        cout << "Cannot open "<< inFileName.mb_str() << "!" << endl;
        return -1;
    }
    if (!image.IsOk()) {
        cout << "Image is not OK!"<< endl;
        return -1;
    }

    CannyEdgeDetector *canny = new CannyEdgeDetector(bIsPara);
    canny->ProcessImage(image.GetData(),image.GetWidth(),image.GetHeight(),1.0f, 15, 21);
    // The processed data will be stored in both the image.GetData() and the return data pointer.

    cout << "Elapsed time: " << canny->GetHysteresisTime() << "us" << endl;
    image.SaveFile(outFileName, wxBITMAP_TYPE_BMP);
//     image.SaveFile(outFileName, wxBITMAP_TYPE_JPEG);
    return 0;
}
