#ifndef INIT_H
#define INIT_H
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <cstdio>
#include <float.h>
#include <cmath>

using namespace std;
int output_model = 0;

string version = "";

float wt = 0.5;

int num_threads = 12;
int trainTimes = 15;
float alpha = 0.03;
float reduce = 0.98;
int tt,tt1;
int dimensionC = 230;
int dimensionWPE = 5;
int window = 3;
int limit = 30;
float marginPositive = 2.5;
float marginNegative = 0.5;
float margin = 2;
float Belt = 0.001;
float *matrixB1, *matrixRelation, *matrixW1, *matrixRelationDao, *matrixRelationPr, *matrixRelationPrDao;
float *matrixB1_egs, *matrixRelation_egs, *matrixW1_egs, *matrixRelationPr_egs;
float *matrixB1_exs, *matrixRelation_exs, *matrixW1_exs, *matrixRelationPr_exs;
float *wordVecDao,*wordVec_egs,*wordVec_exs;
float *positionVecE1, *positionVecE2, *matrixW1PositionE1, *matrixW1PositionE2;
float *positionVecE1_egs, *positionVecE2_egs, *matrixW1PositionE1_egs, *matrixW1PositionE2_egs, *positionVecE1_exs, *positionVecE2_exs, *matrixW1PositionE1_exs, *matrixW1PositionE2_exs;
float *matrixW1PositionE1Dao;
float *matrixW1PositionE2Dao;
float *positionVecDaoE1;
float *positionVecDaoE2;
float *matrixW1Dao;
float *matrixB1Dao;
double mx = 0;
int batch = 16;
int npoch;
int len;
float rate = 1;
FILE *logg;

float *wordVec;
long long wordTotal, dimension, relationTotal;
long long PositionMinE1, PositionMaxE1, PositionTotalE1,PositionMinE2, PositionMaxE2, PositionTotalE2;
map<string,int> wordMapping;
vector<string> wordList;
map<string,int> relationMapping;
vector<int *> trainLists, trainPositionE1, trainPositionE2;
vector<int> trainLength;
vector<int> headList, tailList, relationList;
vector<int *> testtrainLists, testPositionE1, testPositionE2;
vector<int> testtrainLength;
vector<int> testheadList, testtailList, testrelationList;
vector<std::string> nam;

map<string, int> bags_train_name, bags_test_name;
map<string, int* > bags_train_head_tail, bags_test_head_tail;
map<string, map<int, string> > bags_train_relation, bags_test_relation;
map<string, vector<int*> > bags_train_sen, bags_test_sen;
map<string, vector<int> > bags_train_sen_len, bags_test_sen_len;
map<string, vector<int*> > bags_train_PE1, bags_test_PE1;
map<string, vector<int*> > bags_train_PE2, bags_test_PE2;


