# 哈夫曼编码压缩软件
声明
------------------

·BM算法是原封不动照搬的Google上的: <https://www.tutorialandexample.com/boyer-moore-algorithm/>  

·GBK转UTF-8的函数用了这位老哥写的库<https://github.com/breezechen/sconv>  

特性  
------------------
·支持全球任何语言(UTF-8编码)  

·对于长达数十万字的文章，压缩、解压速度极快(1秒之内)，压缩率基本达到最高。   
	
·对于中文，无论UTF8还是ANSI(GBK-2312)编码均支持，且能自动识别。  
	
·使用哈希表优化频数读取、优化写文件速度、优化解压速度  
	
·使用最小堆优化哈夫曼树生成速度  
	
·码表的每8个01字符也压缩为一个字节，进一步提高压缩率  
	
·在压缩文件中搜索时，不仅可返回在二进制文件中的位置，还能返回在原文中的行数、列数  
	
演示
-----------------
[![](http://shrike.site/pic/红楼梦.png)](http://shrike.site/pic/红楼梦.png)
[![](http://shrike.site/pic/三体.png)](http://shrike.site/pic/三体.png)
[![](http://shrike.site/pic/三体找水滴.png)](http://shrike.site/pic/三体找水滴.png)
[![](http://shrike.site/pic/挪威的森林.png)](http://shrike.site/pic/挪威的森林.png)
