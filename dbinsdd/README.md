## DBダミーデータ生成ツール

created at 2011-04-28  
version 1.0.0-beta

ランダムデータ生成ルールを記述してDBにダミーデータを追加できる。PHPで動作。  
処理に時間が掛かる場合を考慮して進捗表示も行う。ブラウザ、コマンドラインどちらでも実行可能。

## 使い方

まず、dbinsdd.iniを編集し、DB環境と生成ルールを記述する。
「*」で始まる項目は予約語の為、記述しても無視される。

**記述例**
```
[*database]
*type = "mysql"
*username = "mysql"
*password = "12345678"
*charset = ""
host = "127.0.0.1"
dbname = "mysql"
```

他に渡したいパラメーターがある場合は、key = value形式で任意の数を指定する。
例えばODBCの場合、hostとdbnameを削除して
```
Driver = "{SQL Server}"
Server = "127.0.0.1\SQLEXPRESS"
Database = "testdb"
```
のように記述できる。

*charsetの部分はDBの文字コードを指定するが、DBの機能で文字コード変換処理を行う場合は指定は不要。  
また、DBの文字コードがUTF-8の場合は省略可。

次に、テーブル毎に生成ルールを記述する。
生成ルールは正規表現風のフォーマットで複雑なルールを設定できる。

**記述例**
```
[user]
*count = "100"
id = "{*newid}"
username = "[A-Za-z]{8}"
password = "[A-Za-z]{6,15}"
...

[admin]
*count = "5"
userid = "[a-z]{6,15}"
pass = "[0-9A-Za-z_]{8}"
date = "2010-01-01"
```

ブラウザでdbinsdd.phpにアクセスするか、コマンドラインでphp dbinsdd.phpで実行できる。
エラーがあれば全てロールバック。

## 課題

- 括弧の対応
- 項目数が多い場合を考慮したデフォルト生成ルール
- 複雑なリレーションへの対応