void init() {
	FILE *f = fopen("../data/data/vec.bin", "rb");
	fscanf(f, "%lld", &wordTotal);
	fscanf(f, "%lld", &dimension);
	cout<<"wordTotal=\t"<<wordTotal<<endl;
	cout<<"Word dimension=\t"<<dimension<<endl;
	PositionMinE1 = 0;
	PositionMaxE1 = 0;
	PositionMinE2 = 0;
	PositionMaxE2 = 0;
	wordVec = (float *)malloc((wordTotal+1) * dimension * sizeof(float));
	wordList.resize(wordTotal+1);
	wordList[0] = "UNK";
	for (int b = 1; b <= wordTotal; b++) {
		string name = "";
		while (1) {
			char ch = fgetc(f);
			if (feof(f) || ch == ' ') break;
			if (ch != '\n') name = name + ch;
		}
		long long last = b * dimension;
		float smp = 0;
		for (int a = 0; a < dimension; a++) {
			fread(&wordVec[a + last], sizeof(float), 1, f);
			smp += wordVec[a + last]*wordVec[a + last];
		}
		smp = sqrt(smp);
		for (int a = 0; a< dimension; a++)
			wordVec[a+last] = wordVec[a+last] / smp;
		wordMapping[name] = b;
		wordList[b] = name;
	}
	wordTotal+=1;
	fclose(f);
	char buffer[1000];
	f = fopen("../data/data/RE/relation2id.txt", "r");
	while (fscanf(f,"%s",buffer)==1) {
		int id;
		fscanf(f,"%d",&id);
		relationMapping[(string)(buffer)] = id;
		relationTotal++;
		nam.push_back((std::string)(buffer));
	}
	fclose(f);
	cout<<"relationTotal:\t"<<relationTotal<<endl;
	
	f = fopen("../data/data/RE/train.txt", "r");
	while (fscanf(f,"%s",buffer)==1)  {
		string e1 = buffer;
		fscanf(f,"%s",buffer);
		string e2 = buffer;
		fscanf(f,"%s",buffer);
		string head_s = (string)(buffer);
		int head = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		int tail = wordMapping[(string)(buffer)];
		string tail_s = (string)(buffer);
		fscanf(f,"%s",buffer);
		string entity = e1+"\t"+e2;

		if(bags_train_name.find(entity) == bags_train_name.end())
			bags_train_name[entity] = 1;
		else
			bags_train_name[entity] += 1;

		int num = relationMapping[(string)(buffer)];
		int len = 0, lefnum = 0, rignum = 0;
		std::vector<int> tmpp;
		while (fscanf(f,"%s", buffer)==1) {
			std::string con = buffer;
			if (con=="###END###") break;
			int gg = wordMapping[con];
			if (con == head_s) lefnum = len;
			if (con == tail_s) rignum = len;
			len++;
			tmpp.push_back(gg);
		}

		if (bags_train_head_tail.find(entity) == bags_train_head_tail.end())
		{
			int *h_t = (int *)calloc(2, sizeof(int));
			h_t[0] = head;
			h_t[1] = tail;
			bags_train_head_tail[entity] = h_t;
		}

		bags_train_relation[entity][num] = "None";
		bags_train_sen_len[entity].push_back(len);

		int *con=(int *)calloc(len,sizeof(int));
		int *conl=(int *)calloc(len,sizeof(int));
		int *conr=(int *)calloc(len,sizeof(int));
		for (int i = 0; i < len; i++) {
			con[i] = tmpp[i];
			conl[i] = lefnum - i;
			conr[i] = rignum - i;
			if (conl[i] >= limit) conl[i] = limit;
			if (conr[i] >= limit) conr[i] = limit;
			if (conl[i] <= -limit) conl[i] = -limit;
			if (conr[i] <= -limit) conr[i] = -limit;
			if (conl[i] > PositionMaxE1) PositionMaxE1 = conl[i];
			if (conr[i] > PositionMaxE2) PositionMaxE2 = conr[i];
			if (conl[i] < PositionMinE1) PositionMinE1 = conl[i];
			if (conr[i] < PositionMinE2) PositionMinE2 = conr[i];
		}

		bags_train_sen[entity].push_back(con);
		bags_train_PE1[entity].push_back(conl);
		bags_train_PE2[entity].push_back(conr);
	}
	fclose(f);

	f = fopen("../data/data/RE/test.txt", "r");	
	while (fscanf(f,"%s",buffer)==1)  {
		string e1 = buffer;
		fscanf(f,"%s",buffer);
		string e2 = buffer;

		fscanf(f,"%s",buffer);
		string head_s = (string)(buffer);
		int head = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);
		string tail_s = (string)(buffer);
		int tail = wordMapping[(string)(buffer)];
		fscanf(f,"%s",buffer);

		string entity = e1+"\t"+e2;

		if(bags_test_name.find(entity) == bags_test_name.end())
			bags_test_name[entity] = 1;
		else
			bags_test_name[entity] += 1;

		int num = relationMapping[(string)(buffer)];
		int len = 0 , lefnum = 0, rignum = 0;
		std::vector<int> tmpp;
		while (fscanf(f,"%s", buffer)==1) {
			std::string con = buffer;
			if (con=="###END###") break;
			int gg = wordMapping[con];
			if (head_s == con) lefnum = len;
			if (tail_s == con) rignum = len;
			len++;
			tmpp.push_back(gg);
		}

		if (bags_test_head_tail.find(entity) == bags_test_head_tail.end())
		{
			int* h_t = (int *)calloc(2, sizeof(int));
			h_t[0] = head;
			h_t[1] = tail;
			bags_test_head_tail[entity] = h_t;
		}
		bags_test_relation[entity][num] = "None";
		bags_test_sen_len[entity].push_back(len);
		int *con=(int *)calloc(len,sizeof(int));
		int *conl=(int *)calloc(len,sizeof(int));
		int *conr=(int *)calloc(len,sizeof(int));
		for (int i = 0; i < len; i++) {
			con[i] = tmpp[i];
			conl[i] = lefnum - i;
			conr[i] = rignum - i;
			if (conl[i] >= limit) conl[i] = limit;
			if (conr[i] >= limit) conr[i] = limit;
			if (conl[i] <= -limit) conl[i] = -limit;
			if (conr[i] <= -limit) conr[i] = -limit;
			if (conl[i] > PositionMaxE1) PositionMaxE1 = conl[i];
			if (conr[i] > PositionMaxE2) PositionMaxE2 = conr[i];
			if (conl[i] < PositionMinE1) PositionMinE1 = conl[i];
			if (conr[i] < PositionMinE2) PositionMinE2 = conr[i];
		}
		bags_test_sen[entity].push_back(con);
		bags_test_PE1[entity].push_back(conl);
		bags_test_PE2[entity].push_back(conr);
	}
	fclose(f);
	cout<<PositionMinE1<<' '<<PositionMaxE1<<' '<<PositionMinE2<<' '<<PositionMaxE2<<endl;

	for (map<string, int> :: iterator bag = bags_train_name.begin(); bag != bags_train_name.end(); bag++)
	{
		string bag_name = bag->first;
		int bag_size = bag->second;
		vector<int *> ps1 = bags_train_PE1[bag_name];
		vector<int *> ps2 = bags_train_PE2[bag_name];
		for (int i = 0; i < bag_size; i++)
		{
			int *work1 = ps1[i];
			int len = bags_train_sen_len[bag_name][i];
			for (int j = 0; j < len; j++)
				work1[j] = work1[j] - PositionMinE1;
			int *work2 = ps2[i];
			for (int j = 0; j < len; j++)
				work2[j] = work2[j] - PositionMinE2;
		}
	}
	for (map<string, int> :: iterator bag = bags_test_name.begin(); bag != bags_test_name.end(); bag++)
	{
		string bag_name = bag->first;
		int bag_size = bag->second;
		vector<int *> ps1 = bags_test_PE1[bag_name];
		vector<int *> ps2 = bags_test_PE2[bag_name];
		for (int i = 0; i < bag_size; i++)
		{
			int *work1 = ps1[i];
			int len = bags_test_sen_len[bag_name][i];
			for (int j = 0; j < len; j++)
				work1[j] = work1[j] - PositionMinE1;
			int *work2 = ps2[i];
			for (int j = 0; j < len; j++)
				work2[j] = work2[j] - PositionMinE2;
		}
	}
	PositionTotalE1 = PositionMaxE1 - PositionMinE1 + 1;
	PositionTotalE2 = PositionMaxE2 - PositionMinE2 + 1;
}

