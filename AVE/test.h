#ifndef TEST_H
#define TEST_H
#include "init.h"
#include <algorithm>
#include <map>
#include <iostream>
#include <string>

int tipp = 0;
float ress = 0;

vector<double> test(int * sentence, int * testPositionE1, int * testPositionE2, int len, vector<float>  &r) {
		
	for (int i = 0; i < dimensionC; i++) {
		int last = i * dimension * window;
		int lastt = i * dimensionWPE * window;
		float mx[3];
		int ti[3];
		for (int i1 = 0; i1<3; i1++)
			mx[i1] = -FLT_MAX;
		int i2 = 0;
		for (int i1 = -window+1; i1 < len; i1++) 
		{
			float res = 0;
			int tot = 0;
			int tot1 = 0;
			for (int j = i1; j < i1 + window; j++)  
			if (j>=0&&j<len)
			{
				int last1 = sentence[j] * dimension;
			 	for (int k = 0; k < dimension; k++) {
			 		res += matrixW1[last + tot] * wordVec[last1+k];
			 		tot++;
			 	}
			 	int last2 = testPositionE1[j] * dimensionWPE;
			 	int last3 = testPositionE2[j] * dimensionWPE;
			 	for (int k = 0; k < dimensionWPE; k++) {
			 		res += matrixW1PositionE1[lastt + tot1] * positionVecE1[last2+k];
			 		res += matrixW1PositionE2[lastt + tot1] * positionVecE2[last3+k];
			 		tot1++;
			 	}
			}
			else
			{
				tot+=dimension;
				tot1+=dimensionWPE;
			}
			if (res > mx[i2]) {
				mx[i2] = res;
				ti[i2] = i1;
			}
			if (i1>=0&&testPositionE1[i1]==-PositionMinE1)
				i2++;
			if (i1>=0&&testPositionE2[i1]==-PositionMinE2)
				i2++;
			assert(i2<3);
		}
		assert(i2==2);
		for (int i1 = 0; i1<3; i1++)
		{
			r.push_back(mx[i1]+matrixB1[3*i+i1]);
		}
	}
	for (int i = 0; i < 3 * dimensionC; i++)
		r[i] = CalcTanh(r[i]);
	vector<double> res;
	double tmp = 0;
	for (int j = 0; j < relationTotal; j++) {
		float s = 0;
		for (int i = 0; i < 3 * dimensionC; i++)
			s +=  0.5 * matrixRelation[j * 3 * dimensionC + i] * r[i];
		s += matrixRelationPr[j];
		s = exp(s);
		tmp+=s;  
		res.push_back(s);
	}
	for (int j = 0; j < relationTotal; j++) 
		res[j]/=tmp;
	return res;
}

bool cmp(pair<string, pair<int,double> > a,pair<string, pair<int,double> >b)
{
    return a.second.second>b.second.second;
}

vector<string> b;
double tot;
vector<pair<string, pair<int,double> > >aa;

pthread_mutex_t mutex;
vector<int> ll_test;

void* testMode(void *id ) 
{
	int ll = ll_test[(long long)id];
	int rr;
	if ((long long)id==num_threads-1)
		rr = b.size();
	else
		rr = ll_test[(long long)id+1];
	vector<float> r;
	double eps = 0.1;
	for (int ii = ll; ii < rr; ii++)
	{
		vector<double> sum;
		vector<double> r_sum;
		r_sum.resize(3 * dimensionC);
		for (int j = 0; j < relationTotal; j++)
			sum.push_back(0.0);
		map<int,int> ok;
		ok.clear();
		vector<vector<double> > rList;
		int bags_size = bags_test_name[b[ii]];
		for (int k=0; k<bags_size; k++)
		{
			r.clear();
			vector<double> score = test(bags_test_sen[b[ii]][k], bags_test_PE1[b[ii]][k], bags_test_PE2[b[ii]][k], bags_test_sen_len[b[ii]][k], r);
			vector<double> r_tmp;
			for (int j = 0; j < 3 * dimensionC; j++)
				r_tmp.push_back(r[j]);
			rList.push_back(r_tmp);
		}
		for (int j = 0; j < relationTotal; j++) {
			vector<float> weight;
			float weight_sum = 0;
			for (int k=0; k<bags_size; k++)
				weight.push_back(1./bags_size);
			vector<float> r;
			r.resize(3 * dimensionC);
			for (int i = 0; i < 3 * dimensionC; i++) 
				for (int k=0; k<bags_size; k++)
					r[i] += rList[k][i] * weight[k];
			vector<float> res;
			double tmp = 0;
			for (int j1 = 0; j1 < relationTotal; j1++) {
				float s = 0;
				for (int i1 = 0; i1 < 3 * dimensionC; i1++)
					s +=  0.5 * matrixRelation[j1 * 3 * dimensionC + i1] * r[i1];
				s += matrixRelationPr[j1];
				res.push_back(s);
			}
			sum[j]= (double)res[j];
		}
		pthread_mutex_lock (&mutex);
		for (int j = 1; j < relationTotal; j++) 
		{
			int tag = 1;
			if (bags_test_relation[b[ii]].find(j) == bags_test_relation[b[ii]].end())
				tag = 0;
			aa.push_back(make_pair(wordList[bags_test_head_tail[b[ii]][0]] + ' ' + wordList[bags_test_head_tail[b[ii]][1]] + ' ' + nam[j], make_pair(tag, sum[j])));
		}
		pthread_mutex_unlock(&mutex);
	}

}

