#include <math.h>
#include <algorithm>
#include <fstream>
#include <iostream>

//указывает, что все переменные с плавающей точкой имеют тип “double”
#define PRES double

//полное описание исходных данных задачи
#define NXB 15				//число шагов сетки по оси X в одном сегменте
#define NX NXB*3+1			//число узлов сетки по оси X во всей структуре
#define NYB 12				//число шагов сетки по оси Y в одном сегменте
#define NY NYB*3+1			//число узлов сетки по оси Y во всей структуре
#define REP 3000			//максимальное число ПВР-итераций
#define DEL 100				//интервал записи распределения температуры для его анимации (запись происходит через каждые DEL временных шагов)
#define AMAT 1.1f			//температуропроводность материала
#define TEM1 5.0f			//температура контакта К1
#define TEM2 15.0f			//температура контакта К2
#define HX 0.2f				//шаг сетки
#define HY 0.3f				//шаг сетки

using namespace std;

int main(int argc, char **argv)
{
	int i, j, k;
	int idt = 0;
	int ndt = 0;
	int s = 0.25f;
	PRES T1 = TEM1, T2 = TEM1*2,T3 =TEM2, T4 = TEM1+TEM2, h = HX, r = HY, a = AMAT, t0;
	PRES T[NY][NX], TT[NY][NX];									//массив Т содержит искомое распределение, массив ТТ – рабочий
	PRES rr = min(h, r);										//меньший из шагов
	PRES tau = 0.25f*rr*rr / a;									//шаг по времени

	//коэффициенты, используемые на итерациях ПВР (коэффициенты в разностных формулах)
	PRES alf_1 = -h / r;
	PRES alf_2 = -r / h;
	PRES alf_3 = 0.5f * alf_2;
	PRES alf_4 = 0.5f * alf_1;
	PRES bet_1 = a * tau / (h*r);
	/*PRES bet_2 = 2.0f*bet_1;
	PRES bet_3 = 3.0f*bet_1;
	PRES bet_4 = 4.0f/3.0f*bet_1;*/
	PRES bet_3 = 2.0f*bet_1;
	PRES bet_2 = 4.0f / 3.0f*bet_1;
	PRES bet_4 = 4.0f*bet_1;
	//
	PRES gam_1 = -2.f*(alf_1 + alf_2);
	PRES gam_2 = -1.5f*(alf_1 + alf_2);
	PRES gam_3 = -(alf_1 + alf_2);
	PRES gam_4 = -(alf_3 + alf_4);

	char filename[128];								//имя записываемого на диск файла

	//начальное распределение температуры
	for (j = 0; j < NY; j++)
	{
		for (i = 0; i < NX; i++)
		{
			T[j][i] = 0.0f;
			TT[j][i] = 0.0f;
		}
	}
	int i1 = NXB; int i2 = i1 + NXB; int i3 = i2 + NXB;
	int j1 = NYB; int j2 = NYB * 2; int j3 = j1 * 3; int rp = REP;
	for (j = 0; j <= j2; j++) {
		for (i = 0; i <= i2; i++) {
			T[j][i] = 0.0f;
			TT[j][i] = 0.0f;
		}
	}
	for (j = j2; j <= j3; j++) {
		T[j][i3] = T4;
		TT[j][i3] = T4;
	}
	for (i = 0; i <=i1; i++) {
		T[0][i] = T2;
		TT[0][i] = T2;
	}
	for (i = i2; i < i3; i++) {
		T[j3][i] = T3;
		TT[j3][i] = T3;
	}
	for (j =0; j <= j1; j++) {
		T[j][0] = T1;
		TT[j][0] = T1;
	}
	//запись на диск начального распределения температуры
	ofstream fout("T1.dat", ios_base::out | ios_base::trunc | ios_base::binary);
	for (j = 0; j < NY; j++)
	{
		for (i = 0; i < NX; i++)
		{
			PRES w = T[j][i];
			fout.write((char*)&w, sizeof w);
		}
	}
	fout.close();

	k = 0;
	while (k < rp) {
		k++;
	/*	t0 = T[j][i];
		TT[j][i] = t0 - bet_4 * (alf_3*T[j][i + 1] + alf_4 * T[j + 1][i] + gam_4 * t0);
		for (j = 1; j <= j2 - j1; j++)
		{
			i = 0;
			t0 = T[j][i];
			TT[j][i] = t0 - bet_2 * (alf_4*(T[j - 1][i] + T[j + 1][i]) + alf_2 * T[j][i + 1] + gam_3 * t0);
		}*/
		for (j = 0; j <= j3; j++) {
			for (i = 0; i <= i3; i++) {
				t0 = T[j][i];
				if ((i == 30) && (j == 28))
				{
					cout << "t";
				}
				if (i == 0 && j == 1) {
					cout << "t" << endl;
				}
				if ((i == 0) && (j == 0))
				{
					TT[j][i] = t0 - bet_4 * (alf_3*T[j][i + 1] + alf_4 * T[j + 1][i] + gam_4 * t0);
					
				}
				else if ((i == 0) && (j > 0) && (j < j2))
				{
					TT[j][i] = t0 - bet_2 * (alf_4*T[j - 1][i] + alf_3 * T[j][i + 1] + alf_2 * T[j][i + 1] + alf_1 * T[j + 1][i] + gam_2 * t0);
					
				}
				else if (((i == i2) && (j > 0) && (j < j1)))
				{
					TT[j][i] = t0 - bet_2 * (alf_4*T[j - 1][i] + alf_3 * T[j][i + 1] + alf_2 * T[j][i - 1] + alf_1 * T[j + 1][i] + gam_2 * t0);
					
				}
				else if (i == i2 && j == j1)
				{
					TT[j][i] = t0 - bet_2 * (alf_4*T[j - 1][i] + alf_3 * T[j][i - 1] + alf_2 * T[j][i + 1] + alf_1 * T[j + 1][i] + gam_2 * t0);
					
				}
				else if (((j == j1) && (i > i1) && (i < i2)) || ((j == 0) && (i > 0) && (i < i1)))
				{
					TT[j][i] = t0 - bet_3 * (alf_3*(T[j][i + 1] + T[j][i - 1]) + alf_1 * T[j + 1][i] + gam_3 * t0);
					
				}
				else if (i == i1 && j == j1)
				{
					TT[j][i] = t0 - bet_3 * (alf_4*(T[j - 1][i] + T[j + 1][i]) + alf_2 * T[j][i + 1] + gam_3 * t0);
					
				}
				else if (((i == i1) && (j > 0) && (j < j1)) || ((i == i1) && (j > j2) && j < j3))
				{
					TT[j][i] = t0 - bet_3 * (alf_4*(T[j - 1][i] + T[j + 1][i]) + alf_2 * T[j][i - 1] + gam_3 * t0);
					
				}
				else if ((j == j2) && (i > i1) && (i < i3))
				{
					TT[j][i] = t0 - bet_3 * (alf_3*(T[j][i + 1] + T[j][i - 1]) + alf_1 * T[j - 1][i] + gam_3 * t0);
					
				}
				else if (i == i1 && j == j2)
				{
					TT[j][i] = t0 - bet_2 * (alf_4*T[j - 1][i] + alf_3 * T[j][i - 1] + alf_2 * T[j][i + 1] + alf_1 * T[j + 1][i] + gam_2 * t0);
					
				}
				else if (i == i1 && j == 0)
				{
					TT[j][i] = t0 - bet_4 * (alf_3*T[j][i - 1] + alf_4 * T[j + 1][i] + gam_4 * t0);
					
				}
				else if (((i > 0) && (i < i1) && (j > 0) && (j < j3)) || ((i >= i1) && (i < i3) && (j > j1) && (j < j2)) || ((i > i2) && (i < i3) && (j > 0) && (j <= j1)))
				{
					TT[j][i] = t0 - bet_1 * (alf_1*(T[j - 1][i] + T[j + 1][i]) + alf_2 * (T[j][i - 1] + T[j][i + 1]) + gam_1 * t0);
					
				}
				/*if ((i < i3) && (i > i2) && (j == 0))
				{
					TT[j][i] = t0 - bet_1 * (alf_3*(T[j][i - 1] + T[j][i + 1]) + alf_1 * T[j + 1][i] + gam_3 * t0);
				}
				else if ((i == i3) && (j > 0) && (j < j2))
				{
					TT[j][i] = t0 - bet_2 * (alf_4*(T[j - 1][i] + T[j + 1][i]) + alf_2 * T[j][i - 1] + gam_3 * t0);
				}
				else if (((i == i2) && (j > j2) && (j < j3)) || ((i == i1) && (j > j1) && (j < j3)))
				{
					TT[j][i] = t0 - bet_2 * (alf_4*(T[j - 1][i] + T[j + 1][i]) + alf_2 * T[j][i - 1] + gam_3 * t0);
				}
				else if (((j == j2) && (i > i1) && (i < i2)) || ((j == j1) && (i > 0) && (i < i1)))
				{
					TT[j][i] = t0 - bet_2 * (alf_3*(T[j][i + 1] + T[j][i - 1]) + alf_1 * T[j - 1][i] + gam_3 * t0);
				}
				else if (((i == i1) && (j == j1)) || ((i == i2) && (j == j2)))
				{
					TT[j][i] = t0 - bet_3 * (alf_1*T[j - 1][i] + alf_3 * T[j][i - 1] + alf_2 * T[j][i + 1] + alf_4 * T[j + 1][i] + gam_2 * t0);
				}
				else if ((i == i3) && (j == 0)) 
				{
					TT[j][i] = t0 - bet_4 * (alf_3*T[j][i - 1] + alf_4 * T[j + 1][i] + gam_4 * t0);
				}
				else if ((i == i1) && (j == j2))
				{
					TT[j][i] = t0 - bet_4 * (alf_4*T[j - 1][i] + alf_3 * T[j][i + 1] + gam_4 * t0);
				}
				else if (((i > 0) && (i < i3) && (j > 0) && (j < j1)) || ((i > i1) && (i < i3) && (j > j1) && (j < j2)) || ((i > i2) && (i < i3) && (j > j2) && (j < j3)))
				{
					TT[j][i] = t0 - bet_1 * (alf_1*(T[j - 1][i] + T[j + 1][i]) + alf_2 * (T[j][i - 1] + T[j][i + 1]) + gam_1 * t0);
				}*/
				//if (i == 15 && j == 12) {
				//	//	cout << "t" << endl;
				//}
				//if ((i == 1) && (j == 0)) {
				//	TT[j][i] = t0 - bet_4 * (alf_3*T[j][i + 1] + alf_4 * T[j + 1][i] + gam_4 * t0);
				//}
				//else if ((i == i2) && (j == 0)) {
				//	TT[j][i] = t0 - bet_4 * (alf_3*T[j][i - 1] + alf_4 * T[j + 1][i] + gam_4 * t0);
				//}
				//else if (((i == i2) && (j <= j1)) || ((i == i2 - i1) && (j > j1) && (j < j2)) || ((i == i2) && (j > j1) && (j < j2 - j1 + 1))) {
				//	TT[j][i] = t0 - bet_2 * (alf_4*(T[j - 1][i] + T[j + 1][i]) + alf_2 * T[j][i - 1] + gam_3 * t0);
				//}
				//else if ((i > i2 - i1) && (i < i1) && (j == j1)) {
				//	TT[j][i] = t0 - bet_2 * (alf_3*(T[j][i - 1] + T[j][i + 1]) + alf_1 * T[j - 1][i] + gam_3 * t0);
				//}
				//else if ((i == i1) && (j == j1)) {
				//	TT[j][i] = t0 - bet_3 * (alf_3*T[j][i - 1] + alf_4 * T[j + 1][i] + alf_2 * T[j][i + 1] + alf_1 * T[j - 1][i] + gam_2 * t0);
				//}
				//else if (((i == i1) && (j > j1) && (j < j2))) {
				//	TT[j][i] = t0 - bet_2 * (alf_4*(T[j - 1][i] + T[j + 1][i]) + alf_2 * T[j][i + 1] + gam_3 * t0);
				//}
				//else if ((j == 0) && (i > 0) && (i < i2)) {
				//	TT[j][i] = t0 - bet_2 * (alf_3*(T[j][i - 1] + T[j][i + 1]) + alf_1 * T[j + 1][i] + gam_3 * t0);
				//}
				//else if ((i == i2 - i1) && (j == j1)) {
				//	TT[j][i] = t0 - bet_3 * (alf_3*T[j][i + 1] + alf_4 * T[j + 1][i] + alf_2 * T[j][i - 1] + alf_1 * T[j - 1][i] + gam_2 * t0);
				//}
				//else if (((i < i2 - i1) && (j < j2)) && (j > 0) && (i > 0) || ((i >= i2 - i1) && (i < i2) && (j < j1)) && (j > 0) || ((i > i1) && (i < i2) && (j >= j1) && (j < j2))) {
				//	TT[j][i] = t0 - bet_1 * (alf_1*(T[j - 1][i] + T[j + 1][i]) + alf_2 * (T[j][i - 1] + T[j][i + 1]) + gam_1 * t0);
				//}

			}
		}
		//подготовка следующего шага по времени
		for (j = 0; j <= j3; j++)
		{
			for (i = 0; i <= i3; i++)
			{
				T[j][i] = TT[j][i];
			}
		}
		idt++;									//запись очередного распределения температуры
		if (idt == DEL)
		{
			cout << "good" << endl;
			idt = 0; ndt++;
			sprintf_s(filename, sizeof(filename), "T%d.dat", ndt + 1);
			ofstream fout(filename, ios_base::out | ios_base::trunc | ios_base::binary);
			for (j = 0; j <= j3; j++) {
				for (i = 0; i <= i3; i++) {
					PRES w = T[j][i];
					fout.write((char*)&w, sizeof w);
				}
			}
			fout.close();
		}
	}
	//запись файла со счетчиками
	int n_x = NX; int n_y = NY; int n_k = ndt;
	ofstream fou("Param.dat", ios_base::out | ios_base::trunc | ios_base::binary);
	fou.write((char*)&n_x, sizeof n_x);
	fou.write((char*)&n_y, sizeof n_y);
	fou.write((char*)&n_k, sizeof n_y);
	fou.close();
}