#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include<random>
#include<stdlib.h>
#include<time.h>

#include <windows.h>
#include <io.h>
#include <wrl.h>
#include <array>
#include <direct.h>
#include "shlobj.h" 

#include <math.h>
#include <stdlib.h>

#include "FreeImage.h"
#pragma comment(lib, "FreeImage.lib")

using namespace std;

namespace FileHelper
{
	/*	Example:
		vector<string> paths;
		const string FilePath = "C:\\Users\\yivanli\\Desktop\\DX12Lab\\DX12Lab\\Textures";
		FileHelper::GetFiles(FilePath, paths);
	*/
	static void GetFiles(string path, vector<string>& files)
	{
		//文件句柄  
		long long hFile = 0;
		//文件信息  
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).c_str(), &fileinfo)) != -1)
		{
			do
			{
				if ((fileinfo.attrib & _A_SUBDIR))//如果是目录,迭代之, 如果不是,加入列表  
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						GetFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
				else
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}
	/*Example: string path; FileHelper::GetProjectPath(path);
	假设在E盘的工程，那么这个值会为："E:\\DX12\\DX12Lab\\DX12Lab"*/
	static void GetProjectPath(string& OutPath)
	{
		char* buffer;//也可以将buffer作为输出参数
		if ((buffer = _getcwd(NULL, 0)) != NULL) OutPath = buffer;
	}
	/*CSIDL_BITBUCKET 回收站 CSIDL_CONTROLS 控制面板 CSIDL_DESKTOP Windows桌面desktop; CSIDL_DESKTOPDIRECTORY desktop的目录；
	CSIDL_DRIVES 我的电脑 CSIDL_FONTS 字体目录 CSIDL_NETHOOD 网上邻居 CSIDL_NETWORK 网上邻居virtual folder CSIDL_PERSONAL 我的文档
	CSIDL_PRINTERS 打印机 CSIDL_PROGRAMS 程序组 CSIDL_RECENT 最近打开文档 CSIDL_SENDTO 发送到菜单项 CSIDL_STARTMENU 快启菜单
	CSIDL_STARTUP 启动目录 CSIDL_TEMPLATES 临时文档*/
	static string GetDesktopPath()
	{
		LPITEMIDLIST pidl; LPMALLOC pShellMalloc; char szDir[1024];
		if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
		{
			if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) { // 如果成功返回true  
				SHGetPathFromIDListA(pidl, szDir);
				pShellMalloc->Free(pidl);
			}
			pShellMalloc->Release();
		}
		string Ret = string(szDir);//Ret.replace(Ret.find_first_of(""), filePath.length(), "")
		return Ret;
	}
}

class FColor
{
public:

	FColor() :R(0), G(0), B(0) {}
	FColor(int r, int g, int b) :R(r), G(g), B(b) {}
	FColor(int val) :R(val), G(val), B(val) {}
	~FColor() {}
	int R, G, B;
};

template<class Type>
class vec2
{
public:
	vec2(Type x, Type y) : X(x), Y(y) {}
	vec2() :X(0), Y(0) {}
	~vec2() {}
	vec2 operator+ (const vec2& Ref) { vec2 ret; ret.X = this->X + Ref.X; ret.Y = this->Y + Ref.Y; return ret; }
	vec2 operator- (const vec2& Ref) { vec2 ret; ret.X = this->X - Ref.X; ret.Y = this->Y - Ref.Y; return ret; }
	vec2 operator+ (const vec2& Ref)const { vec2 ret; ret.X = this->X + Ref.X; ret.Y = this->Y + Ref.Y; return ret; }
	vec2 operator- (const vec2& Ref)const { vec2 ret; ret.X = this->X - Ref.X; ret.Y = this->Y - Ref.Y; return ret; }
	vec2 operator* (const vec2& Ref) { vec2 ret; ret.X = this->X * Ref.X; ret.Y = this->Y * Ref.Y; return ret; }
	vec2 operator* (const Type& fval)const { vec2 ret; ret.X = this->X * fval; ret.Y = this->Y * fval; return ret; }
	vec2 operator/ (const vec2& Ref) { vec2 ret; ret.X = this->X / Ref.X; ret.Y = this->Y / Ref.Y; return ret; }
	vec2 operator/ (const Type& fval) { vec2 ret; ret.X = this->X / fval; ret.Y = this->Y / fval; return ret; }
	void operator= (const vec2& Ref) { this->X = Ref.X; this->Y = Ref.Y; }
	bool operator== (const vec2& Ref)const { return this->X == Ref.X && this->Y == Ref.Y; }
	bool operator!= (const vec2& Ref)const { return this->X != Ref.X || this->Y != Ref.Y; }
	void Print() { cout << "X: " << X << "  " << "Y: " << Y << endl; }
	Type X, Y;
};