double max_pre = 0;

void test() {
	aa.clear();
	b.clear();
	tot = 0;
	ll_test.clear();
	vector<int> b_sum;
	b_sum.clear();
	for (map<string, int>:: iterator it = bags_test_name.begin(); it!=bags_test_name.end(); it++)
	{
		map<int, string> relations = bags_test_relation[it->first];
		for (map<int, string>:: iterator hh = relations.begin(); hh != relations.end(); hh++)
		{
			if (hh->first != 0)
				tot++;
		}
		{
			b.push_back(it->first);
			b_sum.push_back((int)it->second);
		}
	}
	for (int i=1; i<b_sum.size(); i++)
		b_sum[i] += b_sum[i-1];
	int now = 0;
	ll_test.resize(num_threads);
	for (int i=0; i<b_sum.size(); i++)
		if (b_sum[i]>=b_sum[b_sum.size()-1]/num_threads*now)
		{
			ll_test[now] = i;
			now+=1;
		}
	cout<<"tot:\t"<<tot<<endl;
	pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
	for (int a = 0; a < num_threads; a++)
		pthread_create(&pt[a], NULL, testMode,  (void *)a);
	for (int a = 0; a < num_threads; a++)
		pthread_join(pt[a], NULL);
	sort(aa.begin(),aa.end(),cmp);
	double correct=0;
	int output_flag = 0;
	float correct1 = 0;
	double sum_pre = 0;
	for (int i=0; i<min(2000,int(aa.size())); i++)
	{
		if (aa[i].second.first!=0)
			correct1++;	
		float precision = correct1/(i+1);
		float recall = correct1/tot;
		if (i%100==0)
		{
			cout<<"precision:\t"<<correct1/(i+1)<<'\t'<<"recall:\t"<<correct1/tot<<endl;
			if (i>=0&&i<=500)
				sum_pre += precision;
		}
	}
	if (sum_pre>max_pre)
	{
		max_pre = sum_pre;
		output_flag = 1;
	}
	{
		FILE* f = fopen("out/pr.txt", "at");
		for (int i=0; i<min(2000,int(aa.size())); i++)
		{
			if (aa[i].second.first!=0)
				correct++;	
			fprintf(f,"%lf\t%lf\t%lf\t%s\n",correct/(i+1), correct/tot,aa[i].second.second, aa[i].first.c_str());
		}
		fprintf(f, "****************\n\n");
		fclose(f);
		if (!output_model)
			return;
		FILE *fout = fopen(("./out/matrixW1+B1"+version+".txt").c_str(), "w");
		fprintf(fout,"%d\t%d\t%d\t%d\n", dimensionC, dimension, window, dimensionWPE);
		for (int i = 0; i < dimensionC; i++) {
			for (int j = 0; j < dimension * window; j++)
				fprintf(fout, "%f\t",matrixW1[i* dimension*window+j]);
			for (int j = 0; j < dimensionWPE * window; j++)
				fprintf(fout, "%f\t",matrixW1PositionE1[i* dimensionWPE*window+j]);
			for (int j = 0; j < dimensionWPE * window; j++)
				fprintf(fout, "%f\t",matrixW1PositionE2[i* dimensionWPE*window+j]);
			for (int j=0; j<3; j++)
				fprintf(fout, "%f\t", matrixB1[3*i+j]);
			fprintf(fout, "\n");
		}
		fprintf(fout,"%f\n", wt);
		fclose(fout);

		fout = fopen(("./out/matrixRl" + version+".txt").c_str(), "w");
		fprintf(fout,"%d\t%d\n", relationTotal, dimensionC);
		for (int i = 0; i < relationTotal; i++) {
			for (int j = 0; j < 3 * dimensionC; j++)
				fprintf(fout, "%f\t", matrixRelation[3 * i * dimensionC + j]);
			fprintf(fout, "\n");
		}
		for (int i = 0; i < relationTotal; i++) 
			fprintf(fout, "%f\t",matrixRelationPr[i]);
		fprintf(fout, "\n");
		fclose(fout);

		fout = fopen(("./out/matrixPosition"+version+".txt").c_str(), "w");
		fprintf(fout,"%d\t%d\t%d\n", PositionTotalE1, PositionTotalE2, dimensionWPE);
		for (int i = 0; i < PositionTotalE1; i++) {
			for (int j = 0; j < dimensionWPE; j++)
				fprintf(fout, "%f\t", positionVecE1[i * dimensionWPE + j]);
			fprintf(fout, "\n");
		}
		for (int i = 0; i < PositionTotalE2; i++) {
			for (int j = 0; j < dimensionWPE; j++)
				fprintf(fout, "%f\t", positionVecE2[i * dimensionWPE + j]);
			fprintf(fout, "\n");
		}
		fclose(fout);
		fout = fopen(("./out/word2vec"+version+".txt").c_str(), "w");
		fprintf(fout,"%d\t%d\n",wordTotal,dimension);
		for (int i = 0; i < wordTotal; i++)
		{
			for (int j=0; j<dimension; j++)
				fprintf(fout,"%f\t",wordVec[i*dimension+j]);
			fprintf(fout,"\n");
		}
		fclose(fout);
	}
}

#endif
