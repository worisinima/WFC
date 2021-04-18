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
class FVector2D
{
public:
	FVector2D(Type x, Type y) : X(x), Y(y) {}
	FVector2D() :X(0), Y(0) {}
	~FVector2D() {}
	FVector2D operator+ (const FVector2D& Ref) { FVector2D ret; ret.X = this->X + Ref.X; ret.Y = this->Y + Ref.Y; return ret; }
	FVector2D operator- (const FVector2D& Ref) { FVector2D ret; ret.X = this->X - Ref.X; ret.Y = this->Y - Ref.Y; return ret; }
	FVector2D operator+ (const FVector2D& Ref)const { FVector2D ret; ret.X = this->X + Ref.X; ret.Y = this->Y + Ref.Y; return ret; }
	FVector2D operator- (const FVector2D& Ref)const { FVector2D ret; ret.X = this->X - Ref.X; ret.Y = this->Y - Ref.Y; return ret; }
	FVector2D operator* (const FVector2D& Ref) { FVector2D ret; ret.X = this->X * Ref.X; ret.Y = this->Y * Ref.Y; return ret; }
	FVector2D operator* (const Type& fval)const { FVector2D ret; ret.X = this->X * fval; ret.Y = this->Y * fval; return ret; }
	FVector2D operator/ (const FVector2D& Ref) { FVector2D ret; ret.X = this->X / Ref.X; ret.Y = this->Y / Ref.Y; return ret; }
	FVector2D operator/ (const Type& fval) { FVector2D ret; ret.X = this->X / fval; ret.Y = this->Y / fval; return ret; }
	void operator= (const FVector2D& Ref) { this->X = Ref.X; this->Y = Ref.Y; }
	bool operator== (const FVector2D& Ref)const { return this->X == Ref.X && this->Y == Ref.Y; }
	bool operator!= (const FVector2D& Ref)const { return this->X != Ref.X || this->Y != Ref.Y; }
	void Print() { cout << "X: " << X << "  " << "Y: " << Y << endl; }
	Type X, Y;
};

class vec3
{
public:
	vec3(){e[0] = 0; e[1] = 0; e[2] = 0;}
	vec3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
	double x() const { return e[0]; } double y() const { return e[1]; } double z() const { return e[2]; }
	double r() const { return e[0]; } double g() const { return e[1]; } double b() const { return e[2]; }
	const vec3& operator+() const { return *this; }
	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }
	vec3& operator+=(const vec3& v) { e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2]; return *this; }
	vec3& operator*=(const vec3& v) { e[0] *= v.e[0]; e[1] *= v.e[1]; e[2] *= v.e[2]; return *this; }
	vec3& operator/=(const vec3& v) { e[0] /= v.e[0]; e[1] /= v.e[1]; e[2] /= v.e[2]; return *this; }
	vec3& operator-=(const vec3& v) { e[0] -= v.e[0]; e[1] -= v.e[1]; e[2] -= v.e[2]; return *this; }
	vec3& operator*=(const double t) { e[0] *= t; e[1] *= t; e[2] *= t; return *this; }
	vec3& operator/=(const double t) { double k = 1.0 / t; e[0] *= k; e[1] *= k; e[2] *= k; return *this; }
	double length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
	double squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	void make_unit_vector() { double k = 1.0 / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); e[0] *= k; e[1] *= k; e[2] *= k; }
	double e[3];
};
std::istream& operator >> (std::istream& is, vec3& t) {is >> t.e[0] >> t.e[1] >> t.e[2];return is;}
std::ostream& operator<<(std::ostream& os, const vec3& t) {os << t.e[0] << " " << t.e[1] << " " << t.e[2];return os;}
vec3 operator+(const vec3& v1, const vec3& v2) {return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);}
vec3 operator-(const vec3& v1, const vec3& v2) {return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);}
vec3 operator*(const vec3& v1, const vec3& v2) {return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);}
vec3 operator/(const vec3& v1, const vec3& v2) {return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);}
vec3 operator*(double t, const vec3& v) {return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);}
vec3 operator/(vec3 v, double t) {return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);}
vec3 operator*(const vec3& v, double t) {return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);}
double dot(const vec3& v1, const vec3& v2) {return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];}
vec3 cross(const vec3& v1, const vec3& v2) {return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));}
vec3 normalize(vec3 v) {return v / v.length();}

class Float3X3
{
	Float3X3() {e[0] = 0; e[1] = 0; e[2] = 0;e[3] = 0; e[4] = 0; e[5] = 0;e[6] = 0; e[7] = 0; e[8] = 0;}
	Float3X3(const float& v_00, const float& v_01, const float& v_02,const float& v_10, const float& v_11, const float& v_12,const float& v_20, const float& v_21, const float& v_22)
	{
		e[0] = v_00; e[1] = v_01; e[2] = v_02;
		e[3] = v_10; e[4] = v_11; e[5] = v_12;
		e[6] = v_20; e[7] = v_21; e[8] = v_22;
	}
	float e[9];
};

vec3 mul(const Float3X3& matrix, const vec3& vec)
{
	vec3 Ret;

	return Ret;
}
vec3 mul(const vec3& vec, const Float3X3& matrix)
{
	vec3 Ret;

	return Ret;
}

class FImage
{
public:
	//创建默认是黑色填充
	FImage(int X, int Y, string ImageName) :
		SizeX(X),
		SizeY(Y),
		ImageName(ImageName)
	{
		for (int i = 0; i < SizeX; i++)
		{
			vector<FColor> row(SizeY);
			Imagedata.push_back(row);
		}
	}
	bool SaveImageToDesk()
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
				FColor& color = Imagedata[x][y];
				fout << color.R << " " << color.G << " " << color.B << "\n";
			}
		}
		fout.close();
		cout << endl << "Save image successfully" << endl;
		return bCreateImage;
	}
	void ClearImage(FColor& ClearColor)
	{
		cout << endl << "Begin clear image operation" << endl;
		int nx = SizeX; int ny = SizeY;
		for (int i = 0; i < nx; i++)
		{
			for (int j = 0; j < ny; j++)
			{
				Imagedata[i][j] = ClearColor;
			}
		}
	}
	void SetPixleColor(const FColor& newData, const FVector2D<int>& PixleLocation)
	{
		if (PixleLocation.X < SizeX && PixleLocation.Y < SizeY) Imagedata[PixleLocation.X][PixleLocation.Y] = newData;
	}
	void SetPixleColor(const int& newData, const FVector2D<int>& PixleLocation)
	{
		if (PixleLocation.X < SizeX && PixleLocation.Y < SizeY) Imagedata[PixleLocation.X][PixleLocation.Y] = FColor(newData);
	}

	int SizeX, SizeY;
	string ImageName, ImagePath;
	vector<vector<FColor>> Imagedata;
};

//如果要生成2D迷宫就打开这里
#if 1
int main()
{
	vec3 a = vec3(1, 0, 0);
	vec3 b = vec3(0, 1, 0);
	cout << cross(a, b) << endl;

	return 0;
}
#endif