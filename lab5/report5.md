## 编译原理实验5

211220127 顾嘉宇

### 实验目标

实现中间代码的优化，填写完给定代码框架中的TODO

### 实现方式

`src/IR_optimze/solver.c`:

将forward中的对应的prec改为succ即可。`src/IR_optimze/available_expressions_analysis.c`:

newBoundaryFact中返回false，newInitalFact中返回true。

meetInto中使用intersect_with，即并集。

`src/IR_optimze/constant_propagation.c`:

meetValue中根据手册返回对应的CPValue。

calculateValue中除了给的提示代码外，若存在某一个为NAC，则返回NAC，最后若全不满足则返回UNDEF

常量传播是前向的，且函数参数在newBoundary初始化为非常量

`src/IR_optimze/copy_propagation.c`:

是前向的，且初始化时为true，边界时为false。

kill时new_def存在于哪里就从哪里删去

`src/IR_optimze/live_variable_analysis.c`:

后向分析流

meetInto使用并集操作

先Kill再gen

如果在后续块中未被使用，则标记为死代码。

### 编译&测试方式

在命令行输入make获得parser，运行命令./parser input.ir output.ir即可获得输出文件。

### 感想

终于把实验做完了，最后一个实验相比于前四个算是非常友善了，可惜的就是我最后只拿了90分，想debug都不知道从何着手，所以想了想就这样吧，也算是把实验圆满收工了。