class vec3
{
public:
	vec3(){X = 0; Y = 0; Z = 0;}
	vec3(const double& e0, const double& e1, const double& e2) { X = e0; Y = e1; Z = e2; }
	const vec3& operator+() const { return *this; }
	vec3 operator-() const { return vec3(-X, -Y, -Z); }
	vec3& operator+=(const vec3& v) { X += v.X; Y += v.Y; Z += v.Z; return *this; }
	vec3& operator*=(const vec3& v) { X *= v.X; Y *= v.Y; Z *= v.Z; return *this; }
	vec3& operator/=(const vec3& v) { X /= v.X; Y /= v.Y; Z /= v.Z; return *this; }
	vec3& operator-=(const vec3& v) { X -= v.X; Y -= v.Y; Z -= v.Z; return *this; }
	vec3& operator*=(const double& t) { X *= t; Y *= t; Z *= t; return *this; }
	vec3& operator/=(const double& t) { double k = 1.0 / t; X *= k; Y *= k; Z *= k; return *this; }
	double length() const { return sqrt(X * X + Y * Y + Z * Z); }
	double squared_length() const { return X * X + Y * Y + Z * Z; }
	void make_unit_vector() { double k = 1.0 / sqrt(X * X + Y * Y + Z * Z); X *= k; Y *= k; Z *= k; }
	void Print(){ cout << "( " << X << ", " << Y << ", " << Z << " )" << endl; }
	double X, Y, Z;
};
std::istream& operator >> (std::istream& is, vec3& t) {is >> t.X >> t.Y >> t.Z;return is;}
std::ostream& operator<<(std::ostream& os, const vec3& t) {os << t.X << " " << t.Y << " " << t.Z;return os;}
vec3 operator+(const vec3& v1, const vec3& v2) {return vec3(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);}
vec3 operator-(const vec3& v1, const vec3& v2) {return vec3(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);}
vec3 operator*(const vec3& v1, const vec3& v2) {return vec3(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);}
vec3 operator/(const vec3& v1, const vec3& v2) {return vec3(v1.X / v2.X, v1.Y / v2.Y, v1.Z / v2.Z);}
vec3 operator*(double t, const vec3& v) {return vec3(t * v.X, t * v.Y, t * v.Z);}
vec3 operator/(vec3 v, double t) {return vec3(v.X / t, v.Y / t, v.Z / t);}
vec3 operator*(const vec3& v, const double& t) {return vec3(t * v.X, t * v.Y, t * v.Z);}
double dot(const vec3& v1, const vec3& v2) {return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;}
vec3 cross(const vec3& v1, const vec3& v2) {return vec3((v1.Y * v2.Z - v1.Z * v2.Y),(-(v1.X * v2.Z - v1.Z * v2.X)),(v1.X * v2.Y - v1.Y * v2.X));}
vec3 normalize(vec3 v) {return v / v.length();}

class float3x3
{
public:
	float3x3() { e00 = e01 = e02 = e10 = e11 = e12 = e20 = e21 = e22 = 0; }
	float3x3(const float& v_00, const float& v_01, const float& v_02, const float& v_10, const float& v_11, const float& v_12, const float& v_20, const float& v_21, const float& v_22)
	{
		e00 = v_00; e01 = v_01; e02 = v_02;
		e10 = v_10; e11 = v_11; e12 = v_12;
		e20 = v_20; e21 = v_21; e22 = v_22;
	}
	float e00; float e01; float e02; 
	float e10; float e11; float e12; 
	float e20; float e21; float e22;
	float3x3 Inverse()
	{
		float3x3 invMatrix;
		invMatrix.e00 = e00; invMatrix.e01 = e10; invMatrix.e02 = e20;
		invMatrix.e10 = e01; invMatrix.e11 = e11; invMatrix.e12 = e21;
		invMatrix.e20 = e02; invMatrix.e21 = e12; invMatrix.e22 = e22;
		return invMatrix;
	}
};
vec3 mul(const float3x3& matrix, const vec3& vec)
{
	vec3 Ret;
	Ret.X = matrix.e00 * vec.X + matrix.e01 * vec.Y + matrix.e02 * vec.Z;
	Ret.Y = matrix.e10 * vec.X + matrix.e11 * vec.Y + matrix.e12 * vec.Z;
	Ret.Z = matrix.e20 * vec.X + matrix.e21 * vec.Y + matrix.e22 * vec.Z;
	return Ret;
}
vec3 mul(const vec3& vec, const float3x3& matrix)
{
	vec3 Ret;
	Ret.X = matrix.e00 * vec.X + matrix.e10 * vec.Y + matrix.e20 * vec.Z;
	Ret.Y = matrix.e01 * vec.X + matrix.e11 * vec.Y + matrix.e21 * vec.Z;
	Ret.Z = matrix.e02 * vec.X + matrix.e12 * vec.Y + matrix.e22 * vec.Z;
	return Ret;
}

