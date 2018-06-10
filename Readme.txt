Linux环境下：
执行 make 命令即可编译生成二进制文件./parser。
执行 ./parser 即可在命令行下输入我们的类C语言（以#结束）。
执行 ./parser < xxx.c 即可编译我们的类C语言的文件。
执行 ./parser 成功后，输出的中间语法树JSON文件位于 visualization/data.json 中。
阅读 visualization/Readme.txt 以可视化查看JSON格式的中间语法树。