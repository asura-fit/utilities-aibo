/*
  image2.h
*/

#ifndef __IMAGE2_H__
#define __IMAGE2_H__

#include "image.h"
#include <string>
#include <vector>

class CameraInfo {
 public:
  std::string version_str;
  std::string settings_str;

  CameraInfo() {
	version_str = settings_str = "";
  }
  void clear() {
	version_str = settings_str = "";
  }
  CameraInfo &operator =(const CameraInfo &info) {
	version_str = info.version_str;
	settings_str = info.settings_str;
	return *this;
  }
  bool operator ==(const CameraInfo &info) {
	return (version_str == info.version_str &&
			settings_str == info.settings_str);
  }
  bool operator !=(const CameraInfo &info) {
	return (version_str != info.version_str ||
			settings_str != info.settings_str);
  }
};

class simage2 : public simage {
 protected:
  std::vector<std::string> comment;
  CameraInfo info;

 public:
  bool load(const char *filename);
  bool save(const char *filename);

  void YUV2RGB();
  void Fix();

  void AddComment(const std::string str);
  void RemoveComment(const std::string str);
  void GetCameraInfo(CameraInfo &info);
  void SetCameraInfo(const CameraInfo info);
};

#endif /* __IMAGE2_H__ */
