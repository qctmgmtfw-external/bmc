#!/bin/sh
echo 'Multiple Language Support for ZH-TW'
sed -i '/\/\/##LANG_TAGS/{r lang_zh_tw.tag
d}' ./app/config.js
sed -i '/\/\/##LANG_TAGS/{r lang_runtime_support.tag
d}' ./app/main.js
