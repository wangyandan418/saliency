// 2D dct version.. 
// include CARLsim user interface
#include <math.h>
#include <carlsim.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream" 
using namespace std;
using namespace cv;

#define gain_AMPA  .25   //gain for GABAa and AMPA (fast)
#define gain_NMDA  .25   //gain for GABAb and NMDA (slow)
#include "imagespecs.h"

#include "myconnection.h"
#include "dct.h"

#define DEBUG

#ifdef DEBUG
  void writeVecLine(std::vector<float>,string);
#endif


  int main() {

  Mat image_fullsize,image_prev_fullsize;
  Mat image,image_prev;
  Mat image_gray,image_prev_gray;
  Mat image_f(sz_R*scale_R, sz_C*scale_C, CV_32FC3), chan_mot_f;
  Size patch_dim(sz_R,sz_C);

#ifdef DEBUG
  fstream fd_full,fd_dct,fd_in;
  string fullfilename("patches/full.csv");
  string dctfilename("patches/dct.csv");
  string infilename("patches/inputs.csv");
  string patchposfilename("patches/patchpos.csv");
  string patchnegfilename("patches/patchneg.csv");
  string posPipefilename("patches/posPipe.csv");
  string negPipefilename("patches/negPipe.csv");
  //string fullsizefilename("patches/image.csv");

#endif

  // read the images
  image_prev_fullsize = imread("images/000646.jpg",CV_LOAD_IMAGE_COLOR);
  image_fullsize      = imread("images/000647.jpg",CV_LOAD_IMAGE_COLOR);
  //imshow("foo",image_fullsize);
  //waitKey(0);

#ifdef DEBUG
      //writeVecLine(image_fullsize, fullsizefilename);
#endif

#ifdef DEBUG
  // open and delete contents of exsisting debug trace files
  fd_full.open(fullfilename.c_str(), std::fstream::out | std::fstream::trunc);
  fd_dct.open(dctfilename.c_str(), std::fstream::out | std::fstream::trunc);
  fd_in.open(infilename.c_str(), std::fstream::out | std::fstream::trunc);
  fd_full.close();
  fd_dct.close();
  fd_in.close();
#endif

  if( !image_fullsize.data || !image_prev_fullsize.data  )  {
    cout <<  "Could not open or find the image" << std::endl ;
    return -1;
  }
  Size s = image_fullsize.size();
  //printf("fullsize (%d %d), (%d %d)\n", image_fullsize.rows,image_fullsize.cols,s.height,s.width);
  cout << "fullsize ("
	   << image_fullsize.rows << " " << image_fullsize.cols <<"),"
	   << "(" << s.height << " " << s.width << ")"
	   << std::endl;

  // resize the images to working size
  Size target_size(sz_C*scale_C,sz_R*scale_R); // target size template is backward (cols,rows) argh!!!!!!
  resize(image_fullsize,image,target_size);
  resize(image_prev_fullsize,image_prev,target_size);
  //printf("target size (%d,%d) image (%d,%d)\n", target_size.width,target_size.height,image.rows,image.cols);
  cout << "target size ("
	   << target_size.width << " " << target_size.height <<")," << "image ("
	   << image.rows << " " << image.cols << ")"
	   << std::endl;

  // make the motion channel and build the 4 channel image
  cvtColor(image, image_gray, CV_RGB2GRAY);
  cvtColor(image_prev, image_prev_gray, CV_RGB2GRAY);
  Mat chan_mot = image_gray - image_prev_gray;
//  imshow("foo",image);
  //waitKey(0);
//  imshow("foo1",image_gray);
  //waitKey(0);
//  imshow("foo2",image_prev);
  //waitKey(0);
//  imshow("foo3",image_prev_gray);
  //waitKey(0);

  //  namedWindow("foo",CV_WINDOW_AUTOSIZE);
  //  imshow("foo",image);
  //  waitKey(0);

  // convert the images to floats for dct
  //image.convertTo(image,CV_32FC3);
  //chan_mot.convertTo(chan_mot,CV_32FC1);

//  image.convertTo(image_f,CV_32FC3);
//  chan_mot.convertTo(chan_mot_f,CV_32FC1);
  image.convertTo(image_f,CV_32FC3,1/255.0);
  chan_mot.convertTo(chan_mot_f,CV_32FC1,1/255.0);
//  imshow("im",image);
//  imshow("imf",image_f);
  //waitKey(0);


  // make the 4-channel split image ( array of 4 x 1chan) for processing
  vector<Mat> image_split(4),chan_mot_split(1);
  split(image_f, image_split);
  split(chan_mot_f, chan_mot_split); // not sure if this is necessary
  image_split[3] = chan_mot_split[0];
//  imshow("sp0",image_split[0]);
//  imshow("sp1",image_split[1]);
//  imshow("sp2",image_split[2]);
//  imshow("sp3",image_split[3]);
//  waitKey(0);

  // allocate the output images
  Mat img_sal(target_size.height,target_size.width,CV_32FC1,Scalar(0));
  Mat img_sal_int(target_size.height,target_size.width,CV_8UC1,Scalar(0));
  //imshow("foo1",img_sal_int);
  //waitKey(0);


  // make a pyramid patch for remixing overlaps
  //Mat  pyramid = Mat::zeros(sz_R,sz_C, CV_32FC1);
 Mat  pyramid(sz_R,sz_C, CV_32FC1,.5/(float) overlap);
 //for (int i = 0; i<sz_R/2; i++) {
 //   Rect pyr(i,i,sz_R-(2*i),sz_C-(2*i));
 //   pyramid(pyr) += Mat::ones(sz_R-(i*2),sz_C-(i*2),CV_32FC1); 
 // }
 // normalize(pyramid,pyramid,0.0,.5,NORM_MINMAX);
 //imshow("foo1",pyramid);
 //waitKey(0);

#ifdef DEBUG
 //  writeMatToFile(pyramid,"patches/pyramid.yml");
#endif

  // create the  network 
  CARLsim sim("dct", GPU_MODE, USER, 0, -1234);
//  CARLsim sim("dct", CPU_MODE, USER, 0, -1234);
  spiking_dct dctNet(&sim);
  
  // run the first stage of the pipeline

  // break down the image into the patches for each epoch
  for (int i = 0; i<= (target_size.height - sz_R); i+=stride_R)
	  for (int j =0; j<= (target_size.width - sz_C); j+=stride_C)
	  {
		  Rect patch_rect(j,i,sz_C,sz_R);
		  //Rect patch_rect(i,j,sz_R,sz_C);
    
		  Mat foo(sz_C,sz_R,CV_32FC4);
		  vector<Mat> sal_patch(4);
		  Mat patch_temp,patch_temp_int;
		  vector<Mat> dct_patch(4);
		  split(foo,sal_patch);
		  split(foo,dct_patch);
    
		  for (int chan = 0; chan <4; chan++)
		  {
			  // extract patch from input and setup input neurons
			  Mat patch = image_split[chan](patch_rect);
//			  cout<<image_split[chan].cols<<"x"<<image_split[chan].rows<<endl;
//			  cout<<patch_rect<<endl;
//			  imshow("foo8",patch);
//			  waitKey(0);
//			  cout<<patch.type()<<" "<<CV_32FC1<<" "<<patch.isContinuous()<<endl;
//			  float * patch_pix = (float *) patch.data;

			  std::vector<float> inrates_E(sz_R*sz_C);
			  for (int ii = 0; ii<sz_R; ii++)
			  {
				  for (int jj = 0; jj<sz_C; jj++)
				  {
					  float * p = patch.ptr<float>(ii);
					  //inrates_E.at(ii*sz_C+jj) = patch_pix[ii*sz_C+jj];
//					  inrates_E.at(ii*sz_C+jj) = patch_pix[ii*sz_C+jj]/(float) 16.;
					  inrates_E.at(ii*sz_C+jj) = p[jj];
//					  cout<<patch.type()<<" "<<CV_32FC1<<endl;
				  }
			  }

#ifdef DEBUG
      writeVecLine(inrates_E, infilename);
#endif

      dctNet.rundct(&sim,inrates_E);
      vector<float> patchpos = dctNet.getpos();
      vector<float> patchneg = dctNet.getneg();

#ifdef DEBUG
      writeVecLine(patchpos, patchposfilename);
      writeVecLine(patchneg, patchnegfilename);
#endif

      vector<float> posPipe(sz_R*sz_C,0.0);
      vector<float> negPipe(sz_R*sz_C,0.0);
      for(int ip=0; ip<sz_R*sz_C; ip++)
      {
    	  float diff = patchpos.at(ip) - patchneg.at(ip);
    	  if ( diff  > 0)
    	  	  posPipe.at(ip) = 16.0;
    	  else if (diff < 0)
    	  	  negPipe.at(ip) = 16.0;
      }

#ifdef DEBUG
      writeVecLine(posPipe, posPipefilename);
      writeVecLine(negPipe, negPipefilename);
#endif

      dctNet.rundct(&sim,posPipe);
      vector<float> fr_idct2pp = dctNet.getpos();
      vector<float> fr_idct2pm = dctNet.getneg();

      dctNet.rundct(&sim,negPipe);
      vector<float> fr_idct2mp = dctNet.getpos();
      vector<float> fr_idct2mm = dctNet.getneg();

#ifdef DEBUG
      writeVecLine(fr_idct2pp,fullfilename);
      writeVecLine(fr_idct2pm,fullfilename);
      writeVecLine(fr_idct2mp,fullfilename);
      writeVecLine(fr_idct2mm,fullfilename);
      writeVecLine(patchpos,dctfilename);
      writeVecLine(patchneg,dctfilename);
#endif


      float* sal_pix = (float *) sal_patch[chan].data;
      for (int ii = 0; ii<sz_R; ii++)
      {
    	  for (int jj=0; jj<sz_C; jj++)
    	  {
    		  sal_pix[ii*sz_C+jj] =
    		  fr_idct2pp[ii*sz_C+jj]
    		- fr_idct2pm[ii*sz_C+jj]
	        + fr_idct2mp[ii*sz_C+jj]
	        - fr_idct2mm[ii*sz_C+jj];
          }
      }

      // recombine (shaped by weighted average
      //img_sal(patch_rect) =  img_sal(patch_rect) + sal_patch[chan].mul(pyramid);
      img_sal(patch_rect) =  img_sal(patch_rect) + sal_patch[chan].mul(pyramid);
      printf("patch (%d,%d) chan %d done\n\n",i,j,chan);
      //imshow("foo1",sal_patch[0]);
      //waitKey(0);
    }

  }
  //normalize the patch pixels to 0..63 -- (four channel sum = 8-bits/255)
#ifdef DEBUG
  writeVecLine(img_sal,fullfilename);
#endif

  normalize(img_sal, img_sal, 0.0 , 255.0, NORM_MINMAX);

#ifdef DEBUG
  writeVecLine(img_sal,fullfilename);
#endif
  imwrite("sal.bmp",img_sal);
  imshow("foo1",img_sal);
  waitKey(0);
	//img_sal.convertTo(img_sal_int,CV_8UC1);
	//imwrite("sal.bmp",img_sal_int);
	//imshow("foo",img_sal_int);
	//waitKey(0);
}

#ifdef DEBUG
void writeVecLine(std::vector<float> M, string filename)
{
  fstream fout;
  fout.open(filename.c_str(), std::fstream::out | std::fstream::app);
  // appends vector on one csv line to fout stream
    if(!fout) {
        cout<<"File Not Opened"<<endl;  return;
    }

    for(int i=0; i<M.size(); i++)
      fout<<M.at(i)<<",";

    fout<<endl;
    fout.close();
}
#endif
