# AutoPrint #

云打印功能，外网上的print文件夹接收doc文档，本地抓取该文件夹内文档并打印

## crawlDoc ##
抓取文档，并且在printed文件夹备份打印文档<br/><br/>
1. 自动打印文档不超过100KB<br/>
2. print文件夹下放置**<寝室号>**文件夹，文件夹内存放打印文档<br/>
3. **<寝室号>**文件夹从print文件夹内消失证明打印结束<br/>
4. 每10秒爬取一次print文件夹

## sendToPrinter ##
C#实现打印doc<br/>
封装成dll供crawlDoc使用