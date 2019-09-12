#ifndef _BMPLOADER_H_
#define _BMPLOADER_H_

#define swapcolor(a,b){ \
        (a) ^= (b);     \
        (b) ^= (a);     \
        (a) ^= (b);     \
}

class BmpLoader {
private:
	unsigned char* header;//文件头
	unsigned int dataPos;//读取位置
	unsigned int width, height;//图片宽度 高度
	unsigned int imageSize;//图片内容大小
	unsigned short bit;//位数
	unsigned char* tmp;
public:
	unsigned char* data;//图片内容 rgb
	bool hasAlpha;
	BmpLoader();
	~BmpLoader();
	int getWidth();
	int getHeight();
	bool loadBitmap(const char* fileName);
};

#endif
