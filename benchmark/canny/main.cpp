#include <wx/wx.h>
#include <wx/image.h>
#include <sstream>
#include <fstream>
#include <string>
// #include <iostream>


#include "canny_edge_detector.h"
#include "utils.h"


using namespace std;
RRecord gRR("time.json", "canny");

struct jpg_file{};
struct bmp_file{};

template <typename tag> struct InFileSpecific{};

template<> struct InFileSpecific<jpg_file>{
	typedef wxJPEGHandler wxHandler;
	const static wxBitmapType wxType = wxBITMAP_TYPE_JPEG;
	static std::string gen_out_fp(const std::string & name)
	{
	return name + std::string(".jpg");
	}
};
template<> struct InFileSpecific<bmp_file>{
	typedef wxBMPHandler wxHandler;
	const static wxBitmapType wxType = wxBITMAP_TYPE_BMP;
	static std::string gen_out_fp(const std::string & name)
	{
	return name + std::string(".bmp");
	}
};

int main(int argc, char *argv[])
{
  wxImage image;

  std::string fileName = "../../canny/ff/pic/bmp/lena512.bmp";
  typedef InFileSpecific<bmp_file> IFS_t;

  //std::string fileName ="../../canny/ff/pic/jpg/child.jpg";
  //typedef InFileSpecific<jpg_file> IFS_t;

  wxImageHandler * loader = new IFS_t::wxHandler();
  wxImage::AddHandler(loader);
  std::string outfp = IFS_t::gen_out_fp("out");
  wxString inFileName(fileName.c_str());
  wxString outFileName = outfp.c_str();

  ParamParser pp;
  pp.add_option("input-file", "input bmp file");
  pp.add_option("output-file", "output bmp file");
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  gRR.put("thrd-num", thrd_num);

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


  if (!image.LoadFile(inFileName,IFS_t::wxType)) {
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
  image.SaveFile(outFileName, IFS_t::wxType);

  return 0;
}
