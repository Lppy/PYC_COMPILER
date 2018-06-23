Linux环境下：
执行 make 命令即可编译生成二进制文件./parser。
执行 ./parser 即可在命令行下输入我们的类C语言（以#结束）。（text.txt中包含许多例子）
执行 ./parser < xxx.pyc 即可编译我们的类C语言的文件。（test.pyc为一个较复杂的测试用类C语言文件）
执行 ./parser 成功后，输出的中间语法树JSON文件位于 visualization/data.json 中。
阅读 visualization/Readme.txt 以可视化查看JSON格式的中间语法树。
执行 ./parser 成功后，输出的汇编代码为dosx86.asm，在DOSBOX中使用MASM汇编器即可编译运行。
