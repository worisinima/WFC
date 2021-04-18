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
				//如果是目录,迭代之  
				//如果不是,加入列表  
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						GetFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
				else
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}

	/*	Example:
		string path;
		FileHelper::GetProjectPath(path);
		假设在E盘的工程，那么这个值会为：
		"E:\\DX12\\DX12Lab\\DX12Lab"
	*/
	static void GetProjectPath(string& OutPath)
	{
		char* buffer;
		//也可以将buffer作为输出参数
		if ((buffer = _getcwd(NULL, 0)) != NULL)
		{
			OutPath = buffer;
		}
	}

	/*	CSIDL_BITBUCKET 回收站
		CSIDL_CONTROLS 控制面板
		CSIDL_DESKTOP Windows桌面desktop;
		CSIDL_DESKTOPDIRECTORY desktop的目录；
		CSIDL_DRIVES 我的电脑
		CSIDL_FONTS 字体目录
		CSIDL_NETHOOD 网上邻居
		CSIDL_NETWORK 网上邻居virtual folder
		CSIDL_PERSONAL 我的文档
		CSIDL_PRINTERS 打印机
		CSIDL_PROGRAMS 程序组
		CSIDL_RECENT 最近打开文档
		CSIDL_SENDTO 发送到菜单项
		CSIDL_STARTMENU 快启菜单
		CSIDL_STARTUP 启动目录
		CSIDL_TEMPLATES 临时文档
	*/

	static string GetDesktopPath()
	{
		LPITEMIDLIST pidl;
		LPMALLOC pShellMalloc;
		char szDir[1024];
		if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
		{
			if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) {
				// 如果成功返回true  
				SHGetPathFromIDListA(pidl, szDir);
				pShellMalloc->Free(pidl);
			}
			pShellMalloc->Release();
		}

		string Ret = string(szDir);
		//Ret.replace(Ret.find_first_of(""), filePath.length(), "")

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

		int nx = SizeX;
		int ny = SizeY;

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
		if (PixleLocation.X < SizeX && PixleLocation.Y < SizeY)
			Imagedata[PixleLocation.X][PixleLocation.Y] = newData;
	}
	void SetPixleColor(const int& newData, const FVector2D<int>& PixleLocation)
	{
		if (PixleLocation.X < SizeX && PixleLocation.Y < SizeY)
			Imagedata[PixleLocation.X][PixleLocation.Y] = FColor(newData);
	}

	int SizeX;
	int SizeY;
	string ImageName;
	string ImagePath;
	vector<vector<FColor>> Imagedata;
};

struct FSprite4X4
{
	FSprite4X4(
		const FColor& p0,
		const FColor& p1,
		const FColor& p2,
		const FColor& p3,
		const FColor& p4,
		const FColor& p5,
		const FColor& p6,
		const FColor& p7,
		const FColor& p8,
		const FColor& p9,
		const FColor& p10,
		const FColor& p11,
		const FColor& p12,
		const FColor& p13,
		const FColor& p14,
		const FColor& p15
	)
	{
		ImageData = new vector<FColor>();
		ImageData->push_back(p0);ImageData->push_back(p1);ImageData->push_back(p2);ImageData->push_back(p3);
		ImageData->push_back(p4);ImageData->push_back(p5);ImageData->push_back(p6);ImageData->push_back(p7);
		ImageData->push_back(p8);ImageData->push_back(p9);ImageData->push_back(p10);ImageData->push_back(p11);
		ImageData->push_back(p12);ImageData->push_back(p13);ImageData->push_back(p14);ImageData->push_back(p15);
	}
	vector<FColor>* ImageData;
};

float RandInRange(const int& min, const int& max)
{
	return (rand() % (max - min + 1)) + min;
}

bool InRange(const float& val, const float& min, const float& max, bool includeBoarder = false)
{
	if (includeBoarder) { if (val >= min && val <= max) return true; else return false; }
	else { if (val > min && val < max) return true; else return false; }
}

//2D AABB box 相交测试，如果相交返回true，如果没有返回false
template<class type>
bool IsAABB2DIntersect(const FVector2D<type>& A_Min, const FVector2D<type>& A_Max, const FVector2D<type>& B_Min, const FVector2D<type>& B_Max)
{
	return A_Min.X < B_Max.X&& A_Max.X > B_Min.X && A_Min.Y < B_Max.Y&& A_Max.Y > B_Min.Y;
}

