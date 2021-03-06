DR_RE
=====
	This code is for our ACL2017 paper: "Jointly Extracting Relations with Class Ties via Effective Deep Ranking".

Description
-------
	This code contains three parts according to three loss functions in our paper.
	AVE: using average method to gain bag representation; 
	ATT: using sentence-level attention to get bag representation; 
	ATT+Multi: the extended version of ATT and achieves the best performance. 

Code Use
---------
	Take the code of ATT+Multi for example: 
	cd ATT+Multi 
	mkdir out
	make 
	./train 

	the results are in out/pr.txt

Data Fetch
------
You can obtain the data from [data](https://drive.google.com/open?id=0B8KyXw_n5ZQROUd4SngwcVFEYVU)[1][2]. 

Citation
----
	This code is based on the codes from [1], so if you want  to use this code, please cite the following papers:
	
	@InProceedings{
  	author = 	"Ye, Hai 
			and Chao, Wenhan
			and Luo, Zhunchen
			and Li, Zhoujun",
  	title = 	"Jointly Extracting Relations with Class Ties via Effective Deep Ranking",
  	booktitle = 	"Proceedings of the 55th Annual Meeting of the Association for      Computational Linguistics (Volume                         1: Long Papers)",
  	year = 	"2017",
  	publisher = 	"Association for Computational Linguistics"
	}
	
	@inproceedings{
  	author    = {	Yankai Lin and
        	        Shiqi Shen and
        	       	Zhiyuan Liu and
               		Huanbo Luan and
            	        Maosong Sun},
 	title     = {Neural Relation Extraction with Selective Attention over Instances},
  	booktitle = {Proceedings of the 54th Annual Meeting of the Association for Computational
                     Linguistics, {ACL} 2016, August 7-12, 2016, Berlin, Germany, Volume
                     1: Long Papers},
  	year      = {2016}
	}
	
Referrence
---------
	[1] Yankai Lin, Shiqi Shen, Zhiyuan Liu, Huanbo Luan, and Maosong Sun. 2016. Neural relation extraction with selective attention over instances. In Proceedings of ACL. volume 1, pages 2124–2133.
	[2] Sebastian Riedel, Limin Yao, and Andrew McCallum. 2010. Modeling relations and their mentions without labeled text. In Proceedings of ECML-PKDD. Springer, pages 148–163.