class FImage
{
public:
	//创建默认是黑色填充
	FImage(const int& X, const int& Y, string ImageName) :SizeX(X), SizeY(Y), ImageName(ImageName)
	{
		Imagedata = new vector<FColor>();
		for (int i = 0; i < SizeX * SizeY; i++) 
			Imagedata->push_back(FColor(0));
	}
	bool SaveImageAsPPMToDesk()
	{
		const string& Path = FileHelper::GetDesktopPath();
		cout << endl << "begin save image to desk operation" << endl;
		bool bCreateImage = false;
		ofstream fout(Path + "/" + ImageName + ".ppm");
		fout << "P3\n" << SizeX << " " << SizeY << "\n255\n";
		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				FColor& color = (*Imagedata)[y * SizeX + x];
				fout << color.R << " " << color.G << " " << color.B << "\n";
			}
		}
		fout.close();
		cout << endl << "Save image successfully" << endl;
		return bCreateImage;
	}
	void SaveImageAsBMPToDesk()
	{
		FreeImage_Initialise();

		FIBITMAP* bitmap = FreeImage_Allocate(SizeX, SizeY, BPP);
		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				RGBQUAD val;
				val.rgbRed = (*Imagedata)[y * SizeX + x].R;
				val.rgbGreen = (*Imagedata)[y * SizeX + x].G;
				val.rgbBlue = (*Imagedata)[y * SizeX + x].B;
				FreeImage_SetPixelColor(bitmap, x, y, &val);
			}
		}
		const string& Path = FileHelper::GetDesktopPath();
		FreeImage_Save(FIF_BMP, bitmap, (Path + "/" + ImageName + ".bmp").c_str());
		FreeImage_DeInitialise();
	}
	void ClearImage(const FColor& ClearColor)
	{
		cout << endl << "Begin clear image operation" << endl;
		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				(*Imagedata)[y * SizeX + x] = ClearColor;
			}
		}
	}
	void SetPixleColor(const FColor& newData, const vec2<int>& PixleLocation)
	{
		if (PixleLocation.X < SizeX && PixleLocation.Y < SizeY) (*Imagedata)[PixleLocation.Y * SizeX + PixleLocation.X] = newData;
	}
	void SetPixleColor(const int& newData, const vec2<int>& PixleLocation)
	{
		if (PixleLocation.X < SizeX && PixleLocation.Y < SizeY) (*Imagedata)[PixleLocation.Y * SizeX + PixleLocation.X] = FColor(newData);
	}

	int SizeX, SizeY;
	string ImageName, ImagePath;
	vector<FColor>* Imagedata;
	int BPP = 24;// Since we’re outputting three 8bit RGB values
};

//如果要生成就打开这里
#if 1
int main()
{
	FImage* outImage = new FImage(64, 64, "OutImage");
	for (unsigned int y = 0; y < outImage->SizeY; y++)
	{
		for (unsigned int x = 0; x < outImage->SizeX; x++)
		{
			FColor OutColor;
			vec2<float> uv = vec2<float>((float)x / (float)(outImage->SizeX), (float)y / (float)(outImage->SizeY));

			OutColor = FColor(uv.X * 255, uv.Y * 255, 0);
			outImage->SetPixleColor(OutColor, vec2<int>(x, y));
		}
	}
	outImage->SaveImageAsBMPToDesk();

	return 0;
}
#endif