float CalcTanh(float con) {
	if (con > 20) return 1.0;
	if (con < -20) return -1.0;
	float sinhx = exp(con) - exp(-con);
	float coshx = exp(con) + exp(-con);
	return sinhx / coshx;
}

float tanhDao(float con) {
	float res = CalcTanh(con);
	return 1 - res * res;
}

float sigmod(float con) {
	if (con > 20) return 1.0;
	if (con < -20) return 0.0;
	con = exp(con);
	return con / (1 + con);
}

int getRand(int l,int r) {
	int len = r - l;
	int res = rand()*rand() % len;
	if (res < 0)
		res+=len;
	return res + l;
}

float getRandU(float l, float r) {
	float len = r - l;
	float res = (float)(rand()) / RAND_MAX;
	return res * len + l;
}

void norm(float* a, int ll, int rr)
{
	float tmp = 0;
	for (int i=ll; i<rr; i++)
		tmp+=a[i]*a[i];
	if (tmp>1)
	{
		tmp = sqrt(tmp);
		for (int i=ll; i<rr; i++)
			a[i]/=tmp;
	}
}

void norm(vector<double> &a)
{
	double tmp = 0;
	for (int i=0; i<a.size(); i++)
		tmp+=a[i];
	{
		for (int i=0; i<a.size(); i++)
			a[i]/=tmp;
	}
}

#endif
