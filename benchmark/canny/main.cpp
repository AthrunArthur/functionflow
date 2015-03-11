#include <wx/wx.h>
#include <wx/image.h>
#include <sstream>
#include <fstream>
// #include <iostream>


#include "canny_edge_detector.h"
#include "rrecord.h"
#include "parameter_parser.h"


using namespace std;

extern void initial_para_env(int thrd_num);

int main(int argc, char *argv[])
{

  wxImage image;
  wxImageHandler * bmpLoader = new wxBMPHandler();
  wxImage::AddHandler(bmpLoader);
  //    wxImageHandler * jpegLoader = new wxJPEGHandler();
  //    wxImage::AddHandler(jpegLoader);
  wxString inFileName(_T("../canny/ff/pic/bmp/lena512.bmp"));
  wxString outFileName = _T("out.bmp");
  //    wxString inFileName(_T("../canny/ff/pic/jpg/child.jpg"));
  //    wxString outFileName = _T("out.jpg");

  RRecord rr("time.json", "canny");
  ParamParser pp;
  pp.add_option<std::string>("input-file", "input bmp file");
  pp.add_option<std::string>("output-file", "output bmp file");
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);

  bool bIsPara = (thrd_num != 0);
  if(bIsPara)
  {
    initial_para_env(thrd_num);
  }
  if(pp.is_set("input-file"))
  {
    inFileName = wxString(pp.get<std::string>("input-file").c_str(), wxConvUTF8);
  }
  ofstream out_time_file;


  //     cout << "Input File Name " << inFileName.mb_str() << endl;
  if (!image.LoadFile(inFileName,wxBITMAP_TYPE_BMP)) {
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

  rr.put("elapsed-time", canny->GetHysteresisTime());
  cout << "Elapsed time: " << canny->GetHysteresisTime() << "us" << endl;
  image.SaveFile(outFileName, wxBITMAP_TYPE_BMP);
  //    image.SaveFile(outFileName, wxBITMAP_TYPE_JPEG);

  return 0;
}
