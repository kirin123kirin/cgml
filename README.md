# Easy Network Diagram

[![Cmake Auto Build](https://github.com/kirin123kirin/cgml/actions/workflows/cmake.yml/badge.svg)](https://github.com/kirin123kirin/cgml/actions/workflows/cmake.yml)

# Usage
1. Copy Tab Separated data.

2. Command Prompt or PowerShell
```
> cgml
```

3. [Shift] + [Alt] + B -> [Enter]

# Install
1. yEd Graph Editorをインストールする
https://www.yworks.com/products/yed/download

2. [Releaseの最新バージョン](https://github.com/kirin123kirin/cgml/releases/latest)からcgml.exe.gzをダウンロードする

3. ダウンロードしたcgml.exe.gzをyEdインストールディレクトリに解凍する

4. yEdインストールディレクトリを環境変数PATHに追加する

# Environment
* Windows

# Requirements
* [yEd Graph Editor](https://www.yworks.com/products/yed)
* JRE

# Libraries
* [cxxopts | MIT Liscence](https://github.com/jarro2783/cxxopts/raw/master/LICENSE)
* [csv-parser | MIT Liscence](https://github.com/AriaFallah/csv-parser/raw/master/LICENSE)


<!-- Example Windows Command Prompt

set CGML="C:\temp"
cd %CGML%

powershell

Invoke-WebRequest -UseBasicParsing -Uri "https://github.com/kirin123kirin/cgml/releases/download/untagged-7f4ba9e7642206440d97/art.zip" -OutFile "${ENV:TEMP}\art.zip"

Expand-Archive -Force -Path "${ENV:TEMP}\art.zip" -DestinationPath .

exit

setx PATH %PATH;%CGML%

del %TEMP%\art.zip

-->