/*
* 0 0 0 0 0 0 0 0 0 0 0 0 0
* 0 0 0 0 0 0 0 0 0 0 0 0 0
* 0 0 1 1 1 1 1 1 1 1 1 0 0
* 0 0 1 2 2 2 2 2 2 2 1 0 0
* 0 0 1 2 3 3 3 3 3 2 1 0 0
* 0 0 1 2 3 3 3 3 3 2 1 0 0
* 0 0 1 2 3 3 3 3 3 2 1 0 0
* 0 0 1 2 3 3 3 3 3 2 1 0 0
* 0 0 1 2 2 2 2 2 2 2 1 0 0
* 0 0 1 1 1 1 1 1 1 1 1 0 0
* 0 0 0 0 0 0 0 0 0 0 0 0 0
* 0 0 0 0 0 0 0 0 0 0 0 0 0
* 
* None      VE_0	 VE_1	  VE_2
* 0 0 0 0   0 0 0 0  0 0 0 0  0 0 0 0
* 0 0 0 0	0 0 0 0	 0 0 0 0  0 0 0 0
* 0 0 0 0	0 0 1 1	 1 1 1 1  1 1 0 0
* 0 0 0 0	0 0 1 2	 2 2 2 2  2 1 0 0
* 以此类推
* 
*/

enum class EProtoType : unsigned char
{
	VE_None,
	VE_0,
	VE_1,
	VE_2,
	VE_3,
	VE_4,
	VE_5,
	VE_6,
	VE_7,
	VE_8,
	VE_9,
	VE_10,
	VE_11,
	VE_12,
};
#define en EProtoType::VE_None
#define e0 EProtoType::VE_0
#define e1 EProtoType::VE_1
#define e2 EProtoType::VE_2
#define e3 EProtoType::VE_3
#define e4 EProtoType::VE_4
#define e5 EProtoType::VE_5
#define e6 EProtoType::VE_6
#define e7 EProtoType::VE_7
#define e8 EProtoType::VE_8
#define e9 EProtoType::VE_9
#define e10 EProtoType::VE_10
#define e11 EProtoType::VE_11
#define e12 EProtoType::VE_12

struct FProto
{
	FProto()
	{
		NeighborLists = new vector<vector<EProtoType>*>();
		vector<EProtoType>* px = new vector<EProtoType>();
		vector<EProtoType>* nx = new vector<EProtoType>();
		vector<EProtoType>* py = new vector<EProtoType>();
		vector<EProtoType>* ny = new vector<EProtoType>();
		NeighborLists->push_back(px);
		NeighborLists->push_back(nx);
		NeighborLists->push_back(py);
		NeighborLists->push_back(ny);

		ProtoType = EProtoType::VE_None;
		SpriteVal = nullptr;
	}

	vector<vector<EProtoType>*>* NeighborLists;
	EProtoType ProtoType;
	FSprite4X4* SpriteVal;
};

struct FCell
{
	FCell()
	{
		bCollapse = false;
		CollapseProto = nullptr;
		ProtoList = new vector<FProto*>();
	}

	bool bCollapse;
	FProto* CollapseProto;
	vector<FProto*>* ProtoList;
	FVector2D<int> CellLocation;
};

