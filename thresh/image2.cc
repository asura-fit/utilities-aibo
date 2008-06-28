/*
  image2.cc
*/

#include "image2.h"

#include <stdio.h>
#include <fstream>
using namespace std;

const string PREF_VERSION  = "#CameraVersion:";
const string PREF_SETTINGS = "#CameraSettings:";

bool simage2::load(const char *filename)
{
  ifstream ifs(filename, ios::ate | ios::binary);
  if (!ifs.is_open()) {
	return false;
  }

  string imgsize_str, str;
  unsigned int imgsize, len = ifs.tellg();
  ifs.seekg(ios::beg);

  /* initialize */
  comment.clear();
  info.clear();
  delete [] img;
  img = NULL;
  w = h = 0;

  try {

	/* check the format of this image file */
	if (getline(ifs, str).bad() || str != "P6") {
	  throw "Wrong file format";
	}

	/* read comments */
	while (ifs.peek() == '#') {
	  if (getline(ifs, str).bad()) {
		throw "Wrong file format";
	  } else if (str.find(PREF_VERSION) == 0) {
		info.version_str = str.substr(PREF_VERSION.size());
	  } else if (str.find(PREF_SETTINGS) == 0) {
		str = str.substr(PREF_SETTINGS.size());
		info.settings_str = str.substr(0, str.find(';'));
	  } else {
		comment.push_back(str.substr(1));
	  }
	}
//	if (info.version_str == "" || info.settings_str == "") {
//	  throw "Wrong file format";
//	}

	/* read image infomations -- width, height and depth */
	/* but 'depth' infomation is ignored */
	if (getline(ifs, imgsize_str).bad() || getline(ifs, str).bad() ||
		sscanf(imgsize_str.c_str(), "%d %d", &w, &h) != 2) {
	  throw "Wrong file format";
	}
	imgsize = w * h;

	/* read image datas */
	if (len - ifs.tellg() != imgsize * sizeof(rgb)) {
	  throw "Wrong file format";
	}
	if ((img = new rgb[imgsize]) == NULL) {
	  throw "Memory allocating failed";
	}
	ifs.read((char *)img, imgsize * sizeof(rgb));

  } catch (...) {
	comment.clear();
	info.clear();
	delete [] img;
	img = NULL;
	w = h = 0;
  }

  ifs.close();
  return (img != NULL);
}

bool simage2::save(const char *filename)
{
  vector<string>::iterator p;
  ofstream ofs;
  int imgsize = w * h;

  if (img == NULL) {
	return false;
  }

  ofs.open(filename, ios::binary);
  if (!ofs.is_open()) {
	return false;
  }

  ofs << "P6" << endl;
  for (p = comment.begin(); p != comment.end(); p++) {
	ofs << '#' << (*p) << endl;
  }
  ofs << PREF_VERSION << info.version_str << endl;
  ofs << PREF_SETTINGS << info.settings_str << endl;
  ofs << w << " " << h << endl << "255" << endl;
  ofs.write((char *)img, imgsize * sizeof(rgb));
  ofs.close();
  return true;
}

unsigned char int2byte(int i)
{
  if (i < 0) {
	return 0;
  } else if (i > 255) {
	return 255;
  } else {
	return (unsigned char)i;
  }
}

void simage2::YUV2RGB()
{
  int i, size = w * h;
  int y, u, v;

  for (i = 0; i < size; i++) {
	y = img[i].red;
	u = img[i].green * 2 - 255;
	v = img[i].blue * 2 - 255;
	img[i].red = int2byte(y + u);
	img[i].green = int2byte(int(y - 0.51 * u - 0.19 * v));
	img[i].blue = int2byte(y + v);
  }
}

void simage2::Fix()
{
  int i;

  for (i = 0; i < 16; i++) {
    img[(h - 1) * w + i] = img[(h - 2) * w + i];
  }
}

void simage2::AddComment(const string str)
{
  comment.push_back(str);
}

void simage2::RemoveComment(const string str)
{
  vector<string>::iterator p;

  for (p = comment.begin(); p != comment.end(); p++) {
	if ((*p) == str) {
	  comment.erase(p);
	}
  }
}

void simage2::GetCameraInfo(CameraInfo &info)
{
  info = this->info;
}

void simage2::SetCameraInfo(const CameraInfo info)
{
  this->info = info;
}
