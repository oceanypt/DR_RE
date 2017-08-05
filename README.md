DR_RE
=====
This code is for our ACL2017 paper: "Jointly Extracting Relations with Class Ties via Effective Deep Ranking". <br>

Description
-------
This code contains three parts according to three loss functions in our paper.<br>
AVE: using average method to gain bag representation; <br>
ATT: using sentence-level attention to get bag representation; <br>
ATT+Multi: the extended version of ATT and achieves the best performance. <br>

Code Use
---------
Take the code of ATT+Multi for example:
cd ATT+Multi
make
./train

the results are in ./out/pr.txt

Data Fetch
------
You can obtain the data from https://drive.google.com/open?id=0B8KyXw_n5ZQROUd4SngwcVFEYVU. 