struct WFC
{
	WFC(const int& x, const int& y) : SizeX(x), SizeY(y)
	{
		CellList = new vector<FCell*>();
		PropagationCellList = new vector<FCell*>();
		ObservationCellList = new vector<FCell*>();

		//Begin build our proto
		ProtoAssetList = new vector<FProto*>();

		FColor cg = FColor(0, 74, 28);
		FColor cy = FColor(140, 105, 21);
		FColor dy = FColor(75, 57, 13);

		//Proto_None
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_None;
			newProto->SpriteVal = new FSprite4X4(
				0,	0,	0,	0,
				0,	0,	0,	0,
				0,	0,	0,	0,
				0,	0,	0,	0
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { en, e0, e3, e6 };
			*NXList = { en, e2, e5, e8 };
			*PYList = { en, e0, e1, e2 };
			*NYList = { en, e6, e7, e8 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_0
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_0;
			newProto->SpriteVal = new FSprite4X4(
				0,	0,	0,	0,
				0,	0,	0,	0,
				0,	0,	cg,	cg,
				0,	0,	cg,	cy
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e1, e2, e9};
			*NXList = { e2, e5, e8 };
			*PYList = { e3, e6, e9 };
			*NYList = { e6, e7, e8 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_1
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_1;
			newProto->SpriteVal = new FSprite4X4(
				0,	0,	0,	0,
				0,	0,	0,	0,
				cg, cg, cg, cg,
				cy, cy, cy, cy
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e1, e2, e9 };
			*NXList = { e1, e0, e10 };
			*PYList = { e4, e7, e11, e12 };
			*NYList = { e6, e7, e8 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_2
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_2;
			newProto->SpriteVal = new FSprite4X4(
				0,	0,	0, 0,
				0,	0,	0, 0,
				cg, cg, 0, 0,
				cy, cg, 0, 0
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e0, e3, e6 };
			*NXList = { e1, e0, e10 };
			*PYList = { e5, e8, e10 };
			*NYList = { e6, e7, e8 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_3
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_3;
			newProto->SpriteVal = new FSprite4X4(
				0,	0,	cg, cy,
				0,	0,	cg, cy,
				0,	0,	cg, cy,
				0,	0,	cg, cy
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e4, e5, e10, e12};
			*NXList = { e2, e5, e8 };
			*PYList = { e3, e6, e9};
			*NYList = { e3, e0, e11 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_4
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_4;
			newProto->SpriteVal = new FSprite4X4(
				dy, dy, dy, dy,
				dy, dy, dy, dy,
				dy, dy, dy, dy,
				dy, dy, dy, dy
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e4, e5, e10, e12 };
			*NXList = { e4, e3, e9, e11 };
			*PYList = { e4, e7, e11, e12 };
			*NYList = { e4, e1, e9, e10 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_5
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_5;
			newProto->SpriteVal = new FSprite4X4(
				cy, cg, 0, 0,
				cy, cg, 0, 0,
				cy, cg, 0, 0,
				cy, cg, 0, 0
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e0, e3, e6 };
			*NXList = { e4, e3, e9, e11 };
			*PYList = { e5, e8, e10 };
			*NYList = { e2, e5, e12 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_6
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_6;
			newProto->SpriteVal = new FSprite4X4(
				0, 0, cg, cy,
				0, 0, cg, cg,
				0, 0, 0, 0,
				0, 0, 0, 0
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e7, e8, e11 };
			*NXList = { e2, e5, e8 };
			*PYList = { e0, e1, e2 };
			*NYList = { e0, e3, e11 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_7
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_7;
			newProto->SpriteVal = new FSprite4X4(
				cy, cy, cy, cy,
				cg, cg, cg, cg,
				0, 0, 0, 0,
				0, 0, 0, 0
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e7, e8, e11 };
			*NXList = { e7, e6, e12 };
			*PYList = { e0, e1, e2 };
			*NYList = { e1, e4, e9, e10 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_8
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_8;
			newProto->SpriteVal = new FSprite4X4(
				cy, cg, 0, 0,
				cg, cg, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e0, e3, e6 };
			*NXList = { e6, e7, e12 };
			*PYList = { e0, e1, e2 };
			*NYList = { e2, e5, e12 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_9
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_9;
			newProto->SpriteVal = new FSprite4X4(
				0,	0,	cg, cy,
				0,	0,	cg, cy,
				cg, cg, cg, cy,
				cy, cy, cy, cy
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e4, e5, e10, e12};
			*NXList = { e0, e1, e10};
			*PYList = { e4, e7, e11, e12 };
			*NYList = { e0, e3, e11};
			ProtoAssetList->push_back(newProto);
		}
		//Proto_10
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_10;
			newProto->SpriteVal = new FSprite4X4(
				cy,	cg,	0, 0,
				cy,	cg,	0, 0,
				cy,	cg, cg, cg,
				cy, cy, cy, cy
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e1, e2, e9 };
			*NXList = { e3, e4, e9, e11 };
			*PYList = { e4, e7, e12, e11 };
			*NYList = { e2, e5, e12 };
			ProtoAssetList->push_back(newProto);
		}
		//Proto_11
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_11;
			newProto->SpriteVal = new FSprite4X4(
				cy, cy, cy, cy,
				cg, cg, cg, cy,
				0,	0,	cg, cy,
				0,	0,	cg, cy
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e4, e5, e10, e12 };
			*NXList = { e6, e7, e10, e12};
			*PYList = { e3, e6, e9};
			*NYList = { e1, e4, e9, e10};
			ProtoAssetList->push_back(newProto);
		}
		//Proto_12
		{
			FProto* newProto = new FProto();
			newProto->ProtoType = EProtoType::VE_12;
			newProto->SpriteVal = new FSprite4X4(
				cy, cy, cy, cy,
				cy, cg, cg, cg,
				cy, cg, 0, 0,
				cy, cg, 0, 0
			);
			vector<EProtoType>*& PXList = newProto->NeighborLists->at(0);
			vector<EProtoType>*& NXList = newProto->NeighborLists->at(1);
			vector<EProtoType>*& PYList = newProto->NeighborLists->at(2);
			vector<EProtoType>*& NYList = newProto->NeighborLists->at(3);
			*PXList = { e7, e8, e11 };
			*NXList = { e3, e4, e9, e11 };
			*PYList = { e5, e8, e10};
			*NYList = { e2, e4, e9, e10 };
			ProtoAssetList->push_back(newProto);
		}
		//End build proto

		for (int y = 0; y < SizeY; y++)
		{
			for (int x = 0; x < SizeX; x++)
			{
				FCell* newCell = new FCell();
				for (auto var : *ProtoAssetList)
					newCell->ProtoList->push_back(var);
				newCell->CellLocation = FVector2D<int>(x, y);
				CellList->push_back(newCell);
			}
		}
	}

	void StartObservation(FVector2D<int>ObsevationCellLoc)
	{
		if (ObsevationCellLoc.X < SizeX && ObsevationCellLoc.Y < SizeY && ObsevationCellLoc.X >= 0 && ObsevationCellLoc.Y >= 0)
		{
			FCell*& ObCell = CellList->at(ObsevationCellLoc.Y * SizeX + ObsevationCellLoc.X);
			
			ObservationCellList->push_back(ObCell);
			Propagation();
		}
	}

private:
	vector<FCell*>* ObservationCellList;
	vector<FCell*>* PropagationCellList;

	//现在一个Cell发生了塌陷，此塌陷的Cell会影响周围的Cell，让周围的Cell也发生塌陷，现在把这些即将塌陷的
	//周围的Cell找出来，并且在此之前把它们的状态进行约束
	void AddCellToPropagationCellList(FCell* ObsCell, int& CellListLoc, const int& Direction)
	{
		FCell*& NeighborCell = CellList->at(CellListLoc);
		vector<FProto*>*& NeighborCellProtoList = NeighborCell->ProtoList;

		FProto*& CollapseProto = ObsCell->CollapseProto;
		vector<EProtoType>*& CollapseProtoDesireTypeList = CollapseProto->NeighborLists->at(Direction);

		vector<FProto*>* CellProtoListTemp = new vector<FProto*>();
		for (FProto* proto : *NeighborCellProtoList)
		{
			bool bFind = false;
			for (EProtoType val : *CollapseProtoDesireTypeList)
			{
				if (proto->ProtoType == val)
				{
					bFind = true;
					break;
				}
			}
			if (bFind == true)
			{
				CellProtoListTemp->push_back(proto);
			}
		}
		if (CellProtoListTemp->size() == 0)
		{
			FColor r = FColor(255, 0, 0);
			FProto* ErrorProto = new FProto();
			ErrorProto->SpriteVal = new FSprite4X4(
				r, r, r, r,
				r, r, r, r,
				r, r, r, r,
				r, r, r, r
			);
			CellProtoListTemp->push_back(ErrorProto);
			cout << "ErrorSprite(" << NeighborCell->CellLocation.X << ", " << NeighborCell->CellLocation.Y << ")" << endl;
			return;
		}
	
		NeighborCellProtoList->clear();
		for (auto var : *CellProtoListTemp) NeighborCellProtoList->push_back(var);
		CellProtoListTemp->clear(); delete(CellProtoListTemp); CellProtoListTemp = nullptr;

		//Add Unique To PropagationCellList
		bool bFind = false;
		for (const FCell* cell : *PropagationCellList)
		{
			if (cell == NeighborCell)
			{
				bFind = true;
				break;
			}
		}
		if (bFind == false)
		PropagationCellList->push_back(NeighborCell);
	}

	void Observation()
	{
		for (FCell* ObsCell : *ObservationCellList)
		{
			if (ObsCell->bCollapse == false)
			{
				ObsCell->CollapseProto = ObsCell->ProtoList->at((int)RandInRange(0, ObsCell->ProtoList->size() - 1));
				cout << "X:" << ObsCell->CellLocation.X << " Y:" << ObsCell->CellLocation.Y << endl;
				ObsCell->bCollapse = true;

				//把一个Cell塌陷以后，把它周围的未塌陷的cell收集到PropagationCellList的四个子List里用于下次观测使用
				int CellListLoc = 0;
				const int& CX = ObsCell->CellLocation.X;
				const int& CY = ObsCell->CellLocation.Y;
				//PX
				CellListLoc = CY * SizeX + CX + 1;
				if (CX + 1 < SizeX && CellList->at(CellListLoc)->bCollapse == false)
					AddCellToPropagationCellList(ObsCell, CellListLoc, 0);
				//NX
				CellListLoc = CY * SizeX + CX - 1;
				if (CX - 1 >= 0 && CellList->at(CellListLoc)->bCollapse == false)
					AddCellToPropagationCellList(ObsCell, CellListLoc, 1);
				//PY
				CellListLoc = (CY + 1) * SizeX + CX;
				if (CY + 1 < SizeY && CellList->at(CellListLoc)->bCollapse == false)
					AddCellToPropagationCellList(ObsCell, CellListLoc, 2);
				//NY
				CellListLoc = (CY - 1) * SizeX + CX;
				if (CY - 1 >= 0 && CellList->at(CellListLoc)->bCollapse == false)
					AddCellToPropagationCellList(ObsCell, CellListLoc, 3);
			}
		}
	}

	void Propagation()
	{
		while (true)
		{
			Observation();
			ObservationCellList->clear();

			if (PropagationCellList->size() == 0)
			{
				break;
			}
			else
			{
				for (FCell* cell : *PropagationCellList)
					ObservationCellList->push_back(cell);
				PropagationCellList->clear();
			}
		}
	}

private:
	vector<FProto*>* ProtoAssetList;

public:
	vector<FCell*>* CellList;
	int SizeX;
	int SizeY;
};

void DrawSprite4X4ToImage(FImage* OutImage, const FSprite4X4* InSprite, const FVector2D<int>& StartLoc)
{
	const int& ImageSizeX = OutImage->SizeX;
	const int& ImageSizeY = OutImage->SizeX;
	if (ImageSizeX >=4 && ImageSizeY >=4)
	{
		for (unsigned int y = 0; y < 4; y++)
		{
			for (unsigned int x = 0; x < 4; x++)
			{
				if (x + StartLoc.X < ImageSizeX && y + StartLoc.Y < ImageSizeY)
				{
					OutImage->Imagedata[x + (unsigned int)StartLoc.X][y + (unsigned int)StartLoc.Y] = InSprite->ImageData->at(x + y * 4);
				}
			}
		}
	}
}

//如果要生成2D迷宫就打开这里
#if 0
int main()
{
	//初始化随机数种子
	//srand((int)time(NULL));
	srand(0);

	WFC* WFCObj = new WFC(128, 128);

	WFCObj->StartObservation(FVector2D<int>(0, 0));

	FImage* ResImage = new FImage(WFCObj->SizeX * 4, WFCObj->SizeY * 4, "Output");
	for (const FCell* cell : *(WFCObj->CellList))
	{
		if (cell->bCollapse && cell->CollapseProto != nullptr && cell->CollapseProto->SpriteVal != nullptr)
		{
			const FSprite4X4* sprite = cell->CollapseProto->SpriteVal;
			const FVector2D<int>& cellLoc = cell->CellLocation * 4;

			//DrawSpriteToImage
			DrawSprite4X4ToImage(ResImage, sprite, cellLoc);
		}
	}
	ResImage->SaveImageToDesk();

	return 0;
}
#endif