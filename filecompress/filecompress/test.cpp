#include "FileCompress.h"

int main()
{
	FileCompress fc;//类的实例化
	fc.CompressFile("wx_camera_1549296221033.mp4");
	fc.UNCompressFile("wx_camera_1549296221033.hzp");
	system("pasue");
	return 0;
}