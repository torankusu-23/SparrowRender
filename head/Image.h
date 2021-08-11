//图形的读取、存储和显示格式是.tga文件格式
//*------------------------------------------------------bpp相关很多结构,未必有用，待删
#pragma once
#include "fstream"

#pragma pack(push, 1)	//默认对齐系数(8?)压入栈，并设置当前对齐系数为1个字节8位，显示256个数字
struct TGA_Attr		//TGA文件属性 refer:https://blog.csdn.net/m0_46293129/article/details/105330102
{
	char idlength;			//指出图像信息字段长度，取值范围0~255
	char colormaptype;		//0：不使用颜色表 1：使用颜色表
	char datatypecode;		//0：没有图像数据 1：未压缩的颜色表图像 2：未压缩的真彩图像 3：未压缩的黑白图像 9：RLE压缩的颜色表图像 10：RLE压缩的真彩图像 11：RLE压缩的黑白图像
	short colormaporigin;	//颜色表首址
	short colormaplength;	//颜色表长度
	char colormapdepth;		//颜色表项位数
	short x_origin;			//图像X坐标起始位置
	short y_origin;			//图像Y坐标起始位置
	short width;			//图像宽度
	short height;			//图像高度
	char  bitsperpixel;		//图像每像素存储占用位数,值为8、16、24、32等
	char  imagedescriptor;	
};
#pragma pack(pop)	//默认系数出栈

struct Color		//颜色数据结构
{
	unsigned char rgba[4];	//unsigned char 占用1个字节，0~255
	unsigned char bpp;		//bytes per pixel，每个像素申请的字节个数，灰度图1，rgb图3，rgba图4

	Color() : rgba(), bpp(1){for (int i = 0; i < 4; i++) rgba[i] = 0;}

	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) : rgba(), bpp(4)
	{
		rgba[0] = r;
		rgba[1] = g;
		rgba[2] = b;
		rgba[3] = a;
	}

	Color(const unsigned char* color, unsigned char _bpp) : rgba(), bpp(_bpp)	//扩充或收缩颜色结构
	{
		for (int i = 0; i < int(bpp); i++) rgba[i] = color[i];
		for (int i = int(bpp); i < 4; i++) rgba[i] = 0;
	}

	unsigned char& operator[](const int i) { return rgba[i]; }	//ez for use

	Color operator*(float coef)	
	{
		coef = (coef > 1.f ? 1.f : (coef < 0.f ? 0.f : coef));
		for (int i = 0; i < 3; i++) this->rgba[i] *= coef;	//alpha可以不变
		return *this;
	}
};

class Image	//TGA图像类
{
private:
	unsigned char* buffer;
	int width;
	int height;
	int bpp;
	bool load_rle_data(std::ifstream& in);		//读取rle压缩tga文件
	bool unload_rle_data(std::ofstream& out);	//输出rle压缩tga文件

public:
	enum FORMAT {GRAYSCALE = 1, RGB = 3, RGBA = 4};	//

	Image();
	~Image();
	Image(int width, int height, int bpp);
	Image(const Image& image);
	Image& operator=(const Image& image);

	bool read_tga_file(const char* filename);
	bool write_tga_file(const char* filename, bool rle = true);	//? what coef
	bool flip_horizontally();	//水平翻转
	bool flip_vertically();		//垂直翻转
	bool scale(int width, int height);

	Color get_color(int x, int y);
	bool set_color(int x, int y, const Color& c);

	unsigned char* get_buffer();
	int get_width();
	int get_height();
	int get_bpp();
	void clear();
};